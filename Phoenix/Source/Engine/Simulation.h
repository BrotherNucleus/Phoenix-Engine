#pragma once

namespace Phoenix {

	class PHOENIX_API Simulation : public Win32::IApplication, public Win32::Window {
	public:
		Simulation();
		~Simulation();

		virtual VOID		PreInitialize() override;

		VOID				AddChildren();

		virtual LRESULT		MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	protected:
		std::unique_ptr<Win32::ChildWindow>	w_RightWindow;
		std::unique_ptr<Win32::ChildWindow>	w_LeftWindow;
		VOID LayoutChildren();
	};
}