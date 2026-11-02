#include "Phoenix.h"
#include <ctime>
#include <sstream>
#include <iomanip>

WSTRING PHOENIX_API Time::GetTime(BOOL stripped)
{
	time_t now = time(0);
	tm localTime;
	localtime_s(&localTime, &now);
	std::wstringstream wss;
	wss << std::put_time(&localTime, L"%T");

	WSTRING timeString = wss.str();

	if (stripped) {
		WSTRING chars = L":";
		for (WCHAR c : chars) {
			timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
		}
	}
	return timeString;
}

WSTRING PHOENIX_API Time::GetDate(BOOL stripped)
{
	time_t now = time(0);
	tm localTime;
	localtime_s(&localTime, &now);
	std::wstringstream wss;
	wss << std::put_time(&localTime, L"%Y-%m-%d");

	WSTRING timeString = wss.str();

	if (stripped) {
		WSTRING chars = L":";
		for (WCHAR c : chars) {
			timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
		}
	}
	return timeString;
}

WSTRING PHOENIX_API Time::GetDateTime(BOOL stripped)
{
	WSTRING timeString = GetDate(stripped) + L" " + GetTime(stripped);

	if (stripped) {
		WSTRING chars = L":";
		for (WCHAR c : chars) {
			timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
		}
	}
	return timeString;
}
