#include "Phoenix.h"
#include "Win32Utils.h"
namespace Win32 {
	namespace Utils {
		BOOL AddBitmap(const WCHAR* filePath, HDC hWinDC, INT x, INT y) {
			BITMAP qBitmap;
			HDC hLocalDC = CreateCompatibleDC(hWinDC);

			HBITMAP hBitmap = (HBITMAP)LoadImageW(NULL, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (hBitmap == NULL) {
				MessageBox(NULL, L"Failed to load bitmap", L"Error", MB_OK);
				return FALSE;
			}
			GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP), reinterpret_cast<LPVOID>(&qBitmap));

			SelectObject(hLocalDC, hBitmap);

			if (!BitBlt(hWinDC, x, y, qBitmap.bmWidth, qBitmap.bmHeight, hLocalDC, 0, 0, SRCCOPY)) {
				MessageBox(NULL, L"Failed to blit bitmap", L"Error", MB_OK);
				return FALSE;
			}

			::DeleteDC(hLocalDC);
			::DeleteObject(hBitmap);
			return TRUE;
		}
	}
}

