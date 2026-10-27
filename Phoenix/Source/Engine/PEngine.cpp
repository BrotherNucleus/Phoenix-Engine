#include "Phoenix.h"

namespace Engine {
	PEngine g_PhoenixEngine;
	VOID PHOENIX_API SetMode(EngineMode mode)
	{
		g_PhoenixEngine.SetEngineMode(mode);
	}
	EngineMode PHOENIX_API GetMode()
	{
		return g_PhoenixEngine.GetEngineMode();
	}
	std::wstring PHOENIX_API GetModeString()
	{
		switch (Engine::GetMode())
		{
			case EngineMode::DEBUG:
				return L"DEBUG";
			case EngineMode::RELEASE:
				return L"RELEASE";
			case EngineMode::EDITOR:
				return L"EDITOR";
			case EngineMode::SERVER:
				return L"SERVER";
			default:
				return L"NONE";
		}
	}
}

PEngine::PEngine()
{
#ifdef _DEBUG
	m_EngineMode = EngineMode::DEBUG;
#else
	m_EngineMode = EngineMode::RELEASE;
#endif

}

PEngine::~PEngine()
{
}

void PEngine::SetEngineMode(EngineMode mode)
{
	m_EngineMode = mode;
}

EngineMode PEngine::GetEngineMode()
{
	return m_EngineMode;
}
