#pragma once
#include <string>
#include "stdafx.h"
#include "enet/enet.h"
#include "Server_Func.h"

enum BroadcastType {
	WORLD, SERVER
};

class Message {
public:
	static void sendConsole(ENetPeer*, std::string);
	static void sendConsole(ENetPeer*, BroadcastType, std::string);
	static void sendBuble(ENetPeer*, ENetPeer*, std::string);
	static void sendChat(ENetPeer*, std::string);
};