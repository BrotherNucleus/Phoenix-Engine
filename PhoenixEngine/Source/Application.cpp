#include "pch.h"
#include "Application.h"
#include "Platform/Win32/WinEntry.h"


ENTRYAPP(Application)

Application::Application()
{
}

Application::~Application()
{
}

VOID Application::Initialize()
{
	MessageBox(0, L"Hello World!", 0,0);
}

VOID Application::Update()
{
	MessageBox(0, L"Uodating...", 0, 0);
}
