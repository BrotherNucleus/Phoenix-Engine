#include "Phoenix.h"
#include <fstream>
#include <shlobj.h>
#include <cstdio>
#include <tlhelp32.h>

Logger* Logger::Inst;

Logger::Logger()
{
	Inst = this;
}

Logger::~Logger()
{
}


VOID Logger::PrintLog(const WCHAR* Log, ...)
{
	WCHAR buffer[4096];
	va_list args;

	va_start(args, Log);
	vswprintf_s(buffer, Log, args);
	va_end(args);

	std::wfstream outfile;

	outfile.open(std::wstring(LogDirectory() + L"/" + LogFile()), std::ios_base::app);

	if (outfile.is_open())
	{
		std::wstring string = buffer;
		outfile << L"{" << Time::GetDateTime() << L"} " << string;
		outfile.close();
		OutputDebugString(string.c_str());
	}
	else
	{
		MessageBox(0, L"Failed to open log file.", 0, 0);
	}
}

std::wstring Logger::LogDirectory()
{
	WCHAR path[MAX_PATH_PHX];
	WCHAR* AppDataLocal;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, & AppDataLocal);
	wcscpy_s(path, MAX_PATH_PHX, AppDataLocal);
	wcscat_s(path, MAX_PATH_PHX, L"\\");
	wcscat_s(path, MAX_PATH_PHX, PerProjectSettings::ProjectName());
	CreateDirectory(path, NULL);
	wcscat_s(path, L"\\Log");
	CreateDirectory(path, NULL);
	return path;
}

std::wstring Logger::LogFile()
{
	WCHAR path[MAX_PATH_PHX];
	wcscpy_s(path, MAX_PATH_PHX, PerProjectSettings::ProjectName());
	wcscat_s(path, MAX_PATH_PHX, PerProjectSettings::BootTime());
	wcscat_s(path, MAX_PATH_PHX, L".log");
	return path;
}

VOID Logger::PrintDebugSeperator()
{
	std::wstring seperator = L"--------------------------------------------------\n\n";

#ifdef _DEBUG
	std::wfstream outfile;
	outfile.open(std::wstring(LogDirectory() + L"/" + LogFile()), std::ios_base::app);
	if (outfile.is_open())
	{
		outfile << seperator;
		outfile.close();
	}
	else {
		MessageBox(NULL, L"Failed to open log file.", L"Log Error", 0);
	}
#endif
	return VOID();
}

BOOL Logger::IsMTailRunning()
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (_wcsicmp(entry.szExeFile, L"mtail.exe") == 0)
			{
				exists = true;
			}
		}
	}

	CloseHandle(snapshot);
	return exists;
}

VOID Logger::StartMTail()
{
	if (IsMTailRunning())
	{
		Logger::PrintLog(L"MTail is already running.\n");
		return;
	}

	Logger::PrintLog(L"Starting MTail...\n");
	WCHAR path[MAX_PATH_PHX] = { 0 };
	GetCurrentDirectoryW(MAX_PATH_PHX, path);
	std::wstring url = path + std::wstring(L"/mTAIL.exe");
	std::wstring params = L"\"" + LogDirectory() + L"/" + LogFile() + L"\" /start";
	ShellExecute(0, NULL, url.c_str(), params.c_str(), NULL, SW_SHOWDEFAULT);
}
