
#include "Easy2D.hpp"

#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <Windows.h>
#include <GL/GL.h>

#pragma comment(lib, "opengl32.lib")
#include "stb_image.h" // https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// StringUtils
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	uint StringUtils::Hash(const char* _str, uint _hash)
	{
		if (!_str)
			_str = "";
		while (*_str)
			_hash = ((_hash >> 1) + ((_hash & 1) << 15) + Lower(*_str++)) & 0xffff;
		return _hash;
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	// RefCounted
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	RefCounted::RefCounted(void)
		: m_rc(new Counter)
	{
		m_rc->object = this;
	}
	//----------------------------------------------------------------------------//
	RefCounted::~RefCounted(void)
	{
		ASSERT(m_rc == nullptr);
	}
	//----------------------------------------------------------------------------//
	void RefCounted::AddRef(void)
	{
		m_rc->ref++;
	}
	//----------------------------------------------------------------------------//
	void RefCounted::Release(void)
	{
		if (!--m_rc->ref)
			_DeleteThis();
	}
	//----------------------------------------------------------------------------//
	void RefCounted::_DeleteThis(void)
	{
		m_rc->object = nullptr;
		m_rc->Release();
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	// Object
	//----------------------------------------------------------------------------//

	HashMap<uint, Object::TypeInfo> Object::s_types;

	//----------------------------------------------------------------------------//
	Object::TypeInfo* Object::GetOrCreateTypeInfo(const char* _name)
	{
		uint _type = StringUtils::Hash(_name);
		auto _iter = s_types.find(_type);
		if (_iter != s_types.end())
			return &_iter->second;

		LOG("Register %s(0x%04x) typeinfo", _name, _type);

		auto& _typeInfo = s_types[_type];
		_typeInfo.type = _type;
		_typeInfo.name = _name;

		return &_typeInfo;
	}
	//----------------------------------------------------------------------------//
	Object::TypeInfo* Object::GetTypeInfo(uint _type)
	{
		auto _iter = s_types.find(_type);
		if (_iter != s_types.end())
			return &_iter->second;
		return nullptr;
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	// OpenGL
	//----------------------------------------------------------------------------//

	bool (APIENTRY* wglSwapIntervalEXT)(int) = nullptr;

	//----------------------------------------------------------------------------//
	// Engine
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	Engine::Engine(void)
	{
		// window class
		WNDCLASSA _wc;
		{
			memset(&_wc, 0, sizeof(_wc));
			_wc.style = /*CS_HREDRAW | CS_VREDRAW | CS_OWNDC |*/ CS_DBLCLKS;
			_wc.lpfnWndProc = reinterpret_cast<decltype(DefDlgProcA)*>(&_WindowCallback);
			_wc.hInstance = GetModuleHandleA("");
			_wc.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
			_wc.hCursor = LoadCursorA(nullptr, IDC_ARROW);
			_wc.lpszClassName = "test";
			m_wndcls = RegisterClassA(&_wc);
			CHECK(m_wndcls != 0, "RegisterClassA Failed");
		}


		// create window
		{
#if 0 // fullscreen
			int _width = GetSystemMetrics(SM_CXSCREEN);
			int _height = GetSystemMetrics(SM_CYSCREEN);

			m_window = CreateWindowA(_wc.lpszClassName, "GL benchmark", WS_POPUP | WS_VISIBLE, 0, 0, _width, _height, HWND_DESKTOP, nullptr, _wc.hInstance, nullptr);
			m_fullscreen = true;
#else
			m_window = CreateWindowA(_wc.lpszClassName, "GL benchmark", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP, nullptr, _wc.hInstance, nullptr);
			m_fullscreen = false;
#endif

			CHECK(m_window != nullptr, "CreateWindowA Failed");
		}

		// get window size
		{
			RECT _rect;
			GetClientRect((HWND)m_window, &_rect);
			m_size.x = _rect.right - _rect.left;
			m_size.y = _rect.bottom - _rect.top;
		}

		// get cursor pos
		{

		}


		m_dc = GetDC((HWND)m_window);

		// pixel format
		{
			PIXELFORMATDESCRIPTOR _pfd;
			memset(&_pfd, 0, sizeof(_pfd));
			_pfd.nSize = sizeof(_pfd);
			_pfd.nVersion = 1;
			_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			_pfd.cColorBits = 32;
			_pfd.cDepthBits = 24;
			_pfd.cStencilBits = 8;

			int _fmt = ChoosePixelFormat((HDC)m_dc, &_pfd);
			int _result = SetPixelFormat((HDC)m_dc, _fmt, &_pfd);
			CHECK(_result, "SetPixelFormat Failed");
		}

		// create opengl context
		{
			m_rc = wglCreateContext((HDC)m_dc);
			CHECK(m_rc != nullptr, "wglCreateContext failed");
			wglMakeCurrent((HDC)m_dc, (HGLRC)m_rc);

			//TODO: use wglCreateContextAttribsARB 
		}

		// load opengl
		{
			wglSwapIntervalEXT = reinterpret_cast<decltype(wglSwapIntervalEXT)>(wglGetProcAddress("wglSwapIntervalEXT"));
		}

		SetVSync(true);

		m_opened = true;
		m_active = true;
	}
	//----------------------------------------------------------------------------//
	Engine::~Engine(void)
	{
		// TODO
	}
	//----------------------------------------------------------------------------//
	void Engine::RequireExit(bool _exit)
	{
		m_opened = !_exit;
	}
	//----------------------------------------------------------------------------//
	void Engine::BeginFrame(void)
	{
		// timer
		{
			if (m_prevTime == 0)
				m_prevTime = CurrentTime();
			double _currentTime = CurrentTime();
			m_unscaledFrameTime = (float)(_currentTime - m_prevTime);
			m_prevTime = _currentTime;
			m_frameTime = m_unscaledFrameTime * m_timeScale;
		}

		// events
		{
			m_userRequireExit = false;
			for (MSG _msg; PeekMessageA(&_msg, nullptr, 0, 0, PM_REMOVE);)
			{
				TranslateMessage(&_msg);
				DispatchMessageA(&_msg);
			}
		}


		glViewport(0, 0, m_size.x, m_size.y);
	}
	//----------------------------------------------------------------------------//
	void Engine::EndFrame(void)
	{
		SwapBuffers((HDC)m_dc);
		if (m_vsync)
			Sleep(1); // relax
	}
	//----------------------------------------------------------------------------//
	void Engine::SetVSync(bool _vsync)
	{
		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(_vsync ? 1 : 0);
		m_vsync = _vsync;
	}
	//----------------------------------------------------------------------------//
	double Engine::CurrentTime(void)
	{
		std::chrono::duration<double> _time;		_time = std::chrono::duration_cast<decltype(_time)>(std::chrono::high_resolution_clock::now().time_since_epoch());		return _time.count();
	}
	//----------------------------------------------------------------------------//
	void Engine::TimeScale(float _scale)
	{
		if (_scale < 0)
			_scale = 0;
		m_timeScale = _scale;
	}
	//----------------------------------------------------------------------------//
	void Engine::Clear(FrameBufferType::Enum _buffers, const Vector4& _color, float _depth, int _stencil)
	{
		uint _mask = 0;

		int _colorMask[4];
		if (_buffers & FrameBufferType::Color)
		{
			_mask |= GL_COLOR_BUFFER_BIT;
			glGetIntegerv(GL_COLOR_WRITEMASK, _colorMask);
			glColorMask(1, 1, 1, 1);
			glClearColor(_color.r, _color.g, _color.b, _color.a);
		}

		int _depthMask;
		if (_buffers & FrameBufferType::Depth)
		{
			_mask |= GL_DEPTH_BUFFER_BIT;
			glGetIntegerv(GL_DEPTH_WRITEMASK, &_depthMask);
			glClearDepth(_depth);
			glDepthMask(false);
		}

		int _stencilMask;
		if (_buffers & FrameBufferType::Stencil)
		{
			_mask |= GL_STENCIL_BUFFER_BIT;
			glGetIntegerv(GL_STENCIL_WRITEMASK, &_stencilMask);
			glStencilMask(_stencil);
		}

		int _viewport[4], _scissor[4];
		glGetIntegerv(GL_VIEWPORT, _viewport);
		glGetIntegerv(GL_SCISSOR_BOX, _scissor);

		int _scissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
		glEnable(GL_SCISSOR_TEST);
		glScissor(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);

		glClear(_mask);

		glScissor(_scissor[0], _scissor[1], _scissor[2], _scissor[3]);

		if (!_scissorEnabled)
			glDisable(GL_SCISSOR_TEST);

		if (_buffers & FrameBufferType::Color)
		{
			glColorMask(_colorMask[0], _colorMask[1], _colorMask[2], _colorMask[3]);
		}

		if (_buffers & FrameBufferType::Depth)
		{
			glDepthMask(_depthMask);
		}

		if (_buffers & FrameBufferType::Stencil)
		{
			glStencilMask(_stencilMask);
		}
	}
	//----------------------------------------------------------------------------//
	long __stdcall Engine::_WindowCallback(void* _wnd, uint _msg, uint _wParam, long _lParam)
	{
		if (s_instance && s_instance->m_window == _wnd)
			return s_instance->_HandleMessage(_msg, _wParam, _lParam);


		return DefWindowProcA((HWND)_wnd, _msg, _wParam, _lParam);
	}
	//----------------------------------------------------------------------------//
	long Engine::_HandleMessage(uint _msg, uint _wParam, long _lParam)
	{
		switch (_msg)
		{
		case WM_SETFOCUS:
			m_active = true;
			break;

		case WM_KILLFOCUS:
			m_active = false;
			break;

		case WM_CLOSE:
			m_userRequireExit = true;
			return 0;

		case WM_SIZE:
		{
			if (_wParam == SIZE_MAXIMIZED || _wParam == SIZE_RESTORED)
			{
				m_size.x = LOWORD(_lParam);
				m_size.y = HIWORD(_lParam);
			}
		}
		break;

		}

		return DefWindowProcA((HWND)m_window, _msg, _wParam, _lParam);
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}

