#include "Time.h"

using namespace std;

unsigned long Time::now() {
	using namespace chrono;
	return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

unsigned long Time::nowMillisecond() {
	using namespace chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

string Date::convertDate(unsigned long ulong) {
	string x;
	int day = ulong / (24 * 3600);
	if (day != 0) x.append(to_string(day) + " days, ");
	ulong = ulong % (24 * 3600);
	int hour = ulong / 3600;
	if (hour != 0) x.append(to_string(hour) + " hours, ");
	ulong %= 3600;
	int minutes = ulong / 60;
	if (minutes != 0) x.append(to_string(minutes) + " mins, ");
	ulong %= 60;
	int seconds = ulong;

	x.append(to_string(seconds) + " secs");

	return x;
}