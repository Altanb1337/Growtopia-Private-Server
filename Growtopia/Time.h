#pragma once
#include <chrono>
#include <string>

class Time {
public:
	// returns second since epoch
	static unsigned long now();

	// returns millisecond since epoch
	static unsigned long nowMillisecond();
};

class Date {
public:
	// converts seconds to date
	static std::string convertDate(unsigned long ulong);
};