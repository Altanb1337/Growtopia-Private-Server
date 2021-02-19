#pragma once
#include <string>
#include "stdafx.h"
#include "enet/enet.h"
#include <vector>
#include "Server_Func.h"
#include "Player_Info.h"

struct InventoryItem {
	unsigned short itemID = 0;
	unsigned char quantity = 0;
};

struct PlayerInventory {
	std::vector<InventoryItem> items;
	short inventorySize = 100;
};

void sendInventory(ENetPeer*);