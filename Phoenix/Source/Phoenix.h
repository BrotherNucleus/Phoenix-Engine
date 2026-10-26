#pragma once


#ifdef WIN32

#include <Windows.h>

#endif

#ifdef BUILD_DLL
	#define PHOENIX_API __declspec(dllexport)
#else
	#define PHOENIX_API __declspec(dllimport)
#endif

#define MAX_PATH 1024
#define MAX_NAME_STRING 256
#define HInstance() GetModuleHandle(NULL)

#include "Common/Logger.h"
#include "Common/Time.h"
#include "Core/PerProjectSettings.h"

