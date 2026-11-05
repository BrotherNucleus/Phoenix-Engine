#pragma once

namespace Win32
{
	class PHOENIX_API SubObject
	{
	public:

		HWND Handle() {	return m_Handle;	}	

		VOID Handle(HWND handle) { m_Handle = handle; }

		SubObject(WSTRING className, WSTRING classTitle, HICON icon);
		virtual ~SubObject();
		virtual VOID CreateNewClass();
		virtual VOID Initialize() = 0;
		LRESULT WindowProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	protected:
		WSTRING m_Class;
		WSTRING m_Title;

		HICON m_hIcon;

		HWND m_Handle;

		static	LRESULT CALLBACK SetupMassageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static	LRESULT CALLBACK AssignMassageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}