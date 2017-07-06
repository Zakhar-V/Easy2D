#include "Json.hpp"
#include "File.hpp"

namespace Easy2D
{
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
	bool Json::Load(Stream* _src)
	{
		ASSERT(_src != nullptr);

		Array<char> _data;
		_data.resize(_src->Size());
		_src->Read(_data.data(), (uint)_data.size());

		String _err;
		if (!Parse(_data.data(), &_err))
		{
			LOG("%s%s", _src->Name().c_str(), _err.c_str());
			return false;
		}
		return true;
	}
	//----------------------------------------------------------------------------//
	void Json::Save(Stream* _dst)
	{
		ASSERT(_dst != nullptr);

		String _str = Print();
		_dst->Write(_str.c_str(), (uint)_str.length());
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

				_Node().push_back({ "", Null });
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
	//
	//----------------------------------------------------------------------------//
}