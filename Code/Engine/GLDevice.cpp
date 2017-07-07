#include "GLDevice.hpp"
#include "GLGraphics.hpp"
#include <SDL.h>
#include <GL/gl_Load.h>

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// GLDevice
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	bool GLDevice::_Startup(void)
	{
		if (SDL_Init(SDL_INIT_EVERYTHING))
		{
			LOG("Error: SDL_Init falied");
			return false;
		}

		m_window = SDL_CreateWindow("Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

		if (!m_window)
		{
			LOG("Error: Unable to create window");
			return false;
		}

		m_context = SDL_GL_CreateContext(m_window);

		if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED)
		{
			LOG("Error: Unable to load OpenGL");
			return false;
		}

		new GLGraphics;

		return true;
	}
	//----------------------------------------------------------------------------//
	void GLDevice::_Shutdown(void)
	{
		if(gGLGraphics)
			delete gGLGraphics;

		if (m_context)
		{
			SDL_GL_DeleteContext(m_context);
			m_context = nullptr;
		}

		if (m_window)
		{
			SDL_DestroyWindow(m_window);
			m_window = nullptr;
		}
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	// 
	//----------------------------------------------------------------------------//
}
