#include "Phoenix.h"
#include <ctime>
#include <sstream>
#include <iomanip>

std::wstring PHOENIX_API Time::GetTime(BOOL stripped)
{
	time_t now = time(0);
	tm localTime;
	localtime_s(&localTime, &now);
	std::wstringstream wss;
	wss << std::put_time(&localTime, L"%T");

	std::wstring timeString = wss.str();

	if (stripped) {
		std::wstring chars = L":";
		for (WCHAR c : chars) {
			timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
		}
	}
	return timeString;
}

std::wstring PHOENIX_API Time::GetDate(BOOL stripped)
{
	time_t now = time(0);
	tm localTime;
	localtime_s(&localTime, &now);
	std::wstringstream wss;
	wss << std::put_time(&localTime, L"%Y-%m-%d");

	std::wstring timeString = wss.str();

	if (stripped) {
		std::wstring chars = L":";
		for (WCHAR c : chars) {
			timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
		}
	}
	return timeString;
}

std::wstring PHOENIX_API Time::GetDateTime(BOOL stripped)
{
	std::wstring timeString = GetDate(stripped) + L" " + GetTime(stripped);

	if (stripped) {
		std::wstring chars = L":";
		for (WCHAR c : chars) {
			timeString.erase(std::remove(timeString.begin(), timeString.end(), c), timeString.end());
		}
	}
	return timeString;
}
