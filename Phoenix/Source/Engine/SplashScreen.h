#pragma once

#include "Platform/Win32/Window.h"

namespace SplashScreen
{
	VOID PHOENIX_API Open();
	VOID PHOENIX_API Close();
	VOID PHOENIX_API AddMessage(const WCHAR* message);
}

class PHOENIX_API SplashWindow : public Win32::Window{
public:
	SplashWindow();
	~SplashWindow();

	virtual LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

private:
	WCHAR m_outputMessage[MAX_NAME_STRING];
};