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

		RECT		desktop;
		const HWND	hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		RECT R =		{ 0, 0, Size().cx, Size().cy };
		AdjustWindowRect(&R, m_WindowType, FALSE);
		int width =		R.right - R.left;
		int height =	R.bottom - R.top;

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
		case WM_NCCREATE:			{ OnNonClientCreate(); } return true;
		case WM_NCACTIVATE:			{ OnNonClientActivate(LOWORD(wParam) != WA_INACTIVE); } return true;
		case WM_NCPAINT:			{ OnNonClientPaint((HRGN)wParam); } return false;
		case WM_NCLBUTTONDOWN:		{ OnNonClientLMBDown(); } break;
		case WM_NCLBUTTONDBLCLK:	{ Win32::Utils::MaximizeWindow(Handle()); } return 0;


		case WM_GETMINMAXINFO:		{ OnGetMinMaxInfo((MINMAXINFO*)lParam); } return 0;
		case WM_EXITSIZEMOVE:		{ OnExitSizeMove(); } break;
		case WM_PAINT:				{ OnPaint(); } break;	
		case WM_TIMER:				{ RedrawWindow(); } break;
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

		Win32::Caption::AddCaptionButton(new CaptionButton(L"X", CB_CLOSE));
		Win32::Caption::AddCaptionButton(new CaptionButton(L"F", CB_MAXIMIZE));
		Win32::Caption::AddCaptionButton(new CaptionButton(L"M", CB_MINIMIZE));
	}

	VOID Window::OnNonClientActivate(BOOL active)
	{
		Active(active);
	}

	VOID Window::OnNonClientPaint(HRGN region)
	{
		HDC		hdc =		GetDCEx(Handle(), region, DCX_WINDOW | DCX_INTERSECTRGN | DCX_USESTYLE);
		RECT	rect;
		GetWindowRect(Handle(), &rect);
		SIZE	size =		SIZE{ rect.right - rect.left, rect.bottom - rect.top };
		HBITMAP hBitmap =	CreateCompatibleBitmap(hdc, size.cx, size.cy);
		HANDLE	hOld =		SelectObject(hdc, hBitmap);
		RECT	newRect =	{ 0, 0, size.cx, size.cy };
		HBRUSH	hBrush =	CreateSolidBrush(RGB(36, 36, 36));

		FillRect(hdc, &newRect, hBrush);
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

		RECT	rect;
		GetWindowRect(Handle(), &rect);
		SIZE	size = SIZE{ rect.right - rect.left, rect.bottom - rect.top };
		if (ShowTitle()) {

			rect = RECT{ 0, 0, size.cx, 30 };

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, Active() ? RGB(255, 255, 255) : RGB(128, 128, 128));

			DrawText(hdc, m_Title.c_str(), wcslen(m_Title.c_str()), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

		}
		int spacing = 0;

		POINT	cursorPos;
		GetCursorPos(&cursorPos);

		GetWindowRect(Handle(), &rect);

		cursorPos.x -= rect.left;
		cursorPos.y -= rect.top;

		for (auto& button : Caption::CaptionButtons()) {
			spacing += button.Width;
			button.Rect = RECT{ size.cx - spacing, 0, size.cx - spacing + button.Width, 30 };

			if (button.Rect.left <= cursorPos.x && button.Rect.right >= cursorPos.x
				&& button.Rect.top <= cursorPos.y && button.Rect.bottom >= cursorPos.y) {
				HBRUSH hBrush = CreateSolidBrush(RGB(92, 92, 92));
				FillRect(hdc, &button.Rect, hBrush);
				DeleteObject(hBrush);
			}
			else {
				SetBkMode(hdc, TRANSPARENT);
				SetTextColor(hdc, Active() ? RGB(255, 255, 255) : RGB(128, 128, 128));
			}

			if (button.Text.compare(L"F") == 0 && Win32::Utils::IsWindowFullscreen(Handle())) {
				button.Text = L"W";
			} else if (button.Text.compare(L"W") == 0 && !Win32::Utils::IsWindowFullscreen(Handle())) {
				button.Text = L"F";
			}

			DrawText(hdc, button.Text.c_str(), wcslen(button.Text.c_str()), &button.Rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
	}

	VOID Window::OnNonClientLMBDown()
	{
		RECT		windowRect;
		GetWindowRect(Handle(), &windowRect);

		const LONG	width = windowRect.right - windowRect.left;
		const LONG	captionHeight = 30;

		POINT		cursorPosition;
		GetCursorPos(&cursorPosition);

		cursorPosition.x -= windowRect.left;
		cursorPosition.y -= windowRect.top;

		int spacing = 0;

		for (auto& button : Caption::CaptionButtons()) {
			button.Rect = RECT{
				width - spacing - button.Width,
				0,
				width - spacing,
				captionHeight
			};

			spacing += button.Width;

			if (PtInRect(&button.Rect, cursorPosition)) {
				switch (button.Command) {
				case CB_CLOSE:
					SendMessage(Handle(), WM_CLOSE, 0, 0);
					PostQuitMessage(0);
					break;
				case CB_MAXIMIZE:
					Win32::Utils::MaximizeWindow(Handle());
					break;
				case CB_MINIMIZE:
					ShowWindow(Handle(), SW_MINIMIZE);
					break;
				}
				return;
			}
		}
	}

	VOID Window::OnGetMinMaxInfo(MINMAXINFO* minmax) {
		RECT WorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &WorkArea, 0);
		minmax->ptMaxSize.x = WorkArea.right - WorkArea.left;
		minmax->ptMaxSize.y = WorkArea.bottom - WorkArea.top;
		minmax->ptMaxPosition.x = WorkArea.left;
		minmax->ptMaxPosition.y = WorkArea.top;
		minmax->ptMinTrackSize.x = 400;
		minmax->ptMinTrackSize.y = 300;
	}

	VOID Window::OnExitSizeMove() {
		RECT rect;
		GetWindowRect(Handle(), &rect);
		RECT WorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &WorkArea, 0);
		if (rect.top < WorkArea.top + 10 && !Win32::Utils::IsWindowFullscreen(Handle())) {
			Win32::Utils::MaximizeWindow(Handle());
		}
	}

	VOID Window::OnPaint() {
		PAINTSTRUCT ps;
		HDC			hdc = BeginPaint(Handle(), &ps);
		RECT		rect;
		GetClientRect(Handle(), &rect);
		HBRUSH		hBrush = CreateSolidBrush(RGB(36, 36, 36));
		FillRect(hdc, &rect, hBrush);
		DeleteObject(hBrush);
		EndPaint(Handle(), &ps);
	}
}