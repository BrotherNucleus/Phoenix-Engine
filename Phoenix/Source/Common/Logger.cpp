#include "Phoenix.h"
#include <fstream>
#include <shlobj.h>

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

	MessageBox(0, buffer, 0, 0);

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
	WCHAR path[MAX_PATH];
	WCHAR* AppDataLocal;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, & AppDataLocal);
	wcscpy_s(path, MAX_PATH, AppDataLocal);
	wcscat_s(path, MAX_PATH, L"\\");
	wcscat_s(path, MAX_PATH, PerProjectSettings::ProjectName());
	CreateDirectory(path, NULL);
	wcscat_s(path, L"\\Log");
	CreateDirectory(path, NULL);
	return path;
}

std::wstring Logger::LogFile()
{
	WCHAR path[MAX_PATH];
	wcscpy_s(path, MAX_PATH, PerProjectSettings::ProjectName());
	wcscat_s(path, MAX_PATH, PerProjectSettings::BootTime());
	wcscat_s(path, MAX_PATH, L".log");
	return path;
}