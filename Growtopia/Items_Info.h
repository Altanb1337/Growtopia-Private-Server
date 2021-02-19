#pragma once
#include "stdafx.h"
#include "Player_Info.h"
#include "Global_Variable.h"
#include "Server_Func.h"
#include <string>
#include <fstream>

enum ClothType {
	HAIR,
	SHIRT,
	PANTS,
	FEET,
	FACE,
	HAND,
	BACK,
	MASK,
	NECKLACE,
	ANCES,
	NONE
};

enum BlockType {
	FOREGROUND,
	BACKGROUND,
	BEDROCK,
	WHITE_DOOR,
	CLOTH,
	UNKNOWN
};

struct ItemDefinition {
	short id;
	std::string name, description = "This item has no descriptions.";

	int breakHits = 0, dropChance = 0, rarity = 0;
	ClothType clothType;
	BlockType blockType;
};

ItemDefinition getItem(int);

void craftItemDescription();

void buildItemDefinition();

std::ifstream::pos_type fileSize(const char*);

uint32_t hashString(unsigned char*, int);

unsigned char* getA(std::string, int*, bool, bool);

uint32_t hashItemsDat();

unsigned char* getItemsDatData();

int getItemsDatSize();

extern std::vector<ItemDefinition> itemDefs;