#pragma once

#include "Platform/Win32/IApplication.h"

class Application : public IApplication {

public:
	Application();

	~Application();

public:

	VOID SetupPerGameSettings();

	VOID Initialize();

	VOID Update();
};