#include "Phoenix.h"
#include "SplashScreen.h"


namespace SplashScreen
{
#define WM_OUTPUTMESSAGE (WM_USER + 0x0001)

	SplashWindow* m_SplashWindow = nullptr;

	VOID Open()
	{
		if (m_SplashWindow != nullptr) {
			return;
		}
		m_SplashWindow = new SplashWindow();
	}

	VOID Close()
	{
		SendMessage(m_SplashWindow->Handle(), WM_CLOSE, 0, 0);
	}

	VOID AddMessage(const WCHAR* message)
	{
		PostMessage(m_SplashWindow->Handle(), WM_OUTPUTMESSAGE, (WPARAM)message, 0);
	}
}

SplashWindow::SplashWindow()
	: Win32::Window(L"SplashScreen", nullptr, Win32::WindowType::POPUP)
{
	wcscpy_s(m_outputMessage, L"Initializing Phoenix Engine...");
	Win32::Window::CreateNewClass();
	Win32::Window::Initialize();
}

SplashWindow::~SplashWindow()
{
}

LRESULT SplashWindow::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_PAINT:
		{
			HBITMAP hbitmap;
			HDC hdc, hmemdc;
			PAINTSTRUCT ps;

			hdc = BeginPaint(hWnd, &ps);

			Win32::Utils::AddBitmap(PerProjectSettings::SplashURL(), hdc);

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(255, 255, 255));

			if (Engine::GetMode() != Engine::EngineMode::RELEASE) {
				WSTRING engineModeText = Engine::GetModeString() + L" Mode";
				SetTextAlign(hdc, TA_CENTER);
				TextOut(hdc, Size().cy - 60, 15, engineModeText.c_str(), wcslen(engineModeText.c_str()));
			}

			SetTextAlign(hdc, TA_CENTER);
			TextOut(hdc, Size().cx / 2, Size().cy - 30, m_outputMessage, wcslen(m_outputMessage));
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_OUTPUTMESSAGE:
		{
			const WCHAR* msg = (WCHAR*)wParam;
			wcscpy_s(m_outputMessage, msg);
			RedrawWindow();
			return 0;
		}
	}

	return Window::MessageHandler(hWnd, message, wParam, lParam);
}
