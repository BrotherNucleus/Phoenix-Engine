#pragma once
#include "Platform/Win32/SubObject.h"
#include "Platform/Win32/ChildWindow.h"

#include <Uxtheme.h>
	#pragma comment(lib, "uxtheme.lib")


namespace Win32
{
	class PHOENIX_API Window : public Win32::SubObject, public Win32::Caption
	{
	public:
		Window(WSTRING title, HICON icon, WindowType type = RESIZABLE);
		virtual ~Window();
		virtual VOID Initialize() override;

		LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

		VOID RedrawWindow();

		VOID OnNonClientCreate();

		VOID OnNonClientActivate(BOOL active);

		VOID OnNonClientPaint(HRGN);
		VOID PaintCaption(HDC hdc);
		VOID OnNonClientLMBDown();
		VOID OnGetMinMaxInfo(MINMAXINFO* minmax);
		VOID OnExitSizeMove();
		VOID OnPaint();
	protected:
		SIZE		m_Size;
		WindowType	m_WindowType;

		BOOL		m_bActive = false;
	public:
		SIZE Size()					{ return m_Size; }
		BOOL Active()				{ return m_bActive; }

		VOID Size(SIZE m_Size)		{ this->m_Size = m_Size; }
		VOID Size(INT cx, INT cy)	{ this->m_Size.cx = cx; this->m_Size.cy = cy; }
		VOID Active(BOOL active)	{ this->m_bActive = active; }
	};
}