#pragma once

namespace Win32 {
	class PHOENIX_API ChildWindow : public SubObject {
	public:
		ChildWindow(WSTRING title, SIZE size, COLORREF color);
		virtual ~ChildWindow();
		virtual VOID	Initialize() override;
		VOID			AddParent(HWND parent) { m_Parent = parent; }
		VOID			DeReferenceParent() { m_Parent = nullptr; }
		LRESULT			MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	protected:
		HWND		m_Parent;
		COLORREF	m_Color;
		SIZE		m_Size;
	};
}


