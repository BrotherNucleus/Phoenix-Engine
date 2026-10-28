#include "PhoenixEngine.h"

#include "Engine/Simulation.h"
#include "Platform/Win32/WinEntry.h"

class PhoenixEngine : public Phoenix::Simulation {

public:
	PhoenixEngine();

	~PhoenixEngine();

public:

	VOID SetupPerGameSettings();

	VOID Initialize();

	VOID Update();
};

ENTRYAPP(PhoenixEngine)

PhoenixEngine::PhoenixEngine()
{
}

PhoenixEngine::~PhoenixEngine()
{
}

VOID PhoenixEngine::SetupPerGameSettings()
{
	PerProjectSettings::SetProjectName(IDS_PERGAMENAME);
	PerProjectSettings::SetShortName(IDS_SHORTNAME);
	PerProjectSettings::SetMainIcon(IDI_MAINICON);
	PerProjectSettings::SetSplashURL(IDS_SPLASHURL);
}

VOID PhoenixEngine::Initialize()
{

}

VOID PhoenixEngine::Update()
{
}
