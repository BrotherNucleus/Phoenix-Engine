#pragma once

namespace Win32
{
	class Caption
	{
	public:
		struct CaptionButton
		{
			WSTRING Text = L"X";
			int Width = 50;
			RECT Rect;

			CaptionButton(WSTRING text, int width = 50) : Text(text), Width(width) {
			}
		};

		std::list<CaptionButton> m_CaptionButtons = {
		};

	private: 
		BOOL m_ShowTitle = TRUE;

	public:
		BOOL ShowTitle() { return m_ShowTitle; }
		std::list<CaptionButton> CaptionButtons() { return m_CaptionButtons; }

		VOID ShowTitle(BOOL show) { m_ShowTitle = show; }

		VOID AddCaptionButton(CaptionButton* button);
	};
}


