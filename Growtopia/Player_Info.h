#pragma once
#include "stdafx.h"
#include "Server_Func.h"
#include "GamePacket.h"
#include "Global_Variable.h"
#include "Message.h"
#include <string>
#include <vector>
#include <iostream>

struct InventoryItem {
	unsigned short itemID = 0;
	unsigned char quantity = 0;
};

struct PlayerInventory {
	std::vector<InventoryItem> items;
	short inventorySize = 100;
};

void sendInventory(ENetPeer*, PlayerInventory);

enum AdminLevel {
	PLAYER, MODERATOR, OWNER
};

struct PlayerClothing {
	short hair = 0,
		shirt = 0,
		pants = 0,
		feet = 0,
		face = 0,
		hand = 0,
		back = 0,
		mask = 0,
		necklace = 0,
		ances = 0;
};

struct PlayerInfo {
	std::string currentWorld = "EXIT",
		tankIDName = "",
		rawName = "",
		displayName = "",
		tankIDPass = "",
		country = "us",
		IP = "",
		guestName = "";

	int x = 0, y = 0, skinColor = 0x8295C3FF, netID = 0;

	bool facingLeft = false, updating = false, clothesUpdated = false, radio = false,
		loggedOn = false, isIn = false, registered = false;

	std::vector<AdminLevel> adminLevel = { PLAYER };

	PlayerClothing cloth;
	PlayerInventory inventory;

	// state
	bool noclip = false, doubleJump = false;
};

struct PlayerMoving {
	int packetType;
	int netID;
	int x;
	int y;
	int characterState;
	int plantingTree;
	float XSpeed;
	float YSpeed;
	int punchX;
	int punchY;
};

int getState(PlayerInfo*);

class Player {
private:
	
public:
	static std::string filterName(std::string);
	static std::string fixColor(std::string);
	static void login(ENetPeer*, std::string, std::string);
	static void createAccount(ENetPeer*, std::string, std::string, std::string, std::string);
	static void setGrowID(ENetPeer*, std::string, std::string);
	static void joinWorld(ENetPeer*, std::string, int, int);
	static void leaveWorld(ENetPeer*);
	static void sendData(ENetPeer*, PlayerMoving*);
	static void loadClothes(ENetPeer*, ENetPeer*);

	/*
	- spawn(ENetPeer* peer, ENetPeer* otherPeer)
	- The onSpawn packet will be sent to ENetPeer* peer
	- onSpawn's str will contain ENetPeer* otherPeer's information
	*/
	static void spawn(ENetPeer*, ENetPeer*);

	static void sendAction(ENetPeer*, std::string);
};

unsigned char* packPlayerMoving(PlayerMoving*);

PlayerMoving* unpackPlayerMoving(unsigned char*);

void sendPacketRaw(ENetPeer*, int, void*, size_t, void*, int);

void sendState(ENetPeer*);

std::string packPlayerMoving2(PlayerMoving*);

void sendNothing(ENetPeer*, int, int);

void sendPacketRaw3(ENetPeer*, int, void*, size_t, void*, int);