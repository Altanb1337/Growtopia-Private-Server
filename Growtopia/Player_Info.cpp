#pragma warning (disable : 4018)
#pragma warning (disable : 4244)

#define STRINT(x, y) (*(int*)(&(x)[(y)]))
#define STR16(x, y) (*(uint16_t*)(&(x)[(y)]))

#include "stdafx.h"
#include "Player_Info.h"
#include <string>
#include <vector>
#include <fstream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

int getState(ENetPeer* peer) {
	PlayerInfo* p = (PlayerInfo*)(peer->data);
	int i = 0;

	i |= p->noclip << 0;
	i |= p->doubleJump << 1;

	return i;
}

string Player::filterName(string str) {
	string newStr, ret, ret2;

	for (char c : str) newStr += (c >= 'A' && c <= 'Z') ? c - ('A' - 'a') : c;

	for (int i = 0; i < newStr.length(); i++) {
		if (newStr[i] == '`') i++;

		else ret += newStr[i];
	}

	for (char c : ret) if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) ret2 += c;

	string username = ret2;

	if (find(blacklistedName.begin(), blacklistedName.end(), username) != blacklistedName.end()) return "";

	return ret2;
}

string Player::fixColor(string str) {
	string ret;
	int colorLevel = 0;

	for (int i = 0; i < str.length(); i++) {
		if (str[i] == '`') {
			ret += str[i];

			if (i + 1 < str.length()) ret += str[i + 1];

			if (i + 1 < str.length() && str[i + 1] == '`') colorLevel--;

			else colorLevel++;

			i++;
		}
		else ret += str[i];
	}

	for (int i = 0; i < colorLevel; i++) ret += "``";

	for (int i = 0; i > colorLevel; i--) ret += "`w";

	return ret;
}

void Player::login(ENetPeer* peer, string username, string password) {

	if (username.length() < 3) throw -1;

	ifstream ifs("players/" + username + ".json");

	if (ifs.fail()) {
		throw -1;
		return;
	}
	
	json j;
	ifs >> j;

	string pass = j["password"];

	if (password != pass) {
		throw -1;
		return;
	}

	((PlayerInfo*)(peer->data))->loggedOn = true;

	for (ENetPeer* currentPeer : peers) {
		if (currentPeer == peer)
			continue;

		if (((PlayerInfo*)(currentPeer->data))->rawName == username) {
			Message::sendConsole(currentPeer, "`oSomeone else has logged on into this account!");
			Message::sendConsole(peer, "`oSomeone else was logged on into this account. He/she was kicked now");
			enet_peer_disconnect_later(currentPeer, 0);
		}
	}
}

void Player::setGrowID(ENetPeer* peer, string tankIDName, string password) {
	GamePacket p;
	p.extend("SetHasGrowID");
	p.extend((int)((PlayerInfo*)(peer->data))->registered);
	p.extend(tankIDName);
	p.extend(password);

	p.send(peer);
}

void Player::createAccount(ENetPeer* peer, string username, string password, string passwordVerify, string email) {
	string name = filterName(username);

	if (ifstream("players/" + name + ".json").is_open()) {
		throw - 1;
		return;
	}

	if (find(blacklistedName.begin(), blacklistedName.end(), name) != blacklistedName.end()) {
		throw - 2;
		return;
	}

	if (email.find('@') == string::npos) {
		throw - 3;
		return;
	}
	else if (username.length() < 3) {
		throw - 4;
		return;
	}
	else if (username.length() < 4) {
		throw - 5;
		return;
	}
	else if (password != passwordVerify) {
		throw - 6;
		return;
	}

	ofstream ofs("players/" + name + ".json");
	json j;
	
	j["tankIDName"] = username;
	j["email"] = email;
	j["adminLevel"] = { PLAYER };
	j["password"] = password;

	ofs << j;

	Message::sendConsole(peer, "`oYour account has been `2created`o! Disconnecting...");
	setGrowID(peer, username, password);

	enet_peer_disconnect_later(peer, 0);
}

unsigned char* packPlayerMoving(PlayerMoving* dataStruct) {
	unsigned char* data = new unsigned char[56];

	for (int i = 0; i < 56; i++) data[i] = 0;

	memcpy(data, &dataStruct->packetType, 4);
	memcpy(data + 4, &dataStruct->netID, 4);
	memcpy(data + 12, &dataStruct->characterState, 4);
	memcpy(data + 20, &dataStruct->plantingTree, 4);
	memcpy(data + 24, &dataStruct->x, 4);
	memcpy(data + 28, &dataStruct->y, 4);
	memcpy(data + 32, &dataStruct->XSpeed, 4);
	memcpy(data + 36, &dataStruct->YSpeed, 4);
	memcpy(data + 44, &dataStruct->punchX, 4);
	memcpy(data + 48, &dataStruct->punchY, 4);

	return data;
}

