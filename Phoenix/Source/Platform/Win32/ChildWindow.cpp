#include "Phoenix.h"
#include "ChildWindow.h"
namespace Win32 {
	ChildWindow::ChildWindow(WSTRING title, SIZE size, COLORREF color)
		: SubObject(title, title, nullptr), m_Size(size), m_Color(color)
	{
	}

	ChildWindow::~ChildWindow()
	{
		if(m_Handle && IsWindow(m_Handle)) {
			DestroyWindow(m_Handle);
		}
	}

	VOID ChildWindow::Initialize()
	{
		m_Handle = CreateWindowEx(0, m_Class.c_str(), m_Title.c_str(),
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
			0, 0, m_Size.cx, m_Size.cy,            // real size comes from the parent's layout
			m_Parent, nullptr, HInstance(), (void*)this);
	}

	LRESULT ChildWindow::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_ERASEBKGND:
				return 1;
			case WM_PAINT:
				{
					PAINTSTRUCT ps;
					HDC			hdc = BeginPaint(hWnd, &ps);
					RECT		rc; GetClientRect(hWnd, &rc);
					HBRUSH		brush = CreateSolidBrush(m_Color);
					FillRect(hdc, &rc, brush);
					DeleteObject(brush);
					EndPaint(hWnd, &ps);
					return 0;
				}
		}
		return SubObject::MessageHandler(hWnd, message, wParam, lParam);
	}
}