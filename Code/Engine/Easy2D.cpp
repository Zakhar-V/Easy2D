
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
	String StringUtils::Format(const char* _fmt, ...)
	{
		va_list _args;
		va_start(_args, _fmt);
		String _str = FormatV(_fmt, _args);
		va_end(_args);
		return _str;
	}
	//----------------------------------------------------------------------------//
	String StringUtils::FormatV(const char* _fmt, va_list _args)
	{
		// TODO: use another implementation
		char _buffer[4096];
		vsnprintf(_buffer, sizeof(_buffer), _fmt, _args);
		return _buffer;
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	// Tokenizer
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	void Tokenizer::Advance(int _num)
	{
		while (_num-- && *s)
			++s;
	}
	//----------------------------------------------------------------------------//
	int Tokenizer::SkipWhiteSpace(void)
	{
		const char* _start = s;
		while (AnyOf(" \t\n\r"))
			Advance();
		return s - _start;
	}
	//----------------------------------------------------------------------------//
	int Tokenizer::SkipComments(void)
	{
		const char* _start = s;
		if (s[0] == '/' && (s[1] == '/' || s[1] == '*')) // comment
		{
			if (s[1] == '/')
			{
				Advance(2);
				while (!EoF() && !AnyOf("\n\r"))
					Advance();
			}
			else
			{
				Advance(2);
				while (!EoF())
				{
					if (Cmp("*/", 2))
					{
						Advance(2);
						break;
					}
					if (EoF())
						e = "EoF in multiline comment";
					Advance();
				}
			}
		}
		return s - _start;
	}
	//----------------------------------------------------------------------------//
	void Tokenizer::NextToken(void)
	{
		while ((SkipWhiteSpace() || SkipComments()) && !e);
	}
	//----------------------------------------------------------------------------//
	bool Tokenizer::IsNumber(void) const
	{
		return AnyOf("0123456789+-.");
	}
	//----------------------------------------------------------------------------//
	bool Tokenizer::ParseNumber(Number& _val)
	{
		if (!IsNumber())
			return RaiseError("Expected numeric constant not found");

		const char* _start = s;
		if (s[0] == '-')
			Advance();

		int _readed = 0, _result = 0;
		while (AnyOf("0123456789"))
			Advance(), _readed++;

		if (s[0] == '.')
		{
			_val.isFloat = true;

			Advance();
			_readed = 0;
			while (AnyOf("0123456789"))
				Advance(), _readed++;

			if (!_readed)
				return RaiseError("Wrong numeric constant");

			if (AnyOf("eE"))
			{
				Advance();
				if (!AnyOf("+-"))
					return RaiseError("Wrong numeric constant");
				Advance();

				_readed = 0;
				while (AnyOf("0123456789"))
					Advance(), _readed++;

				if (!_readed)
					return RaiseError("Wrong numeric constant");

				_result = sscanf(_start, "%e", &_val.fValue);
			}
			else
			{
				_result = sscanf(_start, "%f", &_val.fValue);
			}
		}
		else if (!_readed)
		{
			return RaiseError("Wrong numeric constant");
		}
		else
		{
			_val.isFloat = false;
			_result = sscanf(_start, "%d", &_val.iValue);
		}

		if (!_result)
			return RaiseError("Wrong numeric constant");

		return true;
	}
	//----------------------------------------------------------------------------//
	bool Tokenizer::IsString(void) const
	{
		return *s == '"';
	}
	//----------------------------------------------------------------------------//
	bool Tokenizer::ParseString(String& _val)
	{
		if (!IsString())
			return RaiseError("Expected string constant not found");

		Advance();
		for (;;)
		{
			if (s[0] == '\\')
			{
				switch (s[1])
				{
				case '\\':
					Advance(2);
					_val += "\\";
					break;
				case '/':
					Advance(2);
					_val += "/";
					break;
				case 'b':
					Advance(2);
					_val += "\b";
					break;
				case 'f':
					Advance(2);
					_val += "\f";
					break;
				case 'n':
					Advance(2);
					_val += "\n";
					break;
				case 'r':
					Advance(2);
					_val += "\r";
					break;
				case 't':
					Advance(2);
					_val += "\t";
					break;
				case 'u':
				{
					Advance(2);
					char _buff[5];
					for (uint i = 0; i < 4; ++i)
					{
						if (!AnyOf("0123456789abcdefABCDEF"))
							return RaiseError("Expected numeric literal");
						_buff[i] = *s;
						Advance();
					}
					_buff[4] = 0;
					uint16 _char = 0;
					sscanf(_buff, "%hx", &_char);

					if (_char > 0xff)
						return RaiseError("Unicode character not supported"); // TODO:

					_val += (char)_char;

				} break;

				default:
					return RaiseError("Unknown escape sequence");
				}
			}
			else if (*s == '"')
			{
				Advance();
				break;
			}
			else if (AnyOf("\n\r"))
			{
				return RaiseError("New line in string constant");
			}
			else
			{
				_val.append(s, 1);
				Advance();
			}
		}

		return true;
	}
	//----------------------------------------------------------------------------//
	bool Tokenizer::RaiseError(const char* _error)
	{
		e = _error;
		return false;
	}
	//----------------------------------------------------------------------------//
	void Tokenizer::GetErrorPos(const char* _start, const char* _pos, int& _line, int& _column)
	{
		// Windows \r\n
		// Linux \n
		// Macintosh(Mac OSX) \n
		// Macintosh(old) \r

		_line = 1;
		_column = 1;
		while (_start < _pos)
		{
			if (_start[0] == '\r')
			{
				if (_start[1] == '\n')
					++_start;
				++_line;
				_column = 0;
			}
			else if (_start[0] == '\n')
			{
				++_line;
				_column = 0;
			}
			++_start;
			++_column;
		}
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
	// Json
	//----------------------------------------------------------------------------//
	
	const Json Json::Null;
	const Json Json::EmptyArray(Json::Type::Array);
	const Json Json::EmptyObject(Json::Type::Object);

	//----------------------------------------------------------------------------//
	Json::Json(const Json& _other)
	{
		SetType(_other.m_type);
		if (IsString())
			_String() = _other._String();
		else if (IsNode())
			_Node() = _other._Node();
		else
			m_int = _other.m_int;
	}
	//----------------------------------------------------------------------------//
	Json::Json(Json&& _temp)
	{
		SetType(_temp.m_type);
		if (IsString())
			_String() = std::move(_temp._String());
		else if (IsNode())
			_Node() = std::move(_temp._Node());
		else
			m_int = _temp.m_int;
	}
	//----------------------------------------------------------------------------//
	Json::Json(bool _value)
	{
		SetType(Type::Bool);
		_Bool() = _value;
	}
	//----------------------------------------------------------------------------//
	Json::Json(int _value)
	{
		SetType(Type::Int);
		_Int() = _value;
	}
	//----------------------------------------------------------------------------//
	Json::Json(uint _value)
	{
		SetType(Type::Int);
		_Int() = _value;
	}
	//----------------------------------------------------------------------------//
	Json::Json(float _value)
	{
		SetType(Type::Float);
		_Float() = _value;
	}
	//----------------------------------------------------------------------------//
	Json::Json(const char* _value)
	{
		SetType(Type::String);
		_String() = _value;
	}
	//----------------------------------------------------------------------------//
	Json::Json(const String& _value)
	{
		SetType(Type::String);
		_String() = _value;
	}
	//----------------------------------------------------------------------------//
	Json::Json(String&& _value)
	{
		SetType(Type::String);
		_String() = std::move(_value);
	}
	//----------------------------------------------------------------------------//
	Json& Json::operator = (const Json& _rhs)
	{
		SetType(_rhs.m_type);
		if (IsString())
			_String() = _rhs._String();
		else if (IsNode())
			_Node() = _rhs._Node();
		else
			m_int = _rhs.m_int;

		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::operator = (Json&& _rhs)
	{
		SetType(_rhs.m_type);
		if (IsString())
			_String() = std::move(_rhs._String());
		else if (IsNode())
			_Node() = std::move(_rhs._Node());
		else
			m_int = _rhs.m_int;
		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::SetType(Type _type)
	{
		if (m_type != _type)
		{
			switch (m_type)
			{
			case Type::String:
				_String().~String();
				break;
			case Type::Array:
			case Type::Object:
				_Node().~Node();
				break;
			};

			m_type = _type;

			switch (m_type)
			{
			case Type::String:
				new(&_String()) String();
				break;
			case Type::Array:
			case Type::Object:
				new(&_Node()) Node();
				break;
			default:
				m_int = 0;
				break;
			};
		}
		/*else if (IsNode() && (_type == Type::Array || _type == Type::Object))
		{
			m_type = _type;
		}*/

		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::SetArray(InitializerList<Json> _value)
	{
		Clear();
		for (auto i : _value)
			Push(i);
		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::SetObject(InitializerList<KeyValue> _value)
	{
		Clear();
		for (auto i : _value)
			Set(i.first, i.second);
		return *this;
	}
	//----------------------------------------------------------------------------//
	bool Json::AsBool(void) const
	{
		switch (m_type)
		{
		case Type::Null:
		case Type::Bool:
		case Type::Int:
			return m_int != 0;
		case Type::Float:
			return m_flt != 0;
		}
		return false;
	}
	//----------------------------------------------------------------------------//
	int Json::AsInt(void) const
	{
		switch (m_type)
		{
		case Type::Null:
		case Type::Bool:
		case Type::Int:
			return m_int;
		case Type::Float:
			return (int)m_flt;
		}
		return 0;
	}
	//----------------------------------------------------------------------------//
	float Json::AsFloat(void) const
	{
		switch (m_type)
		{
		case Type::Null:
		case Type::Bool:
		case Type::Int:
			return (float)m_int;
		case Type::Float:
			return m_flt;
		}
		return 0;
	}
	//----------------------------------------------------------------------------//
	String Json::AsString(void) const
	{
		switch (m_type)
		{
		case Type::Null:
			return "null";
		case Type::Bool:
			return m_bool ? "true" : "false";
		case Type::Int:
			return StringUtils::Format("%d", m_int);
		case Type::Float:
			return StringUtils::Format("%f", m_flt);
		case Type::String:
			return _String();
		}
		return "";
	}
	//----------------------------------------------------------------------------//
	uint Json::Size(void) const
	{
		switch (m_type)
		{
		case Type::Array:
		case Type::Object:
			return (uint)_Node().size();
		}
		return 0;
	}
	//----------------------------------------------------------------------------//
	Json& Json::Clear(void)
	{
		if (IsNode())
			_Node().clear();
		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::Resize(uint _size)
	{
		SetType(Type::Array);
		_Node().resize(_size);
		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::Get(uint _index)
	{
		SetType(Type::Array);
		return _Node()[_index].second;
	}
	//----------------------------------------------------------------------------//
	const Json& Json::Get(uint _index) const
	{
		return IsArray() ? _Node()[_index].second : Null;
	}
	//----------------------------------------------------------------------------//
	Json& Json::Insert(uint _pos, const Json& _value)
	{
		SetType(Type::Array);
		_Node().insert(_Node().begin() + _pos, { "", _value });
		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::Insert(uint _pos, Json&& _value)
	{
		SetType(Type::Array);
		_Node().insert(_Node().begin() + _pos, { "", std::move(_value) });
		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::Push(const Json& _value)
	{
		SetType(Type::Array);
		_Node().push_back({ "", _value });
		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::Push(Json&& _value)
	{
		SetType(Type::Array);
		_Node().push_back({ "", std::move(_value) });
		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::Append(void)
	{
		SetType(Type::Array);
		_Node().push_back({ "", Null });
		return _Node().back().second;
	}
	//----------------------------------------------------------------------------//
	Json& Json::Pop(void)
	{
		if (IsArray())
			_Node().pop_back();
		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::Erase(uint _start, uint _num)
	{
		if (IsArray())
			_Node().erase(_Node().begin() + _start, _Node().begin() + _start + _num);
		return *this;
	}
	//----------------------------------------------------------------------------//
	Json& Json::GetOrAdd(const String& _key)
	{
		SetType(Type::Object);

		Json* _value = Find(_key);
		if (_value)
			return *_value;

		_Node().push_back({ _key, Null });
		return _Node().back().second;
	}
	//----------------------------------------------------------------------------//
	const Json& Json::Get(const String& _key) const
	{
		const Json* _value = Find(_key);
		return _value ? *_value : Null;
	}
	//----------------------------------------------------------------------------//
	Json* Json::Find(const String& _key)
	{
		if (IsObject())
		{
			for (auto& i : _Node())
			{
				if (i.first == _key)
					return &i.second;
			}
		}
		return nullptr;
	}
	//----------------------------------------------------------------------------//
	const Json* Json::Find(const String& _key) const
	{
		if (IsObject())
		{
			for (auto& i : _Node())
			{
				if (i.first == _key)
					return &i.second;
			}
		}
		return nullptr;
	}
	//----------------------------------------------------------------------------//
	Json& Json::Set(const String& _key, const Json& _value)
	{
		GetOrAdd(_key) = _value;
		return *this;
	}
	//----------------------------------------------------------------------------//
	bool Json::Erase(const String& _key)
	{
		if (IsObject())
		{
			for (auto i = _Node().begin(); i != _Node().end(); ++i)
			{
				if (i->first == _key)
				{
					_Node().erase(i);
					return true;
				}
			}
		}
		return false;
	}
	//----------------------------------------------------------------------------//
	Json::Node& Json::Container(void)
	{
		SetType(Type::Object);
		return _Node();
	}
	//----------------------------------------------------------------------------//
	const Json::Node& Json::Container(void) const
	{
		return IsObject() ? _Node() : EmptyObject._Node();
	}
	//----------------------------------------------------------------------------//
	bool Json::Parse(const char* _str, String* _error)
	{
		Tokenizer _stream;
		_stream.s = _str;

		if (!_Parse(_stream))
		{
			if (_error)
			{
				int _l, _c;
				Tokenizer::GetErrorPos(_str, _stream.s, _l, _c);
				*_error = StringUtils::Format("(%d:%d) : JSON error : %s", _l, _c, _stream.e);
			}

			return false;
		}

		return true;
	}
	//----------------------------------------------------------------------------//
	String Json::Print(void) const
	{
		String _str;
		_Print(_str, 0);
		return _str;
	}
	//----------------------------------------------------------------------------//
	bool Json::_Parse(Tokenizer& _str)
	{
		//http://www.json.org/json-ru.html

		_str.NextToken();

		if (_str.e)
			return false;

		if (_str.EoF()) // eof
		{
			SetNull();
		}
		else if (_str.IsNumber()) // int or float
		{
			Tokenizer::Number _val;
			if (!_str.ParseNumber(_val))
				return false;

			if (_val.isFloat)
				SetFloat(_val.fValue);
			else
				SetInt(_val.iValue);
		}
		else if (_str.IsString()) // string
		{
			SetType(Type::String);
			if (!_str.ParseString(_String()))
				return false;
		}
		else if (_str.Cmpi("true", 4)) // bool
		{
			_str += 4;
			SetBool(true);
		}
		else if (_str.Cmpi("false", 5))	// bool
		{
			_str += 5;
			SetBool(false);
		}
		else if (_str.Cmpi("null", 4)) // null
		{
			_str += 4;
			SetNull();
		}
		else if (_str[0] == '[') // array
		{
			++_str;
			SetType(Type::Array);
			for (;;)
			{
				if (_str[0] == ']')
				{
					++_str;
					break;
				}

				if (_str.EoF())
					return _str.RaiseError("Unexpectd EoF");

				if (!Append()._Parse(_str))
					return false;

				_str.NextToken();
				if (_str[0] == ',') // divisor (not necessarily) 
					++_str;
			}
		}
		else if (_str[0] == '{') // object
		{
			++_str;
			SetType(Type::Object);
			for (;;)
			{
				_str.NextToken();

				if (_str[0] == '}')
				{
					++_str;
					break;
				}

				if (_str.EoF())
					return _str.RaiseError("Unexpectd EoF");

				_Node().push_back({"", Null});
				KeyValue& _pair = _Node().back();

				if (!_str.ParseString(_pair.first))
					return false;

				_str.NextToken();
				if (_str[0] == ':')
					++_str;
				else
					return _str.RaiseError("Expected ':' not found");

				_str.NextToken();
				if (!_pair.second._Parse(_str))
					return false;

				_str.NextToken();
				if (_str[0] == ',') // divisor (not necessarily) 
					++_str;
			}
		}
		else
		{
			return _str.RaiseError("Unknown symbol");
		}

		return true;
	}
	//----------------------------------------------------------------------------//
	void Json::_Print(String& _dst, int _depth) const
	{

		switch (m_type)
		{
		case Type::Null:
			_dst += "null";
			break;
		case Type::Bool:
			_dst += _Bool() ? "true" : "false";
			break;
		case Type::Int:
			_dst += StringUtils::Format("%d", _Int());
			break;
		case Type::Float:
			_dst += StringUtils::Format("%f", _Float());
			break;
		case Type::String:
		{
			_PrintString(_dst, _String(), _depth);

		} break;
		case Type::Array:
		{
			bool _oneLine = true;
			if (_Node().size() < 5)
			{
				for (const auto& i : _Node())
				{
					if (i.second.IsNode())
					{
						_oneLine = false;
						break;
					}
				}
			}
			else
				_oneLine = false;

			_dst += "[";

			for (const auto& i : _Node())
			{
				if (!_oneLine)
				{
					_dst += '\n';
					for (int i = 0; i <= _depth; ++i)
						_dst += "\t";
				}
				i.second._Print(_dst, _depth + 1);

				if (&i != &_Node().back())
				{
					_dst += ",";
					if (_oneLine)
						_dst += " ";
				}
			}

			if (!_oneLine)
			{
				_dst += "\n";
				for (int i = 0; i < _depth; ++i)
					_dst += "\t";
			}
			_dst += "]";

		} break;
		case Type::Object:
		{
			_dst += "{\n";

			for (const auto& i : _Node())
			{
				for (int i = 0; i <= _depth; ++i)
					_dst += "\t";

				_PrintString(_dst, i.first, _depth + 1);

				_dst += " : ";
				i.second._Print(_dst, _depth + 1);

				if (&i != &_Node().back())
					_dst += ",\n";
			}

			_dst += "\n";
			for (int i = 0; i < _depth; ++i)
				_dst += "\t";
			_dst += "}";

		} break;
		}
	}
	//----------------------------------------------------------------------------//
	void Json::_PrintString(String& _dst, const String& _src, int _depth)
	{
		_dst += "\"";
		for (char s : _src)
		{
			if (s == '\n')
				_dst += "\\n";
			else if (s == '\r')
				_dst += "\\r";
			else if (s == '\\')
				_dst += "\\\\";
			else
				_dst += s; // TODO:
		}
		_dst += "\"";
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

		m_batch = new Vertex[m_batchMaxSize];
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

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), reinterpret_cast<uint8*>(m_batch) + offsetof(Vertex, color));

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), reinterpret_cast<uint8*>(m_batch) + offsetof(Vertex, tc));

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), reinterpret_cast<uint8*>(m_batch) + offsetof(Vertex, pos));
	}
	//----------------------------------------------------------------------------//
	void Engine::EndFrame(void)
	{
		Flush();

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
		std::chrono::duration<double> _time;
		_time = std::chrono::duration_cast<decltype(_time)>(std::chrono::high_resolution_clock::now().time_since_epoch());
		return _time.count();
	}
	//----------------------------------------------------------------------------//
	void Engine::TimeScale(float _scale)
	{
		if (_scale < 0)
			_scale = 0;
		m_timeScale = _scale;
	}
	//----------------------------------------------------------------------------//
	void Engine::Begin2D(const Vector2& _cameraPos, float _zoom)
	{
		Flush();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, m_size.x, m_size.y, 0, 0, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-_cameraPos.x, -_cameraPos.y, 0);
		glScalef(_zoom, _zoom, 1);
	}
	//----------------------------------------------------------------------------//
	void Engine::Flush(void)
	{
		static const uint GLPrimitveType[] = 
		{
			GL_POINTS, // Points
			GL_LINES, // Lines
			GL_TRIANGLES, // Triangles
			GL_QUADS, // Quads
		};

		glDrawArrays(GLPrimitveType[m_batchType], 0, m_batchSize);
		m_batchSize = 0;
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
	void Engine::Draw(PrimitiveType::Enum _type, const Vertex* _vertices, uint _count, Texture* _texture, uint _mode)
	{
		Vertex* _batch = AddBatch(_type, _count, _texture, _mode);
		memcpy(_batch, _vertices, _count * sizeof(Vertex));
	}
	//----------------------------------------------------------------------------//
	Vertex* Engine::AddBatch(PrimitiveType::Enum _type, uint _count, Texture* _texture, uint _mode)
	{
		if (m_batchType != _type)
		{
			Flush();
			m_batchType = _type;
		}

		if (m_texture != _texture)
		{
			Flush();
			m_texture = _texture;
			//TODO: bind texture
		}

		if (m_batchMode != _mode)
		{
			Flush();
			m_batchMode = _mode;
			//TODO: apply changes
		}

		if (m_batchSize + _count >= m_batchMaxSize)
			Flush();

		Vertex* _batch = m_batch + m_batchSize;
		m_batchSize += _count;
		return _batch;
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

