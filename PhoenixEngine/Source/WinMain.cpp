#include "pch.h"

WCHAR WindowClass[MAX_NAME_STRING];
WCHAR WindowTitle[MAX_NAME_STRING];

INT	WindowHeight;
INT WindowWidth;

HICON hIcon;

LRESULT CALLBACK WindowProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int CALLBACK	WinMain(
	HINSTANCE,
	HINSTANCE,
	LPSTR,
	int
) {

	/* INIT Global Variables */

	WindowHeight = 720;
	WindowWidth = 1280;

	hIcon = LoadIcon(HInstance(), MAKEINTRESOURCE(IDI_MAINICON));

	wcscpy_s(WindowClass, L"Window");
	wcscpy_s(WindowTitle, L"Window");

	/* Create window Class*/

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;

	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(NULL_BRUSH);

	wcex.hIcon = hIcon;
	wcex.hIconSm = hIcon;

	wcex.lpszClassName = WindowClass;
	
	wcex.lpszMenuName = nullptr;

	wcex.hInstance = HInstance();

	wcex.lpfnWndProc = WindowProcess;

	RegisterClassEx(&wcex);

	/* Create and Display Window */

	HWND hWnd = CreateWindowEx(
		0,
		WindowClass,
		WindowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, WindowWidth, WindowHeight,
		nullptr,
		nullptr,
		HInstance(),
		nullptr
	);

	if (!hWnd) 
	{
		MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK);
		return 0;
	}

	ShowWindow(hWnd, SW_SHOW);

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Game loop code would go here
		}
	}

	return 0;
}