#include "Phoenix.h"
#include "SubObject.h"


namespace Win32
{
	SubObject::SubObject(std::wstring className, std::wstring classTitle, HICON icon) 
		: m_Class(className), m_Title(classTitle), m_hIcon(icon)
	{
	}
	SubObject::~SubObject()
	{
	}

	VOID SubObject::CreateNewClass() {
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;

		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(46, 46, 46)));

		wcex.hIcon = m_hIcon;
		wcex.hIconSm = m_hIcon;

		wcex.lpszClassName = m_Class.c_str();

		wcex.lpszMenuName = nullptr;

		wcex.hInstance = HInstance();

		wcex.lpfnWndProc = SetupMassageHandler;

		RegisterClassEx(&wcex);
	}

	LRESULT SubObject::SetupMassageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		if(message == WM_NCCREATE) {
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Win32::SubObject* const pThis = static_cast<Win32::SubObject*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Win32::SubObject::AssignMassageHandler));
			return pThis->AssignMassageHandler(hWnd, message, wParam, lParam);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);

	}

	LRESULT SubObject::AssignMassageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		Win32::SubObject* const pThis = reinterpret_cast<Win32::SubObject*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		return pThis->MessageHandler(hWnd, message, wParam, lParam);
	}

	LRESULT SubObject::CommonMessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
