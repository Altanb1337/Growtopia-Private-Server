#include "Event_Handler.h"
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// Placing actions in header file is recommended

void Event::actionEvent(ENetPeer* peer, string cch, string str) {
	if (cch.find("action|wrench") == 0) {
		// do something
	}
	else if (cch.find("action|dialog_return") == 0) {
		stringstream ss(cch);
		string to, btn;

		string username, password, passwordVerify, email;
		bool isRegisterDialog = false;

		while (getline(ss, to, '\n')) {
			vector<string> info = explode("|", to);

			if (info.size() >= 2) {
				if (info[0] == "buttonClicked") btn = info[1];
			}
		}

		if (btn != "") {
			// do something
		}
	}
	else if (cch.find("action|drop\nitemID|") == 0) {
		string dropStr = "action|drop\nitemID|";
		int itemID = stoi(cch.substr(dropStr.length(), cch.length() - dropStr.length() - 1));

		// do something
	}
	else if (cch.find("text|") != string::npos) {
		PlayerInfo* p = (PlayerInfo*)(peer->data);

		if (!p->registered) {
			Message::sendBuble(peer, peer, "Register account before talking!");
			return;
		}
		else if (p->currentWorld == "EXIT") {
			enet_peer_disconnect_later(peer, 0);
			return;
		}
		
		if (str.length()) {
			if (str[0] == '/') {
				Player::sendAction(peer, str);
				Message::sendConsole(peer, "`6" + str);
			}
			else Message::sendChat(peer, str);
		}

		// commands
		if (str[0] == '/') {
			// use else if() instead of if() repeatedly

			if (str.substr(0, 4) == "/sb ") {
				string message = str.substr(4).c_str();
				
				// do something
			}
			else if (str == "/radio") {
				// do something
			}
		}
	}
}

void Event::playerLoginEvent(ENetPeer* peer, ENetEvent event) {
	PlayerInfo* p = (PlayerInfo*)(peer->data);
	GamePacket packet;
	packet.extend("OnSuperMainStartAcceptLogonHrdxs47254722215a");
	packet.extend(hashItemsDat());
	packet.extend("ubistatic-a.akamaihd.net");
	packet.extend(17091);
	packet.extend("cc.cz.madkite.freedom org.aqua.gg idv.aqua.bulldog com.cih.gamecih2 com.cih.gamecih com.cih.game_cih cn.maocai.gamekiller com.gmd.speedtime org.dax.attack com.x0.strai.frep com.x0.strai.free org.cheatengine.cegui org.sbtools.gamehack com.skgames.traffikrider org.sbtoods.gamehaca com.skype.ralder org.cheatengine.cegui.xx.multi1458919170111 com.prohiro.macro me.autotouch.autotouch com.cygery.repetitouch.free com.cygery.repetitouch.pro com.proziro.zacro com.slash.gamebuster");
	packet.extend("proto=84|choosemusic=audio/mp3/about_theme.mp3|active_holiday=0|server_tick=226933875|clash_active=0|drop_lavacheck_faster=1|isPayingUser=0|");

	packet.send(peer);

	stringstream ss(getTextPointer(event.packet));
	string to;

	while (getline(ss, to, '\n')) {
		string ID = to.substr(0, to.find('|'));
		string act = to.substr(to.find('|') + 1, to.length() - to.find('|') - 1);

		if (ID == "tankIDName") {
			p->tankIDName = act;
			p->registered = true;
		}
		else if (ID == "tankIDPass") p->tankIDPass = act;

		else if (ID == "requestedName") p->guestName = act;

		else if (ID == "country") p->country = act;
	}

	if (!p->registered) {
		p->loggedOn = true;
		p->displayName = "Fake " + Player::fixColor(p->guestName
			.substr(0, p->guestName.length() >= 15 ? p->guestName.length() : 15));
	}
	else {
		p->rawName = Player::filterName(p->tankIDName);

		try { Player::login(peer, p->rawName, p->tankIDPass); }

		catch (int e) {
			if (e == -1) {
				Message::sendConsole(peer, "Wrong username or password");
				enet_peer_disconnect_later(peer, 0);
			}
		}
		
		p->displayName = Player::fixColor(p->tankIDName
			.substr(p->tankIDName.length() >= 18 ? 18 : p->tankIDName.length()));
	}

	for (char c : p->displayName) {
		if (c < 0x20 || c > 0x7A) {
			enet_peer_disconnect_later(peer, 0);
		}
	}

	Player::setGrowID(peer, p->tankIDName, p->tankIDPass);
}

void Event::enterGame(ENetPeer* peer) {
	PlayerInfo* p = (PlayerInfo*)(peer->data);

	if (p->registered) {
		Message::sendConsole(peer, "`oSuccessfully logged on!");
	}
	else {
		Message::sendConsole(peer, "Logged on with a guest account");
	}

	if (p->inventory.items.empty()) {
		p->inventory.items.push_back({ 18, 1 });
		p->inventory.items.push_back({ 32, 1 });
	}
}

