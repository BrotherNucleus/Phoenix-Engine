#pragma once

#include "Engine/Simulation.h"

class Application : public Phoenix::Simulation {

public:
	Application();

	~Application();

public:

	VOID SetupPerGameSettings();

	VOID Initialize();

	VOID Update();
};