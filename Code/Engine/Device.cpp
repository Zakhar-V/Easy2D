#pragma once

#include "Device.hpp"
#include <SDL.h>

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// Device
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	Device::Device(void)
	{
		LOG("Create Device");
	}
	//----------------------------------------------------------------------------//
	Device::~Device(void)
	{
		LOG("Destroy Device");
	}
	//----------------------------------------------------------------------------//
	bool Device::OnEvent(int _type, void* _arg)
	{
		switch (_type)
		{
		case SystemEvent::Startup:
		{
			if (!_Startup())
				return true; // error;

			SDL_GetWindowSize(m_window, &m_size.x, &m_size.y);

			m_opened = true;

		} break;

		case SystemEvent::Shutdown:
		{
			_Shutdown();
			m_opened = false;

		} break;

		case SystemEvent::BeginFrame:
		{
			SDL_Event _event;
			while (SDL_PollEvent(&_event))
			{
				switch (_event.type)
				{
				case SDL_WINDOWEVENT:
					switch (_event.window.event)
					{

					case SDL_WINDOWEVENT_RESIZED:
						m_size.x = _event.window.data1;
						m_size.y = _event.window.data2;
						break;

					case SDL_WINDOWEVENT_CLOSE:
						m_userRequireExit = true;
						break;
					}
					break;
				}
			}

		} break;

		case SystemEvent::EndFrame:
			SDL_GL_SwapWindow(m_window);
			break;
		}
		return false;
	}
	//----------------------------------------------------------------------------//
	void Device::RequireExit(bool _exit)
	{
		m_opened = !_exit;
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}
