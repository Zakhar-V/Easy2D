#pragma once

#include "Base.hpp"

namespace Easy2D
{
	class Stream;

	//----------------------------------------------------------------------------//
	// Tokenizer
	//----------------------------------------------------------------------------//

	//!
	struct Tokenizer
	{
		//!
		struct Number
		{
			bool isFloat = false;
			union
			{
				int iValue = 0;
				float fValue;
			};
		};

		const char* s = nullptr;
		const char* e = nullptr;

		//!
		operator char(void) const { return *s; }
		//!
		char operator * (void) const { return *s; }
		//!
		char operator [] (int _index) const { return s[_index]; }
		//!
		const char* operator ++ (void) { Advance(); return s; }
		//!
		const char* operator ++ (int) { const char* p = s; Advance(); return p; }

		//!
		Tokenizer operator += (int _rhs) { Advance(_rhs); return *this; }

		//!
		void Advance(int _num = 1);
		//!
		int SkipWhiteSpace(void);
		//!
		int SkipComments(void);
		//!
		void NextToken(void);

		//!
		bool IsNumber(void) const;
		//!
		bool ParseNumber(Number& _val);

		//!
		bool IsString(void) const;
		//!
		bool ParseString(String& _val);

		//!
		bool EoF(void) const { return !*s; }

		//!
		bool Cmp(const char* _rhs, int _num) const { return strncmp(s, _rhs, _num) == 0; }
		//!
		bool Cmpi(const char* _rhs, int _num) const { return strnicmp(s, _rhs, _num) == 0; }
		//!
		bool AnyOf(const char* _cset) const { return *s && strchr(_cset, *s); }

		//!
		bool RaiseError(const char* _error);
		//!
		static void GetErrorPos(const char* _start, const char* _pos, int& _line, int& _column);
	};

	//----------------------------------------------------------------------------//
	// Json
	//----------------------------------------------------------------------------//

	//!
	class Json
	{
	public:
		//!
		enum class Type
		{
			Null,
			Bool,
			Int,
			Float,
			String,
			Array,
			Object,
		};

		typedef Pair<String, Json> KeyValue;
		typedef Array<KeyValue> Node;
		typedef Node::iterator Iterator;
		typedef Node::const_iterator ConstIterator;

		//!
		Json(void) = default;
		//!
		~Json(void) { SetType(Type::Null); }
		//!
		Json(const Json& _other);
		//!
		Json(Json&& _temp);
		//!
		Json(Type _type) { SetType(_type); }
		//!
		Json(std::nullptr_t) { }
		//!
		Json(bool _value);
		//!
		Json(int _value);
		//!
		Json(uint _value);
		//!
		Json(float _value);
		//!
		Json(const char* _value);
		//!
		Json(const String& _value);
		//!
		Json(String&& _value);

		//!
		Json& operator = (const Json& _rhs);
		//!
		Json& operator = (Json&& _rhs);

		//!
		bool IsNull(void) const { return m_type == Type::Null; }
		//!
		bool IsBool(void) const { return m_type == Type::Bool; }
		//!
		bool IsInt(void) const { return m_type == Type::Int; }
		//!
		bool IsFloat(void) const { return m_type == Type::Int; }
		//!
		bool IsNumeric(void) const { return m_type == Type::Int || m_type == Type::Float; }
		//!
		bool IsString(void) const { return m_type == Type::String; }
		//!
		bool IsArray(void) const { return m_type == Type::Array; }
		//!
		bool IsObject(void) const { return m_type == Type::Object; }
		//!
		bool IsNode(void) const { return m_type == Type::Array || m_type == Type::Object; }

		//!
		Json& SetType(Type _type);
		//!
		Json& SetNull(void) { return SetType(Type::Null); }
		//!
		Json& SetBool(bool _value) { SetType(Type::Bool).m_bool = _value; return *this; }
		//!
		Json& SetInt(int _value) { SetType(Type::Int).m_int = _value; return *this; }
		//!
		Json& SetFloat(float _value) { SetType(Type::Float).m_flt = _value; return *this; }
		//!
		Json& SetString(const String& _value) { SetType(Type::String)._String() = _value; return *this; }
		//!
		Json& SetString(String&& _value) { SetType(Type::String)._String() = std::move(_value); return *this; }
		//!
		Json& SetArray(InitializerList<Json> _value);
		//!
		Json& SetObject(InitializerList<KeyValue> _value);

