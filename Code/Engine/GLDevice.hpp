#pragma once

#include "Device.hpp"

typedef void* SDL_GLContext;

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// GLDevice
	//----------------------------------------------------------------------------//

#define gGLDevice static_cast<GLDevice*>(GLDevice::Instance)

	class GLDevice : public Device
	{
	public:

	protected:
		//!	Create window and graphics
		bool _Startup(void) override;
		//!	Destroy window and graphics
		void _Shutdown(void) override;

		SDL_GLContext m_context = nullptr;
	};

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}
