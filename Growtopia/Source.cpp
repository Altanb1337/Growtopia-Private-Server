#pragma warning (disable : 4101)

#include "Event_Handler.h"

int main() {

	try { runEvent(); }

	catch (int e) {
		printf("Something has just went wrong...\n");
	}

	return 0;
}