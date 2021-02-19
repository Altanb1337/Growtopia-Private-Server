#include "stdafx.h"
#include "enet/enet.h"
#include "Player_Inventory.h"
#include <vector>

using namespace std;

void sendInventory(ENetPeer* peer) {
	PlayerInfo* p = (PlayerInfo*)(peer->data);

	int inventoryLength = p->inventoryItem.size(), packetLength = 66 + (inventoryLength * 4) + 4,
		messageType = 0x4, packetType = 0x9, netID = -1, characterState = 0x8;

	unsigned char* data = new unsigned char[packetLength];
	
	memset(data, 0, packetLength);
	memcpy(data, &messageType, 4);
	memcpy(data + 4, &packetType, 4);
	memcpy(data + 8, &netID, 4);
	memcpy(data + 16, &characterState, 4);

	int invValue = _byteswap_ulong(inventoryLength);

	memcpy(data + 66 - 4, &invValue, 4);
	invValue = _byteswap_ulong(100);
	memcpy(data + 66 - 8, &invValue, 4);

	int val = 0;

	for (int i = 0; i < inventoryLength; i++) {
		val = 0;
		val |= p->inventoryItem[i].itemID;
		val |= p->inventoryItem[i].quantity << 16;
		val &= 0x00FFFFFF;
		val |= 0x00 << 24;

		memcpy(data + (i * 4) + 66, &val, 4);
	}

	ENetPacket* packet = createPacket(data, packetLength, ENET_PACKET_FLAG_RELIABLE);
	peerSend(peer, packet);
	delete data;
}