#pragma warning (disable : 4018)
#pragma warning (disable : 4101)
#pragma warning (disable : 4267)

#include "stdafx.h"
#include <string>
#include "World_Info.h"
#include "Server_Func.h"
#include "json.hpp"
#include <vector>

using namespace std;
using json = nlohmann::json;

vector<WorldInfo> WorldDB::worlds = {};

WorldInfo generateWorld(string name, int width, int height) {
	WorldInfo world;
	world.name = name;
	world.height = height;
	world.width = width;
	world.items = new WorldItem[world.width * world.height];
	int whiteDoorPos = rand() % 100;

	for (int i = 0; i < world.width * world.height; i++) {
		
		if (i >= 3800 && i < 5400 && !(rand() % 50)) world.items[i].foreground = 10;

		else if (i >= 3700 && i < 5400) {
			if (i > 5400) {
				if (i % 7 == 0) world.items[i].foreground = 4;

				else world.items[i].foreground = 2;
			}
			else world.items[i].foreground = 2;
		}
		else if (i >= 5400) world.items[i].foreground = 8;

		if (i >= 3700) world.items[i].background = 14;

		if (i == (3600 + whiteDoorPos)) world.items[i].foreground = 6;

		else if (i >= 3600 && i < 3700) world.items[i].foreground = 0;

		if (i == (3700 + whiteDoorPos)) world.items[i].foreground = 8;
	}

	return world;
}

AWorld WorldDB::get2(string worldName) {
	if (worlds.size() >= 200) saveRedundant();

	AWorld ret;
	string name = upperStr(worldName);

	if (name.length() < 1) throw 1;

	for (char c : name) 
		if ((c < 'A' || c > 'Z') && (c < '0' || c > '9')) throw 2; 

	if (name == "EXIT") throw 3;

	vector<string> blacklistedWorld;

	for (string info : blacklistedName) blacklistedWorld.push_back(upperStr(info));

	if (find(blacklistedWorld.begin(), blacklistedWorld.end(), name) != blacklistedWorld.end()) throw 3;

	for (int i = 0; i < worlds.size(); i++) {
		if (worlds[i].name == name) {
			ret.id = 1;
			ret.info = worlds[i];
			ret.ptr = &worlds[i];

			return ret;
		}
	}

	ifstream ifs("worlds/" + name + ".json");
	if (ifs.is_open()) {
		json j;
		ifs >> j;

		WorldInfo info;
		info.name = j["name"].get<string>();
		info.width = j["width"];
		info.height = j["height"];
		info.owner = j["owner"].get<string>();
		info.items = new WorldItem[info.width * info.height];

		for (int i = 0; i < info.width * info.height; i++) {
			info.items[i].foreground = j["tiles"][i]["foreground"];
			info.items[i].background = j["tiles"][i]["background"];
		}

		worlds.push_back(info);
		ret.id = worlds.size() - 1;
		ret.info = info;
		ret.ptr = &worlds[worlds.size() - 1];

		return ret;
	}
	else {
		WorldInfo info = generateWorld(name, 100, 60);

		worlds.push_back(info);
		ret.id = worlds.size() - 1;
		ret.info = info;
		ret.ptr = &worlds[worlds.size() - 1];

		return ret;
	}
}

WorldInfo WorldDB::get(string name) { return get2(name).info; }

void WorldDB::flush(WorldInfo info) {
	ofstream ofs("worlds/" + info.name + ".json");

	if (ofs.fail()) return;

	json j;
	j["name"] = info.name;
	j["width"] = info.width;
	j["height"] = info.height;
	j["owner"] = info.owner;

	json js = json::array();

	for (int i = 0; i < info.width * info.height; i++) {
		json x;
		
		x["background"] = info.items[i].background;
		x["foreground"] = info.items[i].foreground;

		js.push_back(js);
	}

	j["tiles"] = js;

	ofs << j;
}

void WorldDB::flush2(AWorld info) { flush(info.info); }

void WorldDB::save(AWorld info) {
	flush2(info);
	delete info.info.items;

	worlds.erase(worlds.begin() + info.id);
}

void WorldDB::saveAll() {
	for (int i = 0; i < worlds.size(); i++) {
		flush(worlds[i]);
		delete worlds[i].items;
	}

	worlds.clear();
}

