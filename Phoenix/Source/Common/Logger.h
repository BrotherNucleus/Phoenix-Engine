#pragma once

#include <string>

class PHOENIX_API Logger {

private:
	static Logger* Inst;
public:
	static Logger* Instance() { return Inst; }

public:
	Logger();
	~Logger();

	static VOID PrintLog(const WCHAR* Log, ...);

	static std::wstring LogDirectory();

	static std::wstring LogFile();

};