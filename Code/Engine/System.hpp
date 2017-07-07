#pragma once

#include "Object.hpp"

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// SystemEvent
	//----------------------------------------------------------------------------//

	struct SystemEvent
	{
		enum Enum
		{
			Startup = StringUtils::ConstHash("SystemEvent::Startup"),
			Start = StringUtils::ConstHash("SystemEvent::Start"),

			BeginFrame = StringUtils::ConstHash("SystemEvent::BeginFrame"),
			Update = StringUtils::ConstHash("SystemEvent::Update"),
			PostUpdate = StringUtils::ConstHash("SystemEvent::PostUpdate"),
			Render = StringUtils::ConstHash("SystemEvent::Render"),
			PostRender = StringUtils::ConstHash("SystemEvent::PostRender"),
			EndFrame = StringUtils::ConstHash("SystemEvent::EndFrame"),

			Stop = StringUtils::ConstHash("SystemEvent::Stop"),
			Shutdown = StringUtils::ConstHash("SystemEvent::Shutdown"),
		};
	};

	//----------------------------------------------------------------------------//
	// System
	//----------------------------------------------------------------------------//

	class System : public NonCopyable
	{
	public:
		//!
		System(void);
		//!
		virtual ~System(void);

		//!
		virtual bool OnEvent(int _type, void* _arg) { return false; }

		//!
		static bool SendEvent(int _event, void* _arg = nullptr, bool _defaultOrder = true);

	private:

		System* m_prev = nullptr;
		System* m_next = nullptr;
		static System* s_first;
		static System* s_last;
	};

	//----------------------------------------------------------------------------//
	// Module
	//----------------------------------------------------------------------------//

	template <class T> class Module : public System, public Singleton<T>
	{

	};

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}
