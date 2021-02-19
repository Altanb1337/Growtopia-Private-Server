#pragma warning (disable : 4244)
#pragma warning (disable : 4018)

#include "World_TileFunc.h"
#include "Global_Variable.h"

using namespace std;

void sendTileUpdate(ENetPeer* peer, int x, int y, int tile) {
	PlayerInfo* p = (PlayerInfo*)(peer->data);

	if (tile > itemDefs.size()) return;

	PlayerMoving data;

	data.packetType = 0x3;
	data.characterState = 0x0;
	data.x = x;
	data.y = y;
	data.punchX = x;
	data.punchY = y;
	data.netID = p->netID;
	data.plantingTree = tile;

	WorldInfo* world = getWorld(peer);

	if (world == nullptr) {
		printf("Null world pointer at world %s by %s", p->currentWorld.c_str(), p->tankIDName.c_str());
		return;
	}

	int ii = x + (y * world->width);
	WorldItem worldItem = world->items[ii];
	int block = (worldItem.foreground >= 1) ? worldItem.foreground : worldItem.background;

	sendNothing(peer, x, y);

	// Placing blocks, wrenching blocks, activating blocks codes in here

	if (getItem(tile).blockType == BlockType::CLOTH) return;

	// Breaking blocks
	if (tile == 18) {
		if (worldItem.background == 6864 && worldItem.foreground == 0) return;

		else if (worldItem.background == 0 && worldItem.foreground == 0) return;

		int handTool = p->cloth.hand;
		int breakLevel = (handTool == 98 || handTool == 1439 || handTool == 4956) ? 8 : 6;

		data.packetType = 0x8;
		data.plantingTree = breakLevel;

		if (Time::now() - worldItem.breakTime >= 4) {
			world->items[ii].breakTime = Time::now();
			world->items[ii].breakLevel = 0;
		}

		if (y < world->height) {
			world->items[ii].breakTime = Time::now();
			world->items[ii].breakLevel += breakLevel;
		}

		if (y < world->height && worldItem.breakLevel >= getItem(block).breakHits * 6) {
			data.packetType = 0x3;
			data.plantingTree = 18;
			world->items[ii].breakLevel = 0;

			if (worldItem.foreground != 0) world->items[ii].foreground = 0;

			else world->items[ii].background = 6864; // Can't be changed to 0
		}
	}
	else {
		// Placing blocks

		// Please use a function that erases the player items in another header file instead
		for (int i = 0; i < p->inventory.items.size(); i++) {
			if (p->inventory.items[i].itemID == tile) {

				if (p->inventory.items[i].quantity > 1) p->inventory.items[i].quantity--;

				else p->inventory.items.erase(p->inventory.items.begin() + i);
			}
		}

		if (getItem(tile).blockType == BlockType::BACKGROUND) world->items[ii].background = tile;

		else {
			if (worldItem.foreground != 0) return;

			world->items[ii].foreground = tile;
		}

		world->items[ii].breakLevel = 0;
	}

	for (ENetPeer* currentPeer : peers) {
		if (isHere(peer, currentPeer)) 
			sendPacketRaw(currentPeer, 4, packPlayerMoving(&data), 56, 0, ENET_PACKET_FLAG_RELIABLE);
	}
}