vector<WorldInfo> WorldDB::getRandomWorlds() {
	vector<WorldInfo> ret;

	for (int i = 0; i < ((worlds.size() < 10) ? worlds.size() : 10); i++) ret.push_back(worlds[i]);

	if (worlds.size() >= 5) {
		for (int i = 0; i < 6; i++) {
			bool possible = true;

			WorldInfo world = worlds[rand() % worlds.size() - 4];

			for (int j = 0; j < ret.size(); j++) {
				if (world.name == ret[i].name || world.name == "EXIT") possible = false;
			}

			if (possible) ret.push_back(world);
		}
	}

	return ret;
}

void WorldDB::saveRedundant() {
	for (int i = 4; i < worlds.size(); i++) {
		bool canFree = true;

		for (ENetPeer* currentPeer : peers) {
			if (((PlayerInfo*)(currentPeer->data))->currentWorld == worlds[i].name) canFree = false;
		}

		if (canFree) {
			flush(worlds[i]);
			delete worlds[i].items;

			worlds.erase(worlds.begin() + i);;
			i--;
		}
	}
}

void saveAllWorlds() {
	printf("Saving all worlds...\n");

	WorldDB::saveAll();

	printf("All worlds have been saved!\n");
}

WorldInfo* getWorld(ENetPeer* peer) {
	try { return WorldDB::get2(((PlayerInfo*)(peer->data))->currentWorld).ptr; }

	catch (int e) { return nullptr; }
}

WorldInfo* getWorld(string world) {
	try { return WorldDB::get2(world).ptr; }

	catch (int e) { return nullptr; }
}

void sendWorld(ENetPeer* peer, WorldInfo* world) {
	((PlayerInfo*)(peer->data))->clothesUpdated = false;

	string worldName = world->name;
	int worldWidth = world->width, worldHeight = world->height;
	int area = worldWidth * worldHeight;
	unsigned short nameLength = worldName.length();

	int allocator = (8 * area);
	int total = 78 + nameLength + area + 24 + allocator;

	unsigned char* data = new unsigned char[total];
	int s1 = 4, s3 = 8;

	memset(data, 0, total);
	memcpy(data, &s1, 1);
	memcpy(data + 4, &s1, 1);
	memcpy(data + 16, &s3, 1);
	memcpy(data + 66, &nameLength, 1);
	memcpy(data + 68, worldName.c_str(), nameLength);
	memcpy(data + 68 + nameLength, &worldWidth, 1);
	memcpy(data + 72 + nameLength, &worldHeight, 1);
	memcpy(data + 76 + nameLength, &area, 2);

	ENetPacket* packet = enet_packet_create(data, total, ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(peer, 0, packet);

	for (int i = 0; i < area; i++) {
		PlayerMoving data;

		data.packetType = 0x3;
		data.characterState = 0x0;
		data.x = i % world->width;
		data.y = i / world->height;
		data.punchX = i % world->width;
		data.punchY = i / world->width;
		data.XSpeed = 0;
		data.YSpeed = 0;
		data.netID = -1;
		data.plantingTree = world->items[i].foreground;
		sendPacketRaw(peer, 4, packPlayerMoving(&data), 56, 0, ENET_PACKET_FLAG_RELIABLE);

		if (getItem(world->items[i].foreground).blockType != BlockType::WHITE_DOOR) {
			data.plantingTree = world->items[i].background;
			sendPacketRaw(peer, 4, packPlayerMoving(&data), 56, 0, ENET_PACKET_FLAG_RELIABLE);
		}
	}
	((PlayerInfo*)(peer->data))->currentWorld = world->name;

	delete[] data;
}

int getPlayerInWorld(string world) {
	int i = 0;

	for (ENetPeer* currentPeer : peers) {
		if (((PlayerInfo*)(currentPeer->data))->currentWorld == world) i++;
	}

	return i;
}

int getPlayerInWorld(ENetPeer* peer) {
	int i = 0;
	for (ENetPeer* currentPeer : peers) {
		if (isHere(peer, currentPeer)) i++;
	}

	return i;
}

void sendWorldOffer(ENetPeer* peer) {
	if (!((PlayerInfo*)(peer->data))->isIn) return;
	
	WorldDB worldDB;

	vector<WorldInfo> worlds = WorldDB::getRandomWorlds();
	
	string str = "default|";

	if (worlds.size() > 0) str += worlds[0].name;

	str += "\nadd_button|Showing: Random Worlds|_worldCategory_|0.6|3529161471|\n";

	for (int i = 0; i < worlds.size(); i++)
		str += "add_floater|" + worlds[i].name + "|" + to_string(getPlayerInWorld(worlds[i].name)) + "|0.55|3529161471|\n";

	GamePacket p;
	p.extend("OnRequestWorldSelectMenu");
	p.extend(str);

	p.send(peer);
}