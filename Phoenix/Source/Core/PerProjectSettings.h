#pragma once

class PHOENIX_API PerProjectSettings {

private:
	static PerProjectSettings* Inst;

	static PerProjectSettings* Instance() { return Inst; }

public: 
	PerProjectSettings();
	~PerProjectSettings();

private:
	WCHAR m_ProjectName[MAX_NAME_STRING];
	WCHAR m_ShortName[MAX_NAME_STRING];
	HICON m_MainIcon;
	WCHAR m_BootTime[MAX_NAME_STRING];
	WCHAR m_SplashURL[MAX_NAME_STRING];

public:

	static WCHAR* ProjectName() { return Instance()->m_ProjectName; }
	static WCHAR* ShortName() { return Instance()->m_ShortName; }
	static HICON MainIcon() { return Instance()->m_MainIcon; }
	static WCHAR* BootTime() { return Instance()->m_BootTime; }
	static WCHAR* SplashURL() { return Instance()->m_SplashURL; }

	static VOID SetProjectName(UINT id) { LoadString(HInstance(), id, Instance()->m_ProjectName, MAX_NAME_STRING); }
	static VOID SetShortName(UINT id) { LoadString(HInstance(), id, Instance()->m_ShortName, MAX_NAME_STRING); }
	static VOID SetMainIcon(UINT id) { LoadIcon(HInstance(), MAKEINTRESOURCE(id)); }
	static VOID SetSplashURL(UINT id) { LoadString(HInstance(), id, Instance()->m_SplashURL, MAX_NAME_STRING); }

};