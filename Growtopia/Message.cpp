#include "stdafx.h"
#include "GamePacket.h"
#include "enet/enet.h"
#include <string>
#include "Message.h"
#include "Global_Variable.h"

using namespace std;

void Message::sendConsole(ENetPeer* peer, string message) {
	GamePacket packet;
	packet.extend("OnConsoleMessage");
	packet.extend(message);

	packet.send(peer);
}

void Message::sendConsole(ENetPeer* peer, BroadcastType e, string message) {
	switch (e)
	{
	case WORLD:
	{
		for (ENetPeer* currentPeer : peers) {
			if (isHere(peer, currentPeer)) Message::sendConsole(currentPeer, message);
		}
	} break;
	case SERVER:
	{
		for (ENetPeer* currentPeer : peers) Message::sendConsole(currentPeer, message);
	} break;

	default:
		break;
	}
}

void Message::sendBuble(ENetPeer* peer, ENetPeer* currentPeer, string message) {
	GamePacket p;
	p.extend("OnTalkBubble");
	p.extend(((PlayerInfo*)(peer->data))->netID);
	p.extend(message);
	p.extend(0);

	p.send(currentPeer);
}

void Message::sendChat(ENetPeer* peer, string message) {
	PlayerInfo* p = (PlayerInfo*)(peer->data);
	if (!message.length()) return;

	for (ENetPeer* currentPeer : peers) {
		if (isHere(peer, currentPeer)) {
			sendBuble(peer, currentPeer, message);
			sendConsole(currentPeer, "CP:0_PL:4_OID:_CT:[W]_ `6<`0" + p->displayName + "`6> `$" + message);
		}
	}
}