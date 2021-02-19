#pragma once
#include "stdafx.h"
#include <cctype>
#include "enet/enet.h"
#include "Player_Info.h"
#include "Global_Variable.h"
#include "World_Info.h"
#include <string>
#include <vector>
#include <fstream>

unsigned long byteSwap(unsigned long);

void sendData(ENetPeer*, int, char*, int);

int getPacketID(char*);

char* getPacketData(char*);

std::string encodeText(char*);

inline bool isNumber(std::string);

int ch2n(char);

char* getTextPointer(ENetPacket*);

unsigned char* getStructPointer(ENetPacket*);

int getMessageType(ENetPacket*);

// const string& delimeter, const string& stringToSplit
std::vector<std::string> explode(const std::string&, const std::string&);

inline bool isHere(ENetPeer*, ENetPeer*);

std::string upperStr(std::string);

void sendWho(ENetPeer* peer);