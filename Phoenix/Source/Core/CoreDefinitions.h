#pragma once

#ifdef BUILD_DLL
	#define PHOENIX_API __declspec(dllexport)
#else
	#define PHOENIX_API __declspec(dllimport)
#endif

#define MAX_PATH_PHX 1024
#define MAX_NAME_STRING 256
#define HInstance() GetModuleHandle(NULL)