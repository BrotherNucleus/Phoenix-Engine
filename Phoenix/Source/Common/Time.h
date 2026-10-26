#pragma once
#include <string>

namespace Time {

	std::wstring PHOENIX_API GetTime(BOOL stripped = false);

	std::wstring PHOENIX_API GetDate(BOOL stripped = false);

	std::wstring PHOENIX_API GetDateTime(BOOL stripped = false);
}