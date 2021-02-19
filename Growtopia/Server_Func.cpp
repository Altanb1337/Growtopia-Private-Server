#pragma warning (disable : 4018)
#pragma warning (disable : 4715)

#include "stdafx.h"
#include "Server_Func.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

unsigned long byteSwap(unsigned long x) {
	x = ((x & 0xFFFF0000FFFF0000) >> 16) | ((x & 0x0000FFFF0000FFFF) << 16);

	return ((x & 0xFF00FF00FF00FF00) >> 8) | ((x & 0x00FF00FF00FF00FF) << 8);
}

void sendData(ENetPeer* peer, int num, char* data, int len) {
	ENetPacket* p = enet_packet_create(0, len + 5, ENET_PACKET_FLAG_RELIABLE);

	memcpy(p->data, &num, 4);

	if (data != NULL) memcpy(p->data + 4, data, len);

	char c = 0;
	memcpy(p->data + 4 + len, &c, 1);

	enet_peer_send(peer, 0, p);
}

int getPacketID(char* data) { return *data; }

char* getPacketData(char* data) { return data + 4; }

string encodeText(char* text) {
	string ret;

	while (text[0] != 0) {
		switch (text[0])
		{
		case '\n': ret += "\\n";
			break;
		case '\t': ret += "\\t";
			break;
		case '\b': ret += "\\b";
			break;
		case '\\': ret += "\\\\";
			break;
		case '\r': ret += "\\r";
			break;
		default:
			ret += text[0];
			break;
		}

		text++;
	}

	return ret;
}

inline bool isNumber(string str) { return str.find_first_not_of("0123456789") == std::string::npos; }

int ch2n(char text) {
	int i = -1;

	if (isNumber(to_string(text))) i = stoi(to_string(text));

	if (i >= 0 && i <= 9) return i;
	
	switch (text)
	{
	case 'A': return 10;

	case 'B': return 11;

	case 'C': return 12;

	case 'D': return 13;

	case 'E': return 14;

	case 'F': return 15;

	default:
		break;
	}
}

char* getTextPointer(ENetPacket* p) {
	char c = 0;
	memcpy(p->data + p->dataLength - 1, &c, 1);

	return (char*)(p->data + 4);
}

unsigned char* getStructPointer(ENetPacket* p) {
	unsigned int packetLength = p->dataLength;
	unsigned char* result = NULL;

	if (packetLength >= 0x3C) {
		unsigned char* packetData = p->data;
		result = packetData + 4;

		if (*(unsigned char*)(packetData + 16) & 8) {
			if (packetLength < *(int*)(packetData + 56) + 60) {
				printf("Packet is too small for the extended packet to be valid\n");
				printf("Size of float is 4. tankUpdatePacket's size is 56\n");
				result = 0;
			}
		}
		else {
			int i = 0;
			memcpy(packetData + 56, &i, 4);
		}
	}

	return result;
}

int getMessageType(ENetPacket* p) {
	int result = 0;

	if (p->dataLength > 3u) result = *p->data;

	else 
		printf("Bad packet's data length, ignoring message\n");

	return result;
}

vector<string> explode(const string& delimeter, const string& str) {
	vector<string> v;

	int strLength = str.length(), delimeterLength = delimeter.length();

	if (delimeterLength == 0) return v;

	int i = 0, k = 0;

	while (i < strLength) {
		int j = 0;

		while (i + j < strLength && j < delimeterLength && str[i + j] == delimeter[j]) j++;

		if (j == delimeterLength) {
			v.push_back(str.substr(k, i - k));
			i += delimeterLength;
			k = i;
		}
		else i++;
	}

	v.push_back(str.substr(k, i - k));

	return v;
}

inline bool isHere(ENetPeer* peer, ENetPeer* peer2) {
	return ((PlayerInfo*)(peer->data))->currentWorld == ((PlayerInfo*)(peer2->data))->currentWorld;
}

string upperStr(string str) {
	string ret;

	for (char c : str) ret += toupper(c);

	return ret;
}

void sendWho(ENetPeer* peer) {
	for (ENetPeer* currentPeer : peers) {
		if (isHere(peer, currentPeer)) 
			Message::sendBuble(currentPeer, peer, ((PlayerInfo*)(currentPeer->data))->displayName);
	}
}