void runEvent() {
	ENetHost* server;

	// loading
	printf("Preparing server's tribute, running functions loader...\n");

	{
		int enetStatus = enet_initialize();

		if (enetStatus < 0) printf("Failed to initialize ENet!\n");
	}

	signal(SIGINT, [](int s) -> void {
		saveAllWorlds();
		exit(0);
	});

	WorldDB::get("START");

	ENetAddress address;

	enet_address_set_host(&address, "0.0.0.0");
	address.port = 17091;

	server = enet_host_create(&address, 1024, 10, 0, 0);

	if (server == NULL) {
		printf("An error has just occured while trying to create an ENet server\n");
		exit(EXIT_FAILURE);
	}

	server->checksum = enet_crc32;
	enet_host_compress_with_range_coder(server);

	if (ifstream("items.dat").fail()) 
		printf("items.dat not found! Have you put everything in the same directory?\n");
	
	printf("Loading items' definition for the server...\n");
	buildItemDefinition();
	printf("Items' definition loaded to the server!\n");

	// successfully loaded
	printf("All functions have been executed and succeeded!\n\n");

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
						printf("Packet dropped\n");
						break;
					}

					int messageType = getMessageType(event.packet);

					switch (messageType)
					{
						case 2:
						{
							string cch = getTextPointer(event.packet),
								str = cch.substr(cch.find("text|") + 5, cch.length() - cch.find("text|") - 1);

							Event::actionEvent(peer, cch, str);

							if (!((PlayerInfo*)(peer->data))->isIn) Event::playerLoginEvent(peer, event);

							if (cch.substr(0, 17) == "action|enter_game" && !((PlayerInfo*)(peer->data))->isIn) {
								Event::enterGame(peer);

								((PlayerInfo*)(peer->data))->isIn = true;
								((PlayerInfo*)(peer->data))->loggedOn = true;

								sendWorldOffer(peer);
							}
							if (strcmp(getTextPointer(event.packet), "action|refresh_item_data\n") == 0) {
								ENetPacket* packet = enet_packet_create(
									getItemsDatData(), getItemsDatSize() + 60,
									ENET_PACKET_FLAG_RELIABLE);
								enet_peer_send(peer, 0, packet);

								((PlayerInfo*)(peer->data))->updating = true;
							}
							
						} break;

						case 3:
						{
							stringstream ss(getTextPointer(event.packet));
							string to;
							bool joinRequest = false;

							while (getline(ss, to, '\n')) {
								string id = to.substr(0, to.find("|"));
								string act = to.substr(to.find("|") + 1, to.length() - to.find("|") - 1);

								if (id == "name" && joinRequest) {
									if (!((PlayerInfo*)(peer->data))->loggedOn) break;

									try { Player::joinWorld(peer, act, 0, 0); }

									catch (int err) {
										string errorMessage;

										switch (err)
										{
											case 1:
											{
												errorMessage = "You've exited the world!";
											} break;
											case 2:
											{
												errorMessage = "You have entered bad characters in the world name!";
											} break;
											case 3:
											{
												errorMessage = "Exit from what? Click back if you're done playing.";
											} break;
											default:
											{
												errorMessage = "I know this menu is magical and all, but it has its limitations! You can't visit this world!";
											} break;
										}

										Message::sendConsole(peer, errorMessage);
										GamePacket p;
										p.extend("OnFailedToEnterWorld");
										p.extend(1);

										p.send(peer);
									}
								}
								else if (id == "action") {
									if (act == "join_request") joinRequest = true;

									else if (act == "quit_to_exit") {
										Player::leaveWorld(peer);
										((PlayerInfo*)(peer->data))->currentWorld = "EXIT";
										sendWorldOffer(peer);
									}
									else if (act == "quit") enet_peer_disconnect_later(peer, 0);
								}
							}

						} break;

						case 4:
						{
							unsigned char* packetTank = getStructPointer(event.packet);

							if (packetTank) {
								PlayerMoving* playerMove = unpackPlayerMoving(packetTank);

								switch (playerMove->packetType)
								{
									case 0:
									{
										((PlayerInfo*)(peer->data))->x = playerMove->x;
										((PlayerInfo*)(peer->data))->y = playerMove->y;
										((PlayerInfo*)(peer->data))->facingLeft = playerMove->characterState & 0x10;
										Player::sendData(peer, playerMove);

										if (!((PlayerInfo*)(peer->data))->clothesUpdated) {
											for (ENetPeer* currentPeer : peers) {
												if (isHere(peer, currentPeer)) {
													Player::loadClothes(peer, currentPeer);
													Player::loadClothes(currentPeer, peer);
												}
											}

											((PlayerInfo*)(peer->data))->clothesUpdated = false;
										}

									} break;
									case 3:
									{
										sendTileUpdate(peer, playerMove->punchX, playerMove->punchY, playerMove->plantingTree);
									} break;
									case 7:
									{
										WorldInfo* world = getWorld(peer);
										int x = playerMove->punchX, y = playerMove->punchY;

										if (getItem(world->items[x + (y * world->width)].foreground).blockType == BlockType::WHITE_DOOR) {
											Player::leaveWorld(peer);
											((PlayerInfo*)(peer->data))->currentWorld = "EXIT";
											sendWorldOffer(peer);
										}

									} break;
									case 10:
									{
										/* 
											- packetType 10 is the selected item
											- plantingTree is the item ID
										*/
									} break;
									case 18:
									{
										Player::sendData(peer, playerMove);
									} break;
									default: break;
								}

								delete playerMove;
							}
							else printf("Bad tank packet!\n");
						} break;

						default:
						{
							printf("Unknown packet type!\n");
						} break;

						enet_packet_destroy(event.packet);
					}
				} break;
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					if (((PlayerInfo*)(peer->data))->currentWorld != "EXIT") Player::leaveWorld(peer);

					((PlayerInfo*)(peer->data))->inventory.items.clear();

					delete (PlayerInfo*)(peer->data);
					peer->data = NULL;

					peers.erase(remove(peers.begin(), peers.end(), peer), peers.end());
				} break;
			}

		}

	}
}