#include "Phoenix.h"

PerProjectSettings* PerProjectSettings::Inst;

PerProjectSettings::PerProjectSettings()
{
	Inst = this;

	wcscpy_s(m_ProjectName, L"undefined");
	wcscpy_s(m_ShortName, L"undefined");
	wcscpy_s(m_BootTime, Time::GetDateTime(TRUE).c_str());
	wcscpy_s(m_SplashURL, L"..\\Phoenix\\Assets\\Images\\SplashScreen.bmp");
}

PerProjectSettings::~PerProjectSettings()
{
}