PlayerMoving* unpackPlayerMoving(unsigned char* data) {
	PlayerMoving* dataStruct = new PlayerMoving;
	memcpy(&dataStruct->packetType, data, 4);
	memcpy(&dataStruct->netID, data + 4, 4);
	memcpy(&dataStruct->characterState, data + 12, 4);
	memcpy(&dataStruct->plantingTree, data + 20, 4);
	memcpy(&dataStruct->x, data + 24, 4);
	memcpy(&dataStruct->y, data + 28, 4);
	memcpy(&dataStruct->XSpeed, data + 32, 4);
	memcpy(&dataStruct->YSpeed, data + 36, 4);
	memcpy(&dataStruct->punchX, data + 44, 4);
	memcpy(&dataStruct->punchY, data + 48, 4);

	return dataStruct;
}

void sendPacketRaw(ENetPeer* peer, int a1, void* packetData, size_t packetSize, void* a4, int flag) {
	if (a1 == 4 && *((unsigned char*)packetData + 12) & 8) {
		ENetPacket* p = enet_packet_create(0, packetSize + *((DWORD*)packetData + 13) + 5, flag);
		int i = 4;

		memcpy(p->data, &i, 4);
		memcpy((char*)p->data + 4, packetData, packetSize);
		memcpy((char*)p->data + packetSize + 4, a4, *((DWORD*)packetData + 13));
		enet_peer_send(peer, 0, p);
	}
	else {
		ENetPacket* p = enet_packet_create(0, packetSize + 5, flag);

		memcpy(p->data, &a1, 4);
		memcpy((char*)p->data + 4, packetData, packetSize);
		enet_peer_send(peer, 0, p);
	}

	delete (char*)packetData;
}

void sendState(ENetPeer* peer) {
	PlayerInfo* p = (PlayerInfo*)(peer->data);

	int netID = p->netID, state = getState(peer);

	for (ENetPeer* currentPeer : peers) {
		if (isHere(peer, currentPeer)) {
			PlayerMoving data;

			data.packetType = 0x14;
			data.characterState = 0;
			data.x = 1000;
			data.y = 100;
			data.punchX = 0;
			data.punchY = 0;
			data.XSpeed = 300;
			data.YSpeed = 600;
			data.netID = netID;
			data.plantingTree = state;

			unsigned char* raw = packPlayerMoving(&data);
			int hex = 0x808000;

			memcpy(raw + 1, &hex, 3);
			float waterSpeed = 125.0f;

			memcpy(raw + 16, &waterSpeed, 4);
			sendPacketRaw(currentPeer, 4, raw, 56, 0, ENET_PACKET_FLAG_RELIABLE);
		}
	}
}

string packPlayerMoving2(PlayerMoving* dataStruct) {
	string data;

	data.resize(56);
	STRINT(data, 0) = dataStruct->packetType;
	STRINT(data, 4) = dataStruct->netID;
	STRINT(data, 12) = dataStruct->characterState;
	STRINT(data, 20) = dataStruct->plantingTree;
	STRINT(data, 24) = *(int*)&dataStruct->x;
	STRINT(data, 28) = *(int*)&dataStruct->y;
	STRINT(data, 32) = *(int*)&dataStruct->XSpeed;
	STRINT(data, 36) = *(int*)&dataStruct->YSpeed;
	STRINT(data, 44) = dataStruct->punchX;
	STRINT(data, 48) = dataStruct->punchY;

	return data;
}

void sendNothing(ENetPeer* peer, int x, int y) {
	PlayerMoving data;

	data.netID = ((PlayerInfo*)(peer->data))->netID;
	data.packetType = 0x8;
	data.plantingTree = 0;
	data.netID = -1;
	data.x = x;
	data.y = y;
	data.punchX = x;
	data.punchY = y;

	sendPacketRaw(peer, 4, packPlayerMoving(&data), 56, 0, ENET_PACKET_FLAG_RELIABLE);
}

void sendPacketRaw3(ENetPeer* peer, int a1, void* packetData, size_t packetSize, void* a4, int flag) {
	ENetPacket* p = enet_packet_create(nullptr, packetSize + 5, flag);

	memcpy(p->data, &a1, 4);
	memcpy((char*)p->data + 4, packetData, packetSize);
	enet_peer_send(peer, 0, p);

	delete (char*)packetData;
}

