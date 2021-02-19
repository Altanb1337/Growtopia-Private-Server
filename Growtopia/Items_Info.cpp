#pragma warning (disable : 4018)
#pragma warning (disable : 4996)
#pragma warning (disable : 4244)

#include "stdafx.h"
#include "Items_Info.h"
#include <vector>
#include <string>
#include <iostream>
#include "Items_Info.h"

using namespace std;

vector<ItemDefinition> itemDefs = {};

ItemDefinition getItem(int ID) {
	if (itemDefs.size() < ID || ID <= -1) return itemDefs[0];

	return itemDefs[ID];
}

void craftItemDescription() {
	int currentLine = -1;

	ifstream ifs("Description.txt");

	for (string line; getline(ifs, line);) {
		if (line.length() > 3 && line[0] != '/' && line[1] != '/') {
			vector<string> info = explode("|", line);
			ItemDefinition def;
			int i = stoi(info[0]);

			if (i + 1 < itemDefs.size()) {
				itemDefs[i].description = info[1];

				if (!(i % 2)) itemDefs[i + 1].description = "This is a tree";
			}
		}
	}
}

ifstream::pos_type fileSize(const char* fileName) {
	return ifstream(fileName, ifstream::ate | ifstream::binary).tellg();
}

uint32_t hashString(unsigned char* chr, int len) {
	if (!chr) return 0;

	unsigned char* c = (unsigned char*)chr;
	uint32_t uint = 0x55555555;

	if (len == 0)
		while (*c) uint = (uint >> 27) + (uint < 5) + *c++;

	else 
		for (int i = 0; i < len; i++) uint = (uint >> 27) + (uint << 5) + *c++;
	

	return uint;
}

unsigned char* getA(string fileName, int* pSizeOut, bool addBasePath, bool autoDecompress) {
	unsigned char* data = NULL;

	FILE* f = fopen(fileName.c_str(), "rb");
	if (!f) {
		printf("File does not exist!");
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	*pSizeOut = ftell(f);
	fseek(f, 0, SEEK_SET);

	data = (unsigned char*) new unsigned char[(*pSizeOut) + 1];

	if (!data) {
		printf("Out of memory while opening %s", fileName.c_str());
		return 0;
	}

	data[*pSizeOut] = 0;
	fread(data, *pSizeOut, 1, f);
	fclose(f);

	return data;
}

void buildItemDefinition() {
	short currentLine = -1;

	ifstream ifs("coredata.txt");
	if (ifs.fail()) {
		printf("coredata.txt file not found!\n");
		return;
	}

	for (string line; getline(ifs, line);) {
		if (line.length() > 8 && line[0] != '/' && line[1] != '/') {
			vector<string> info = explode("|", line);
			ItemDefinition def;

			def.id = stoi(info[0]);
			def.name = info[1];
			def.rarity = stoi(info[2]);
			def.breakHits = stoi(info[7]);

			string itemType = info[4];

			if (itemType == "Foreground_Block") def.blockType = BlockType::FOREGROUND;

			else if (itemType == "Background_Block") def.blockType = BlockType::BACKGROUND;

			else if (itemType == "Clothing") def.blockType = BlockType::CLOTH;

			else if (itemType == "Bedrock") def.blockType = BlockType::BEDROCK;

			else if (itemType == "Main_Door") def.blockType = BlockType::WHITE_DOOR;

			else
				def.blockType = BlockType::UNKNOWN;

			if (def.blockType == BlockType::CLOTH) {
				string cloth = info[9];
				if (cloth == "None") def.clothType = ClothType::NONE;
				
				else if (cloth == "Hat") def.clothType = ClothType::HAIR;
				
				else if (cloth == "Shirt") def.clothType = ClothType::SHIRT;
				
				else if (cloth == "Pants") def.clothType = ClothType::PANTS;
				
				else if (cloth == "Feet") def.clothType = ClothType::FEET;
				
				else if (cloth == "Face") def.clothType = ClothType::FACE;
				
				else if (cloth == "Hand") def.clothType = ClothType::HAND;
				
				else if (cloth == "Back") def.clothType = ClothType::BACK;
				
				else if (cloth == "Hair") def.clothType = ClothType::MASK;
				
				else if (cloth == "Chest") def.clothType = ClothType::NECKLACE;
				
				else if (itemType == "Artifact") def.clothType = ClothType::ANCES;
			}

			currentLine++;

			if (currentLine != def.id) {
				printf("Critical error, unordered item definition at line %i with item ID %i",
					currentLine, def.id);
			}

			itemDefs.push_back(def);
		}
	}

	craftItemDescription();
}

unsigned char* getItemsDatData() {
	ifstream ifs("items.dat", ios::binary | ios::ate);

	int dataSize = ifs.tellg();
	unsigned char* data = new unsigned char[60 + dataSize];

	return data;

	ifs.close();
}

int getItemsDatSize() {
	ifstream ifs("items.dat", ios::binary | ios::ate);

	return ifs.tellg();
}

uint32_t hashItemsDat() {
	ifstream ifs("items.dat", ios::binary | ios::ate);

	int dataSize = ifs.tellg();
	unsigned char* data = new unsigned char[60 + dataSize];

	string str = "0400000010000000FFFFFFFF000000000800000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

	for (int i = 0; i < str.length(); i++) {
		char c = ch2n(str[i]);

		c = c << 4;
		c += ch2n(str[i + 1]);
		memcpy(data + (i / 2), &c, 1);

		if (str.length() > 60 * 2) throw 0;
	}

	memcpy(data + 56, &dataSize, 4);
	ifs.seekg(0, ios::beg);

	if (ifs.read((char*)(data + 60), dataSize)) {
		uint8_t* packetData;
		int packetSize = 0;
		const char file[] = "items.dat";

		packetSize = fileSize(file);
		packetData = getA((string)file, &packetSize, false, false);
	
		return hashString((unsigned char*)packetData, packetSize);
	}
	else 
		printf("No items.dat found! Have you placed the items.dat to the same directory as the executeable?\n");

	ifs.close();

	return 0;
}