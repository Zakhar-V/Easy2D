#include "Base.hpp"

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// StringUtils
	//----------------------------------------------------------------------------//

	const String StringUtils::EmptyString;

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
	int StringUtils::Cmpi(const char* _str1, const char* _str2)
	{
		return stricmp(_str1, _str2);
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//

}
