#pragma once

#include "Platform/Win32/IApplication.h"

namespace Phoenix {

	class PHOENIX_API Simulation : public Win32::IApplication{
	public:
		Simulation();
		~Simulation();
	};
}