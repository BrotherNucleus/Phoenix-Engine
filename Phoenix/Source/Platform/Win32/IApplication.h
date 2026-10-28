#pragma once

#define ENTRYAPP(x) Win32::IApplication* EntryApplication() { return new x; }

namespace Win32 {

	class PHOENIX_API IApplication {

	public:
		IApplication();

		virtual ~IApplication() {};

	public:
		virtual VOID SetupPerGameSettings() = 0;

		virtual VOID PreInitialize() = 0;

		virtual VOID Initialize() = 0;

		virtual VOID Update() = 0;
	};

	IApplication* EntryApplication();
}