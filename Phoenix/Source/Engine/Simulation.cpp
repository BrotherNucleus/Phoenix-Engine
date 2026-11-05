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
		Logger::PrintLog(L"ClipChildren: %d\n", Win32::Utils::HasStyle(Handle(), WS_CLIPCHILDREN));
		w_LeftWindow = std::make_unique<Win32::ChildWindow>(L"Viewport", SIZE{ 640, 720 }, RGB(66, 46, 46));
		w_RightWindow = std::make_unique<Win32::ChildWindow>(L"Outliner", SIZE{ 640, 720 }, RGB(46, 46, 66));
		AddChildren();
		w_LeftWindow->CreateNewClass();   w_LeftWindow->Initialize();
		w_RightWindow->CreateNewClass();  w_RightWindow->Initialize();

		LayoutChildren();
		SplashScreen::Close();
		
	}

	VOID Simulation::AddChildren()
	{
		w_LeftWindow->AddParent(Handle());
		w_RightWindow->AddParent(Handle());
	}

	LRESULT Simulation::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SIZE:
			LayoutChildren();
			break;
		case WM_ERASEBKGND:
			return 1;                       // don't erase with the class brush
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);          // validate the region, draw nothing
			EndPaint(hWnd, &ps);
			return 0;
		}
		}

		return Window::MessageHandler(hWnd, message, wParam, lParam);
	}

	VOID Simulation::LayoutChildren()
	{
		if (!w_LeftWindow			|| !w_RightWindow			|| 
			!w_LeftWindow->Handle() || !w_RightWindow->Handle()) return;

		RECT rc;
		GetClientRect(Handle(), &rc);
		const int top = 0;                // leave room for your custom caption
		const int w = rc.right * 4 / 5;
		const int h = rc.bottom - top;

		MoveWindow(w_LeftWindow->Handle(), 0, top, w, h, TRUE);
		MoveWindow(w_RightWindow->Handle(), w, top, rc.right - w, h, TRUE);
	}

}