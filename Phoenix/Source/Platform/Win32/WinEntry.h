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

	EntryApp->SetupPerGameSettings();
	CmdLineArgs::ReadArguments();
	Logger logger;

	EntryApp->PreInitialize();
	EntryApp->Initialize();

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			EntryApp->Update();
		}
	}

	return 0;
}