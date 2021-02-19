#pragma once
#include "stdafx.h"
#include "enet/enet.h"
#include "Player_Info.h"
#include "Server_Func.h"
#include "Global_Variable.h"
#include <string>
#include <vector>

struct WorldItem {
	unsigned short foreground = 0, background = 0;
	short breakLevel = 0;
	unsigned long breakTime = 0;
};

struct WorldInfo {
	int width = 100, height = 60;
	int weather = 0;

	std::string name = "NULL",
		owner = "";

	WorldItem* items;
};

struct AWorld {
	WorldInfo* ptr;
	WorldInfo info;
	int id;
};

class WorldDB {
	static std::vector<WorldInfo> worlds;
public:
	static WorldInfo get(std::string);
	static AWorld get2(std::string);
	static void flush(WorldInfo);
	static void flush2(AWorld);
	static void save(AWorld);
	static void saveAll();
	static void saveRedundant();
	static std::vector<WorldInfo> getRandomWorlds();
};

void saveAllWorlds();

WorldInfo generateWorld(std::string, int, int);

WorldInfo* getWorld(std::string);
WorldInfo* getWorld(ENetPeer*);

void sendWorld(ENetPeer*, WorldInfo*);

void sendWorldOffer(ENetPeer*);

int getPlayerInWorld(ENetPeer*);
int getPlayerInWorld(std::string);