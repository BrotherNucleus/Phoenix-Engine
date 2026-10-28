#include "Phoenix.h"
#include "Simulation.h"
#include "Engine/SplashScreen.h"

Phoenix::Simulation::Simulation()
{
}

Phoenix::Simulation::~Simulation()
{
}

VOID Phoenix::Simulation::PreInitialize()
{
	Logger::PrintDebugSeperator();
	Logger::PrintLog(L"Initializing Application...\n");
	Logger::PrintLog(L"Project Name: %s\n", PerProjectSettings::ProjectName());
	Logger::PrintLog(L"Short Name: %s\n", PerProjectSettings::ShortName());
	Logger::PrintLog(L"Boot Time: %s\n", PerProjectSettings::BootTime());
	Logger::PrintLog(L"Engine Mode: %s\n", Engine::GetModeString().c_str());
	Logger::PrintDebugSeperator();

	SplashScreen::Open();
}
