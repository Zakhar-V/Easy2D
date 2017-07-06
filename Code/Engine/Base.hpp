#pragma once

//----------------------------------------------------------------------------//
// Includes
//----------------------------------------------------------------------------//

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

#include <stdint.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>

//----------------------------------------------------------------------------//
// Debug
//----------------------------------------------------------------------------//

#ifdef _DEBUG
#include <cassert>
#define ASSERT(...) assert(##__VA_ARGS__)
#else
#define ASSERT(...)
#endif

#define CHECK(...) ASSERT(##__VA_ARGS__)

#define LOG(msg, ...) {printf(msg, ##__VA_ARGS__); printf("\n");}

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// Typedefs
	//----------------------------------------------------------------------------//

	typedef unsigned int uint;
	typedef int8_t int8;
	typedef uint8_t uint8;
	typedef int16_t int16;
	typedef uint16_t uint16;
	typedef int32_t int32;
	typedef uint32_t uint32;
	typedef int64_t int64;
	typedef uint64_t uint64;

	typedef std::string String;
	template <class T> using Array = std::vector<T>;
	template <class T> using List = std::list<T>;
	template <class T, class U> using HashMap = std::unordered_map<T, U>;
	template <class T, class U> using Pair = std::pair<T, U>;
	template <class T> using InitializerList = std::initializer_list<T>;

	//----------------------------------------------------------------------------//
	// Linked list
	//----------------------------------------------------------------------------//

#define LL_LINK(HEAD, NODE, PREV, NEXT) {\
	NODE->NEXT = HEAD; \
	if (HEAD) \
		HEAD->PREV = NODE; \
	HEAD = NODE; }

#define LL_UNLINK(HEAD, NODE, PREV, NEXT) {\
		if (NODE->PREV) \
			NODE->PREV->NEXT = NODE->NEXT; \
		else \
		{ \
			ASSERT(NODE == HEAD); \
			HEAD = NODE->NEXT; \
		} \
		if (NODE->NEXT) \
			NODE->NEXT->PREV = NODE->PREV; \
		NODE->PREV = nullptr; \
		NODE->NEXT = nullptr; }

	//----------------------------------------------------------------------------//
	// StringUtils
	//----------------------------------------------------------------------------//

	struct StringUtils
	{
		//!
		static constexpr bool IsAlpha(char _ch) { return (_ch >= 'A' && _ch <= 'Z') || (_ch >= 'a' && _ch <= 'z') || ((uint8)_ch >= 0xc0); }
		//!
		static constexpr char Lower(char _ch) { return IsAlpha(_ch) ? (_ch | 0x20) : _ch; }
		//!
		static constexpr char Upper(char _ch) { return IsAlpha(_ch) ? (_ch & ~0x20) : _ch; }

		//!\return BSD checksum
		static constexpr uint ConstHash(const char* _str, uint _hash = 0) { return *_str ? ConstHash(_str + 1, ((_hash >> 1) + ((_hash & 1) << 15) + Lower(*_str)) & 0xffff) : _hash; }
		//!
		static uint Hash(const char* _str, uint _hash = 0);

		//!
		static String Format(const char* _fmt, ...);
		//!
		static String FormatV(const char* _fmt, va_list _args);

		//!
		static int Cmpi(const char* _str1, const char* _str2);

		//!
		static const String EmptyString;
	};

	//----------------------------------------------------------------------------//
	// NonCopyable
	//----------------------------------------------------------------------------//

	class NonCopyable
	{
	public:
		NonCopyable(void) = default;
		~NonCopyable(void) = default;

	private:
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator = (const NonCopyable&) = delete;
	};

	//----------------------------------------------------------------------------//
	// Singleton
	//----------------------------------------------------------------------------//

	//!
	template <class T> class Singleton
	{
	public:
		//!
		Singleton(void)
		{
			ASSERT(s_instance == nullptr);
			s_instance = static_cast<T*>(this);
		}
		//!
		~Singleton(void)
		{
			s_instance = nullptr;
		}

		//!
		static T* Get(void)
		{
			return s_instance;
		}

		static T* const& Instance;

	protected:
		static T* s_instance;
	};

	template <class T> T* Singleton<T>::s_instance = nullptr;
	template <class T> T* const& Singleton<T>::Instance = Singleton<T>::s_instance;

	//----------------------------------------------------------------------------//
	// 
	//----------------------------------------------------------------------------//
}

