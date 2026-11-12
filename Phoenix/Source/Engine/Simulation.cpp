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

		m_Renderer = new CoreRenderer();
		try
		{
			m_Renderer->Initialize(Handle());
		}
		catch (const std::exception&)
		{
			Logger::PrintLog(L"Renderer initialization failed.\n");
			MessageBox(Handle(), L"Failed to initialize Direct3D 12. See the log for details.", L"Phoenix", MB_OK | MB_ICONERROR);
			delete m_Renderer;
			m_Renderer = nullptr;
			PostQuitMessage(1);
		}
		SplashScreen::Close();
		
	}

	VOID Simulation::Update()
	{
		if (m_Renderer == nullptr)
			return;

		try
		{
			m_Renderer->Render();
		}
		catch (const std::exception&)
		{
			Logger::PrintLog(L"Renderer failure, shutting down.\n");
			delete m_Renderer;
			m_Renderer = nullptr;
			PostQuitMessage(1);
		}
	}

	LRESULT Simulation::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_PAINT:
		{
			// D3D owns the client area: validate it, but never GDI-fill it (Window::OnPaint would).
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		return 0;

		case WM_SIZE:
			// lParam = new client size. Also fires with (0,0) on minimise.
			if (m_Renderer != nullptr && m_Renderer->IsInitialized())
			{
				m_Renderer->Resize(LOWORD(lParam), HIWORD(lParam));
			}
			break;
		case WM_ERASEBKGND: return 1;
		}


		return Window::MessageHandler(hWnd, message, wParam, lParam);
	}

}