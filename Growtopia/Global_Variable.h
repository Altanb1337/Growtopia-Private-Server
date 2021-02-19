#pragma once
#include "stdafx.h"
#include "enet/enet.h"
#include <vector>
#include "Items_Info.h"
#include "World_Info.h"

extern std::vector<ENetPeer*> peers;

extern std::vector<std::string> blacklistedName;

extern uint32_t itemsDatHash;