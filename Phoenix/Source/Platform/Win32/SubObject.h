#pragma once

namespace Win32
{
	class PHOENIX_API SubObject
	{
	public:

		HWND GetHandle() {	return m_Handle;	}	

		SubObject(std::wstring className, std::wstring classTitle, HICON icon);
		virtual ~SubObject();
		virtual VOID CreateNewClass();
		LRESULT WindowProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual VOID Initialize() = 0;
	protected:
		std::wstring m_Class;
		std::wstring m_Title;

		HICON m_hIcon;

		HWND m_Handle;

		static LRESULT CALLBACK SetupMassageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT AssignMassageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
		virtual LRESULT CommonMessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}