#pragma once

#define CB_CLOSE 0
#define CB_MINIMIZE 1
#define CB_MAXIMIZE 2

namespace Win32
{
	class Caption
	{
	public:
		struct CaptionButton
		{
			INT Command = -1;
			WSTRING Text = L"X";
			int Width = 50;
			RECT Rect;

			CaptionButton(WSTRING text, INT Command, int width = 50) : Text(text), Command(Command), Width(width) {
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


