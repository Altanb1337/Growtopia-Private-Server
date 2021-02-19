#pragma once
// recommended not to include this header in any other files except in source file

#include "stdafx.h"
#include <string>
#include <vector>
#include <signal.h>
#include <fstream>
#include <sstream>


// local
#include "stdafx.h"
#include "GamePacket.h"
#include "Items_Info.h"
#include "Dialog.h"
#include "Message.h"
#include "Player_Info.h"
#include "Time.h"
#include "Global_Variable.h"
#include "Server_Func.h"
#include "World_TileFunc.h"
#include "World_Info.h"

class Event {
public:
	static void actionEvent(ENetPeer*, std::string, std::string);

	static void playerLoginEvent(ENetPeer*, ENetEvent);

	static void enterGame(ENetPeer*);
};

void runEvent();