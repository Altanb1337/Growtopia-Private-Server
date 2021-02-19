#pragma once
#include "stdafx.h"
#include "enet/enet.h"
#include "Server_Func.h"
#include <string>

struct GamePacket {
private:
	int index = 0, len = 0;
	unsigned char* packetData = new unsigned char[61];
public:
	GamePacket(int delay = 0, int netID = -1);
	~GamePacket();

	void extend(std::string);
	void extend(int);
	void extend(unsigned int);
	void extend(float);
	void extend(float, float);
	void extend(float, float, float);

	void send(ENetPeer*);
};