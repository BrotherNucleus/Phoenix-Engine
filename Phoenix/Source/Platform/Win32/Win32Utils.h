#pragma once

#include "Phoenix.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

namespace Win32 {

	enum WindowType : DWORD {
		STATIC = WS_OVERLAPPED,
		RESIZABLE = WS_SIZEBOX,
		POPUP = WS_POPUP,
	};

	namespace Utils {
		BOOL PHOENIX_API AddBitmap(CONST WCHAR* filePath, HDC hWinDC, INT x = 0, INT y = 0);

		inline VOID PHOENIX_API ModifyWindowStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd) {
			DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
			SetWindowLong(hWnd, GWL_STYLE, (dwStyle & ~dwRemove) | dwAdd);
		}

		inline VOID PHOENIX_API ModifyWindowExStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd) {
			DWORD dwStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
			SetWindowLong(hWnd, GWL_EXSTYLE, (dwStyle & ~dwRemove) | dwAdd);
		}

		inline BOOL PHOENIX_API HasStyle(HWND hWnd, DWORD dwStyle) {
			DWORD dwCurrentStyle = GetWindowLong(hWnd, GWL_STYLE);
			if ((dwCurrentStyle & dwStyle) != 0) return TRUE;
			return FALSE;
		}

		inline VOID PHOENIX_API ModifyClassStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd) {
			DWORD dwStyle = GetWindowLong(hWnd, GCL_STYLE);
			SetClassLong(hWnd, GCL_STYLE, (dwStyle & ~dwRemove) | dwAdd);
		}
	}
}