		//!
		bool AsBool(void) const;
		//!
		int AsInt(void) const;
		//!
		float AsFloat(void) const;
		//!
		String AsString(void) const;

		//!
		operator bool(void) const { return AsBool(); }
		//!
		operator int(void) const { return AsInt(); }
		//!
		operator uint(void) const { return AsInt(); }
		//!
		operator float(void) const { return AsFloat(); }
		//!
		operator String(void) const { return AsString(); }

		// [ARRAY OR OBJECT]

		//!
		uint Size(void) const;
		//!
		Json& Clear(void);

		// [ARRAY ONLY]

		//!	
		Json& Resize(uint _size);

		//!
		Json& operator [] (uint _index) { return Get(_index); }
		//!
		const Json& operator [] (uint _index) const { return Get(_index); }
		//
		Json& operator [] (int _index) { return Get(_index); }
		//!
		const Json& operator [] (int _index) const { return Get(_index); }
		//!
		Json& Get(uint _index);
		//!
		const Json& Get(uint _index) const;

		//! Insert element to array. \return this
		Json& Insert(uint _pos, const Json& _value);
		//! Insert element to array. \return this
		Json& Insert(uint _pos, Json&& _value);
		//! Add new item to end of array. \return this
		Json& Push(const Json& _value);
		//! Add new item to end of array. \return this
		Json& Push(Json&& _value);
		//! Add new item to end of array and return it. \return new item
		Json& Append(void);

		//!	Remove last element of array. \return this
		Json& Pop(void);
		//! Remove range of elements from array. \return this
		Json& Erase(uint _start, uint _num);

		// [OBJECT ONLY]

		//!
		Json& operator [] (const String& _key) { return GetOrAdd(_key); }
		//!
		const Json& operator [] (const String& _key) const { return Get(_key); }
		//!
		Json& operator [] (const char* _key) { return GetOrAdd(_key); }
		//!
		const Json& operator [] (const char*_key) const { return Get(_key); }

		//! Get or add value of key.
		Json& GetOrAdd(const String& _key);
		//! Get value of key
		const Json& Get(const String& _key) const;

		//!Find value of key
		Json* Find(const String& _key);
		//!Find value of key
		const Json* Find(const String& _key) const;

		//!	Add key with value to object. \return this
		Json& Set(const String& _key, const Json& _value);
		//! Remove key from object
		bool Erase(const String& _key);

		//!
		Node& Container(void);
		//!
		const Node& Container(void) const;
		//!
		Iterator Begin(void) { return Container().begin(); }
		//!
		ConstIterator Begin(void) const { return Container().begin(); }
		//!
		Iterator End(void) { return Container().end(); }
		//!
		ConstIterator End(void) const { return Container().end(); }

		//!
		bool Parse(const char* _str, String* _error = nullptr);
		//!
		String Print(void) const;

		//!
		bool Load(Stream* _src);
		//!
		void Save(Stream* _dst);

		//!
		static const Json Null;
		//!
		static const Json EmptyArray;
		//!
		static const Json EmptyObject;

	protected:
		//!
		bool _Parse(Tokenizer& _str);
		//!
		void _Print(String& _dst, int _depth) const;
		//!
		static void _PrintString(String& _dst, const String& _src, int _depth);

		//!
		bool& _Bool(void) { return m_bool; }
		//!
		const bool& _Bool(void) const { return m_bool; }
		//!
		int& _Int(void) { return m_int; }
		//!
		const int& _Int(void) const { return m_int; }
		//!
		float& _Float(void) { return m_flt; }
		//!
		const float& _Float(void) const { return m_flt; }
		//!
		String& _String(void) { return *reinterpret_cast<String*>(m_str); }
		//!
		const String& _String(void) const { return *reinterpret_cast<const String*>(m_str); }
		//!
		Node& _Node(void) { return *reinterpret_cast<Node*>(m_node); }
		//!
		const Node& _Node(void) const { return *reinterpret_cast<const Node*>(m_node); }

		Type m_type = Type::Null;
		union
		{
			bool m_bool;
			int m_int = 0;
			float m_flt;
			uint8 m_str[sizeof(String)];
			uint8 m_node[sizeof(Node)];
		};
	};

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}
