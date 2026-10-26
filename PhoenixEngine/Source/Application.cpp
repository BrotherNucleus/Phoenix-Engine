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
	Logger::PrintLog(L"Hello World! %s, Thank You!", L"Phoenix");
}

VOID Application::Update()
{
	MessageBox(0, L"Update", 0, 0);
}
