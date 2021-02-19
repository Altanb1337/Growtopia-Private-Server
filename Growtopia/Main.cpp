#pragma warning(disable : 4018)

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>
#include <conio.h>
#include <signal.h>
#include <sstream>

// local files

#include "stdafx.h"
#include "enet/enet.h"
#include "GamePacket.h"
#include "Items_Info.h"
#include "Dialog.h"
#include "Message.h"
#include "Player_Inventory.h"
#include "Player_Info.h"
#include "Time.h"
#include "Global_Variable.h"
#include "Server_Func.h"
#include "World_TileFunc.h"
#include "World_Info.h"

using namespace std;

int main() {
	printf("Growtopia Private Server\n");
	
	printf("Configurating server's requirement\n");

	enet_initialize();
	signal(SIGINT, [](int i) -> void {
		saveAllWorlds;
		exit(0);
	});

	WorldDB::get("START");

	ENetAddress address;
	enet_address_set_host(&address, "0.0.0.0");
	address.port = 17091;	

	ENetHost* server = enet_host_create(&address, 1024, 10, 0, 0);

	if (server == NULL) {
		printf("Unable to create an ENet server\n");
		return;
	}

	server->checksum = enet_crc32;
	enet_host_compress_with_range_coder(server);

	printf("Building items database...\n");
	
	buildItemDefinition();
	
	itemsDatHash = hashItemsDat();

	printf("Items database has been built\n");

	ENetEvent event;

	while (true) {

		while (enet_host_service(server, &event, 1000)) {
			ENetPeer* peer = event.peer;

			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{

				event.peer->data = new PlayerInfo;
				char clientConnection[16];
				enet_address_get_host_ip(&peer->address, clientConnection, 16);
				
				((PlayerInfo*)(peer->data))->IP = clientConnection;

				sendData(peer, 1, 0, 0);

				if (!(find(peers.begin(), peers.end(), peer) != peers.end())) peers.push_back(peer);

				((PlayerInfo*)(peer->data))->netID = peers.size();

			} break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
				if (((PlayerInfo*)(peer->data))->updating) {
					printf("Packet has just dropped\n");
					break;
				}

				int messageType = getMessageType(event.packet);

				WorldInfo* world = getWorld(peer);

				switch (messageType)
				{
				case 2:
				{

					string cch = getTextPointer(event.packet),
						textPacketStr = cch.substr(cch.find("text|") + 5, cch.length() - cch.find("text|") - 1);

					runActionEvent(peer, cch, textPacketStr);

					if (!((PlayerInfo*)(event.peer->data))->isIn) {
						runActionLogginOnEvent(peer, event);
					}

				} break;

				}

			} break;

			}
		}

	}

	return 0;
}