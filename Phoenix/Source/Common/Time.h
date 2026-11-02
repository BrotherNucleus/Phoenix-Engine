#pragma once
#include <string>

namespace Time {

	WSTRING PHOENIX_API GetTime(BOOL stripped = false);

	WSTRING PHOENIX_API GetDate(BOOL stripped = false);

	WSTRING PHOENIX_API GetDateTime(BOOL stripped = false);
}