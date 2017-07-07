#pragma once

#include "System.hpp"
#include "Math.hpp"

struct SDL_Window;

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// Device
	//----------------------------------------------------------------------------//

#define gDevice Easy2D::Device::Instance

	class Device abstract : public Module<Device>
	{
	public:
		//!
		Device(void);
		//!
		~Device(void);
		//!
		bool OnEvent(int _type, void* _arg) override;

		//!
		const IntVector2& WindowSize(void) { return m_size; }

		//!
		bool IsOpened(void) { return m_opened; }
		//!
		bool UserRequireExit(void) { return m_userRequireExit; }
		//!
		void RequireExit(bool _exit = true);


	protected:
		//!	Create window and graphics
		virtual bool _Startup(void) = 0;
		//!	Destroy window and graphics
		virtual void _Shutdown(void) = 0;

		SDL_Window* m_window = nullptr;
		IntVector2 m_size = { 0, 0 };

		bool m_opened = false;
		bool m_userRequireExit = false;
	};

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}
