#include "Phoenix.h"
#include "IApplication.h"
#include "Common/CmdLArgs.h"

extern Win32::IApplication* EntryApplication();

int CALLBACK	WinMain(
	HINSTANCE,
	HINSTANCE,
	LPSTR,
	int
) {
	auto EntryApp = EntryApplication();

	PerProjectSettings Settings;

	EntryApp->SetupPerProjectSettings();
	CmdLineArgs::ReadArguments();
	Logger logger;

	EntryApp->PreInitialize();
	EntryApp->Initialize();

	MSG msg = { 0 };
	int loggedMessages = 0;
	int loggedUpdates = 0;
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (loggedMessages < 60)
			{
				Logger::PrintLog(L"msg 0x%04X hwnd %p\n", msg.message, msg.hwnd);
				++loggedMessages;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (loggedUpdates < 3)
			{
				Logger::PrintLog(L"Update called\n");
				++loggedUpdates;
			}
			EntryApp->Update();
		}
	}

	delete EntryApp;
	return 0;
}