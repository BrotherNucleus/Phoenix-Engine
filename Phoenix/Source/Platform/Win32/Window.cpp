#include "Phoenix.h"
#include "Window.h"

#define DCX_USESTYLE 0x00010000

namespace Win32
{
	Window::Window(WSTRING title, HICON icon, WindowType type)
		: Win32::SubObject(title, title, icon), m_WindowType(type)
	{
		m_Size = SIZE{ DEFAULT_WIDTH, DEFAULT_HEIGHT };
	}
	Window::~Window()
	{
	}

	VOID Window::Initialize() {

		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		RECT R = { 0, 0, Size().cx, Size().cy};
		AdjustWindowRect(&R, m_WindowType, FALSE);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		m_Handle = CreateWindow(
			m_Class.c_str(),
			m_Title.c_str(),
			m_WindowType,
			((desktop.right - width) / 2),
			((desktop.bottom - height) / 2), width, height,
			0,
			0,
			HInstance(),
			(void*)this
		);
		ShowWindow(Handle(), SW_SHOW);
		UpdateWindow(Handle());
	}
	LRESULT Window::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{

		switch (message)
		{
		case WM_NCCREATE: { OnNonClientCreate(); } return true;
		case WM_NCACTIVATE: { OnNonClientActivate(LOWORD(wParam) != WA_INACTIVE); } return true;
		case WM_NCPAINT: { OnNonClientPaint((HRGN)wParam); } return false;
		case WM_TIMER: { RedrawWindow(); } break;
		}
		return SubObject::MessageHandler(hWnd, message, wParam, lParam);
	}

	VOID Window::RedrawWindow() {
		SetWindowPos(Handle(), 0, 0, 0, 0, 0, 
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME | SWP_FRAMECHANGED);
		SendMessage(Handle(), WM_PAINT, 0, 0);
	}

	VOID Window::OnNonClientCreate()
	{
		SetTimer(Handle(), 1, 100, NULL);
		SetWindowTheme(Handle(), L"", L"");

		Win32::Utils::ModifyClassStyle(Handle(), 0, CS_DROPSHADOW);

		Win32::Caption::AddCaptionButton(new CaptionButton(L"X"));
		Win32::Caption::AddCaptionButton(new CaptionButton(L"F"));
		Win32::Caption::AddCaptionButton(new CaptionButton(L"M"));
	}

	VOID Window::OnNonClientActivate(BOOL active)
	{
		Active(active);
	}

	VOID Window::OnNonClientPaint(HRGN region)
	{
		HDC hdc = GetDCEx(Handle(), region, DCX_WINDOW | DCX_INTERSECTRGN | DCX_USESTYLE);

		RECT rect;
		GetWindowRect(Handle(), &rect);

		SIZE size = SIZE {rect.right - rect.left, rect.bottom - rect.top};

		HBITMAP hBitmap = CreateCompatibleBitmap(hdc, size.cx, size.cy);
		HANDLE hOld = SelectObject(hdc, hBitmap);

		RECT newRect = { 0, 0, size.cx, size.cy };




		HBRUSH hBrush = CreateSolidBrush(RGB(36, 36, 36));

		FillRect(hdc,&newRect, hBrush);		
		if (Active()) {
			hBrush = CreateSolidBrush(RGB(250, 100, 15));
			FrameRect(hdc, &newRect, hBrush);
		}

		PaintCaption(hdc);

		DeleteObject(hBrush);




		BitBlt(hdc, 0, 0, size.cx, size.cy, hdc, 0, 0, SRCCOPY);
		DeleteObject(hBitmap);

		ReleaseDC(Handle(), hdc);
	}

	VOID Window::PaintCaption(HDC hdc) {

		RECT rect;
		GetWindowRect(Handle(), &rect);

		SIZE size = SIZE{ rect.right - rect.left, rect.bottom - rect.top };

		if (ShowTitle()) {

			rect = RECT{ 0, 0, size.cx, 30 };

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, Active() ? RGB(255, 255, 255) : RGB(128, 128, 128));

			DrawText(hdc, m_Title.c_str(),wcslen(m_Title.c_str()), &rect, DT_SINGLELINE | DT_VCENTER  | DT_CENTER);
			
		}
		int spacing = 0;
		for (auto &button : Caption::CaptionButtons()) {
			spacing += button.Width;
			button.Rect = RECT{ size.cx - spacing, 0, size.cx - spacing + button.Width, 30 };
			DrawText(hdc, button.Text.c_str(), wcslen(button.Text.c_str()), &button.Rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
	}
}
