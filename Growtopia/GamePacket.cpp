#include "stdafx.h"
#include "enet/enet.h"
#include "GamePacket.h"
#include <string>

using namespace std;

GamePacket::GamePacket(int delay, int netID) {
	len = 61;

	int messageType = 0x4, packetType = 0x1, charState = 0x8;

	memset(packetData, 0, 61);
	memcpy(packetData, &messageType, 4);
	memcpy(packetData + 4, &packetType, 4);
	memcpy(packetData + 8, &netID, 4);
	memcpy(packetData + 16, &charState, 4);
	memcpy(packetData + 24, &delay, 4);
}

GamePacket::~GamePacket() {
	delete[] packetData;
}

void GamePacket::extend(std::string str) {
	unsigned char* data = new unsigned char[len + 2 + str.length() + 4];

	memcpy(data, packetData, len);
	delete[] packetData;

	packetData = data;
	data[len] = index;
	data[len + 1] = 0x2;

	int strLength = str.length();

	memcpy(data + len + 2, &strLength, 4);
	memcpy(data + len + 6, str.data(), strLength);

	len += 2 + str.length() + 4;
	index++;
	packetData[60] = (unsigned char)index;
}

void GamePacket::extend(int i) {
	unsigned char* data = new unsigned char[len + 2 + 4];

	memcpy(data, packetData, len);
	delete[] packetData;

	packetData = data;
	data[len] = index;
	data[len + 1] = 0x9;

	memcpy(data + len + 2, &i, 4);
	len += 2 + 4;
	index++;
	packetData[60] = (unsigned char)index;
}

void GamePacket::extend(unsigned int i) {
	unsigned char* data = new unsigned char[len + 2 + 4];

	memcpy(data, packetData, len);
	delete[] packetData;

	packetData = data;
	data[len] = index;
	data[len + 1] = 0x5;

	memcpy(data + len + 2, &i, 4);
	len += 2 + 4;
	index++;
	packetData[60] = (unsigned char)index;
}

void GamePacket::extend(float f) {
	unsigned char* data = new unsigned char[len + 2 + 4];

	memcpy(data, packetData, len);
	delete[] packetData;

	packetData = data;
	data[len] = index;
	data[len + 1] = 0x1;
	memcpy(data + len + 2, &f, 4);
	len += + 2 + 4;
	index++;
	packetData[60] = (unsigned char)index;
}

void GamePacket::extend(float f, float f2) {
	unsigned char* data = new unsigned char[len + 2 + 8];

	memcpy(data, packetData, len);
	delete[] packetData;

	packetData = data;
	data[len] = index;
	data[len + 1] = 0x3;
	memcpy(data + len + 2, &f, 4);
	memcpy(data + len + 6, &f2, 4);
	len += + 2 + 8;
	index++;
	packetData[60] = (byte)index;
}

void GamePacket::extend(float f, float f2, float f3) {
	unsigned char* data = new unsigned char[len + 2 + 12];

	memcpy(data, packetData, len);
	delete[] packetData;

	packetData = data;
	data[len] = index;
	data[len + 1] = 0x4;
	memcpy(data + len + 2, &f, 4);
	memcpy(data + len + 6, &f2, 4);
	memcpy(data + len + 10, &f3, 4);
	len = len + 2 + 12;
	index++;
	packetData[60] = (byte)index;
}

void GamePacket::send(ENetPeer* peer) {
	ENetPacket* p = enet_packet_create(packetData, len, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, p);
}