void Player::leaveWorld(ENetPeer* peer) {
	PlayerInfo* p = (PlayerInfo*)(peer->data);

	GamePacket packet;
	packet.extend("OnRemove");
	packet.extend("netID|" + to_string(p->netID) + "\n");

	packet.send(peer);

	for (ENetPeer* currentPeer : peers) {
		if (isHere(peer, currentPeer)) packet.send(currentPeer);
	}
}

void Player::joinWorld(ENetPeer* peer, string worldName, int x, int y) {
	PlayerInfo* p = (PlayerInfo*)(peer->data);

	if (p->currentWorld != "EXIT") leaveWorld(peer);

	WorldInfo world = WorldDB::get(worldName);

	sendWorld(peer, &world);

	int enterPositionX = 3040, enterPositionY = 736;

	for (int i = 0; i < world.width * world.height; i++) {
		if (getItem(world.items[i].foreground).blockType == BlockType::WHITE_DOOR) {
			enterPositionX = (i % world.width) * 32;
			enterPositionY = (i / world.width) * 32;
		}
	}

	if (x != 0 && y != 0) {
		enterPositionX = x;
		enterPositionY = y;
	}

	p->x = enterPositionX;
	p->y = enterPositionY;

	spawn(peer, peer);

	for (ENetPeer* currentPeer : peers) {
		if (peer == currentPeer) continue;

		if (isHere(peer, currentPeer)) {
			spawn(currentPeer, peer);
		}
	}

	sendInventory(peer, p->inventory);
}

void Player::spawn(ENetPeer* peer, ENetPeer* currentPeer) {
	PlayerInfo* p = (PlayerInfo*)(peer->data);

	string str = "spawn|avatar\nnetID|" + to_string(p->netID);
	str.append("\nuserID|" + to_string(p->netID));
	str.append("\ncolrect|0|0|20|30");
	str.append("\nposXY|" + to_string(p->x) + "|" + to_string(p->y));
	str.append("\nname|" + p->displayName);
	str.append("\ncountry|" + p->country);
	str.append("\ninvis|0\nmstate|0\nsmstate|0\ntype|local\n");

	GamePacket packet;
	packet.extend("OnSpawn");
	packet.extend(str);
	packet.send(currentPeer);
}

void Player::sendAction(ENetPeer* peer, string action) {
	GamePacket p(0, ((PlayerInfo*)(peer->data))->netID);

	for (ENetPeer* currentPeer : peers) {
		if (isHere(peer, currentPeer)) {
			p.send(currentPeer);
		}
	}
}

void Player::loadClothes(ENetPeer* peer, ENetPeer* currentPeer) {
	PlayerClothing cloth = ((PlayerInfo*)(peer->data))->cloth;

	GamePacket p(0, ((PlayerInfo*)(peer->data))->netID);
	p.extend("OnSetClothing");
	p.extend(cloth.hair, cloth.shirt, cloth.pants);
	p.extend(cloth.feet, cloth.face, cloth.hand);
	p.extend(cloth.back, cloth.mask, cloth.necklace);
	p.extend(((PlayerInfo*)(peer->data))->skinColor);
	p.extend(cloth.ances, 0.0f, 0.0f);
	p.send(currentPeer);
}

void sendInventory(ENetPeer* peer, PlayerInventory inventory) {
	int inventoryLength = inventory.items.size(), packetLength = 66 + (inventoryLength * 4) + 4,
		messageType = 0x4, packetType = 0x9, netID = -1, characterState = 0x8;

	unsigned char* data = new unsigned char[packetLength];

	memset(data, 0, packetLength);
	memcpy(data, &messageType, 4);
	memcpy(data + 4, &packetType, 4);
	memcpy(data + 8, &netID, 4);
	memcpy(data + 16, &characterState, 4);

	int invValue = _byteswap_ulong(inventoryLength);

	memcpy(data + 66 - 4, &invValue, 4);
	invValue = _byteswap_ulong(inventory.inventorySize);
	memcpy(data + 66 - 8, &invValue, 4);

	int val = 0;

	for (int i = 0; i < inventoryLength; i++) {
		val = 0;
		val |= inventory.items[i].itemID;
		val |= inventory.items[i].quantity << 16;
		val &= 0x00FFFFFF;
		val |= 0x00 << 24;

		memcpy(data + (i * 4) + 66, &val, 4);
	}

	ENetPacket* packet = enet_packet_create(data, packetLength, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
	delete data;
}

void Player::sendData(ENetPeer* peer, PlayerMoving* data) {
	for (ENetPeer* currentPeer : peers) {
		if (peer == currentPeer) continue;

		if (isHere(peer, currentPeer)) {
			data->netID = ((PlayerInfo*)(peer->data))->netID;
			sendPacketRaw(currentPeer, 4, packPlayerMoving(data), 56, 0, ENET_PACKET_FLAG_RELIABLE);
		}
	}
}