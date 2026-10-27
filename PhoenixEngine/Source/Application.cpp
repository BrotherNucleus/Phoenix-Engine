#include "pch.h"
#include "Application.h"
#include "Platform/Win32/WinEntry.h"


ENTRYAPP(Application)

Application::Application()
{
}

Application::~Application()
{
}

VOID Application::SetupPerGameSettings()
{
	PerProjectSettings::SetProjectName(IDS_PERGAMENAME);
	PerProjectSettings::SetShortName(IDS_SHORTNAME);
	PerProjectSettings::SetMainIcon(IDI_MAINICON);
}

VOID Application::Initialize()
{
	Logger::PrintDebugSeperator();
	Logger::PrintLog(L"Initializing Application...\n");
	Logger::PrintLog(L"Project Name: %s\n", PerProjectSettings::ProjectName());
	Logger::PrintLog(L"Short Name: %s\n", PerProjectSettings::ShortName());
	Logger::PrintLog(L"Boot Time: %s\n", PerProjectSettings::BootTime());
	Logger::PrintLog(L"Engine Mode: %s\n", Engine::GetModeString().c_str());
	Logger::PrintDebugSeperator();
}

VOID Application::Update()
{
}
