#pragma once


#ifdef WIN32

#include <Windows.h>

#endif

#ifdef BUILD_DLL
	#define PHOENIX_API __declspec(dllexport)
#else
	#define PHOENIX_API __declspec(dllimport)
#endif