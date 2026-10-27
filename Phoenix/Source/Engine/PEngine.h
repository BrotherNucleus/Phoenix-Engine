#pragma once

class PHOENIX_API PEngine;

namespace Engine {
	enum EngineMode : INT {
		NONE,
		DEBUG,
		RELEASE,
		EDITOR,
		SERVER
	};

	extern PEngine g_PhoenixEngine;

	VOID PHOENIX_API SetMode(EngineMode mode);
	EngineMode PHOENIX_API GetMode();

	std::wstring PHOENIX_API GetModeString();
}

using namespace Engine;

class PHOENIX_API PEngine {
public:
	PEngine();
	~PEngine();

	void SetEngineMode(EngineMode mode);
	EngineMode GetEngineMode();

private:
	EngineMode m_EngineMode;
};