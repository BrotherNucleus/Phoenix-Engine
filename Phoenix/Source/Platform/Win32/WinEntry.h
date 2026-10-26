#include "Phoenix.h"
#include "IApplication.h"

extern IApplication* EntryApplication();

int CALLBACK	WinMain(
	HINSTANCE,
	HINSTANCE,
	LPSTR,
	int
) {
	auto EntryApp = EntryApplication();

	PerProjectSettings Settings;

	EntryApp->SetupPerGameSettings();
	Logger logger;
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