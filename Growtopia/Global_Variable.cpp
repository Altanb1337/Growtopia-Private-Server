#include "Global_Variable.h"
#include "stdafx.h"
#include <vector>
#include <string>

using namespace std;

vector<string> blacklistedName = {
		"prn", "con", "aux", "nul",
		"com1", "com2", "com3", "com4", "com5", "com6", "com7", "com8", "com9",
		"lpt1", "lpt2", "lpt3", "lpt4", "lpt5", "lpt6", "lpt7", "lpt8", "lpt9"
};

vector<ENetPeer*> peers = {};

WorldDB worldDB;

uint32_t itemsDatHash = 0;