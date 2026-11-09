#include "Phoenix.h"
#include "Simulation.h"
#include "Engine/SplashScreen.h"

namespace Phoenix {

	Simulation::Simulation()
		: Win32::Window(L"MainApplication", nullptr)
	{

	}

	Simulation::~Simulation()
	{
	}

	VOID Simulation::PreInitialize()
	{
		Logger::PrintDebugSeperator();
		Logger::PrintLog(L"Initializing Application...\n");
		Logger::PrintLog(L"Project Name: %s\n", PerProjectSettings::ProjectName());
		Logger::PrintLog(L"Short Name: %s\n", PerProjectSettings::ShortName());
		Logger::PrintLog(L"Boot Time: %s\n", PerProjectSettings::BootTime());
		Logger::PrintLog(L"Engine Mode: %s\n", Engine::GetModeString().c_str());
		Logger::PrintDebugSeperator();

		SplashScreen::Open();

		Win32::Window::CreateNewClass();
		Size(1280, 720);
		Win32::Window::Initialize();
		SplashScreen::Close();
		
	}

	LRESULT Simulation::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return Window::MessageHandler(hWnd, message, wParam, lParam);
	}

}