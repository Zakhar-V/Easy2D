#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

#include <stdint.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>

#define CHECK(cond, ...)

#ifdef _DEBUG
#include <cassert>
#define ASSERT(...) assert(##__VA_ARGS__)
#else
#define ASSERT(...)
#endif

#define LOG(msg, ...) {printf(msg, ##__VA_ARGS__); printf("\n");}

namespace Easy2D
{
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
	// 
	//----------------------------------------------------------------------------//

	template <class T> T Min(T _a, T _b)
	{
		return _a < _b ? _a : _b;
	}
	template <class T> T Max(T _a, T _b)
	{
		return _a > _b ? _a : _b;
	}
	template <class T> T Clamp(T _value, T _min, T _max)
	{
		return Max(_min, Min(_value, _max));
	}

	struct IntVector2
	{
		union
		{
			struct
			{
				int x, y;
			};
			int v[2];
		};
	};


	struct Vector2
	{
		union
		{
			struct
			{
				float x, y;
			};
			float v[2];
		};
	};

	struct Vector3
	{
		union
		{
			struct
			{
				float x, y, z;
			};
			float v[3];
		};
	};

	struct Vector4
	{
		union
		{
			struct
			{
				float x, y, z, w;
			};
			struct
			{
				float r, g, b, a;
			};
			float v[4];
		};
	};

	struct Quaternion
	{
		union
		{
			struct
			{
				float x, y, z, w;
			};
			float v[4];
		};
	};

	struct Matrix34
	{
		union
		{
			struct
			{
				float m00, m01, m02, m03;
				float m10, m11, m12, m13;
				float m20, m21, m22, m23;
			};
			float m[3][4];
			float v[12];
		};
	};

	struct Matrix44
	{
		//Matrix44& CreateOrtho2D(float _width, float _height);

		//const Matrix44 Zero;
		//const Matrix44 Identity;

		union
		{
			struct
			{
				float m00, m01, m02, m03;
				float m10, m11, m12, m13;
				float m20, m21, m22, m23;
				float m30, m31, m32, m33;
			};
			float m[4][4];
			float v[16];
		};
	};

	struct Rect
	{
		Vector2 lower = { 0, 0 };
		Vector2 upper = { 0, 0 };
	};

	struct Color4ub
	{
		union
		{
			struct
			{
				uint8 r, g, b, a;
			};
			uint8 v[4];
			uint8 rgba;
		};
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
	// RefCounted
	//----------------------------------------------------------------------------//

	//!
	class RefCounted
	{
	public:
		//!
		struct Counter
		{
			RefCounted* object;
			int ref = 0;
			int weak = 1;

			//! Increments the counter of weak references
			void AddRef(void)
			{
				++weak;
			}
			//! Decrements the counter of weak references
			void Release(void)
			{
				if (!--weak && !ref)
					delete this;
			}
		};

		//!
		RefCounted(void);
		//!
		virtual ~RefCounted(void);

		//! Increments the counter of strong references
		void AddRef(void);
		//! Decrements the counter of strong references
		void Release(void);

	protected:
		//! Delete this object. You can overload this function for another behavior on deletion.
		virtual void _DeleteThis(void);

		Counter* m_rc;
	};

	//!
	template <class T> static void AddRef(T* _ptr)
	{
		if (_ptr)
			_ptr->AddRef();
	}
	//!
	template <class T> static void Release(T* _ptr)
	{
		if (_ptr)
			_ptr->Release();
	}
	//!
	template <class T, class U> static void Assign(T*& _lhs, U* _rhs)
	{
		AddRef(_rhs);
		Release(_lhs);
		_lhs = static_cast<T*>(_rhs);
	}

	//----------------------------------------------------------------------------//
	// SharedPtr
	//----------------------------------------------------------------------------//

	template <class T> class SharedPtr
	{
	public:
		//!
		SharedPtr(void) = default;
		//!
		SharedPtr(const SharedPtr& _ptr) :
			SharedPtr(_ptr.m_ptr)
		{
		}
		//!
		SharedPtr(SharedPtr&& _ptr) :
			m_ptr(_ptr.m_ptr)
		{
			_ptr.m_ptr = nullptr;
		}
		//!
		SharedPtr(const T* _ptr) :
			m_ptr(const_cast<T*>(_ptr))
		{
			AddRef(m_ptr);
		}
		//!
		~SharedPtr(void)
		{
			Release(m_ptr);
		}

		//!
		SharedPtr& operator = (const SharedPtr& _rhs)
		{
			Assign(m_ptr, _rhs.m_ptr);
			return *this;
		}
		//!
		SharedPtr& operator = (SharedPtr&& _rhs)
		{
			std::swap(m_ptr, _rhs.m_ptr);
			return *this;
		}
		//!
		SharedPtr& operator = (const T* _rhs)
		{
			Assign(m_ptr, const_cast<T*>(_rhs));
			return *this;
		}

		//!	Implicit cast
		operator T* (void) const
		{
			return Get();
		}

		//!
		T& operator * (void) const
		{
			ASSERT(m_ptr != nullptr);
			return *Get();
		}
		//!
		T* operator -> (void) const
		{
			ASSERT(m_ptr != nullptr);
			return Get();
		}

		//!
		T* Get(void) const
		{
			return const_cast<T*>(m_ptr);
		}
		//!
		template <class X> X* Cast(void) const
		{
			return static_cast<X*>(const_cast<T*>(m_ptr));
		}

	private:
		T* m_ptr = nullptr;
	};

	//----------------------------------------------------------------------------//
	// Object
	//----------------------------------------------------------------------------//

	// !
#define RTTI(TYPE) \
	enum : uint { TypeID = StringUtils::ConstHash(TYPE) }; \
	uint GetTypeID(void) override { return TypeID; } \
	bool IsTypeOf(uint _type) override { return _type == TypeID || __super::IsTypeOf(_type); } \
	template <class T> bool IsTypeOf(void) { return IsTypeOf(T::TypeID); } \
	static constexpr const char* TypeName = TYPE; \
	const char* GetTypeName(void) override { return TypeName; }

	// !
	typedef SharedPtr<class Object> ObjectPtr;

	// !
	class Object : public RefCounted
	{
	public:
		// !
		enum : uint { TypeID = StringUtils::ConstHash("Object") };
		// !
		virtual uint GetTypeID(void) { return TypeID; }
		// !
		virtual bool IsTypeOf(uint _type) { return _type == TypeID; }
		// !
		template <class T> bool IsTypeOf(void) { return IsTypeOf(T::TypeID); }
		// !
		static constexpr const char* TypeName = "Object";
		// !
		virtual const char* GetTypeName(void) { return TypeName; }

		// !
		typedef SharedPtr<Object>(*FactoryPfn)(void);
		// !
		template <class T> static SharedPtr<Object> Factory() { return new T; }

		// !
		struct TypeInfo
		{
			uint type;
			const char* name;
			FactoryPfn Factory = nullptr;
			uint flags = 0; //!< type-specific flags

			TypeInfo* SetFactory(FactoryPfn _factory) { Factory = _factory; return this; }
			TypeInfo* SetFlags(uint _flags) { flags = _flags; return this; }
			TypeInfo* AddFlags(uint _flags) { flags |= _flags; return this; }
			bool HasAnyOfFlags(uint _flags) { return (flags & _flags) != 0; }
		};

		//!
		static TypeInfo* GetOrCreateTypeInfo(const char* _name);
		//!
		static TypeInfo* GetTypeInfo(uint _type);
		//!
		template <class T> static TypeInfo* GetOrCreateTypeInfo(void) { return GetOrCreateTypeInfo(T::TypeName); }
		//!
		template <class T> static SharedPtr<T> Create(void)
		{
			auto _iter = s_types.find(T::TypeID);
			if (_iter != s_types.end() && _iter->second.Factory)
				return _iter->second.Factory().Cast<T>();

			LOG("Error: Factory for %s not found", T::TypeName); // or better use of FATAL?
			return nullptr;
		}
		//!
		template <class T> static TypeInfo* Register(uint _flags = 0)
		{
			TypeInfo* _info = GetOrCreateTypeInfo<T>();
			_info->Factory = &Object::Factory<T>;
			_info->AddFlags(_flags);
			return _info;
		}

	private:
		static HashMap<uint, TypeInfo> s_types;
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
	// Stream
	//----------------------------------------------------------------------------//

	typedef SharedPtr<class Stream> StreamPtr;

	class Stream : public Object
	{
	public:
		RTTI("Stream");

		//!
		enum class SeekOrigin
		{
			Set = SEEK_SET,
			Current = SEEK_CUR,
			End = SEEK_END,
		};

		//!
		virtual const String& Name(void) = 0;

		//!
		virtual bool IsOpened(void) = 0;
		//!
		virtual void Close(void) = 0;

		//!
		virtual uint Size(void) = 0;
		//!
		virtual bool EoF(void) = 0;
		//!
		virtual void Seek(int _offset, SeekOrigin _origin = SeekOrigin::Current) = 0;
		//!
		virtual uint Tell(void) = 0;

		//!
		virtual bool IsReadOnly(void) = 0;
		//!
		virtual uint Read(void* _dst, uint _size) = 0;
		//!
		virtual uint Write(const void* _src, uint _size) = 0;
		//!
		virtual void Flush(void) = 0;

	protected:
	};

	//----------------------------------------------------------------------------//
	// FileStream
	//----------------------------------------------------------------------------//

	typedef SharedPtr<class FileStream> FileStreamPtr;

	class FileStream : public Stream
	{
	public:
		RTTI("DiskFile");

		enum class Mode
		{
			ReadOnly, //!< Open a existent file for reading only. Not create file if no exists.
			ReadWriteExistent, //!< Open a existent file for reading and writing. Not create file if no exists.
			ReadWrite, //!< Open existent or create new file for reading and writing. 
			Overwrite, //!< Create new file for reading and writing.
		};

		//!
		FileStream(void) = default;
		//!
		~FileStream(void);

		//!
		const String& Name(void) override { return m_name; }

		//!
		bool Open(const String& _name, Mode _mode);
		//!
		bool IsOpened(void) override { return m_handle != nullptr; }
		//!
		void Close(void) override;

		//!
		uint Size(void) override { return m_size; }
		//!
		bool EoF(void) override;
		//!
		void Seek(int _offset, SeekOrigin _origin = SeekOrigin::Current) override;
		//!
		uint Tell(void) override;

		//!
		bool IsReadOnly(void) override { return m_readOnly; }
		//!
		uint Read(void* _dst, uint _size) override;
		//!
		uint Write(const void* _src, uint _size) override;
		//!
		void Flush(void) override;

	protected:
		String m_name;
		bool m_readOnly = true;
		uint m_size = 0;
		FILE* m_handle = nullptr;
	};

	//----------------------------------------------------------------------------//
	// PathUtils
	//----------------------------------------------------------------------------//

	struct PathUtils
	{
		//!
		static bool IsFullPath(const char* _path);
		//!
		static bool IsDelimeter(char _ch);
		//!
		static String Extension(const char* _path);
		//!
		static String Extension(const String& _path) { return Extension(_path.c_str()); }
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
		Iterator End(void) { return Container().begin(); }
		//!
		ConstIterator End(void) const { return Container().begin(); }

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
	// Graphics Defs
	//----------------------------------------------------------------------------//

	struct FrameBufferType
	{
		enum Enum : uint
		{
			Color = 0x1,
			Depth = 0x2,
			Stencil = 0x4,
			DepthStencil = Depth | Stencil,
			All = Color | DepthStencil,
		};
	};

	struct PrimitiveType
	{
		enum Enum
		{
			Points,
			Lines,
			Triangles,
			Quads,
		};
	};

	struct Vertex
	{
		Vector3 tc;
		Color4ub color;
		Vector3 pos;
	};

	//----------------------------------------------------------------------------//
	// Resource
	//----------------------------------------------------------------------------//

	typedef SharedPtr<class Resource> ResourcePtr;

	//!
	class Resource : public Object
	{
	public:
		RTTI("Resource");

		//!
		virtual bool Load(Stream* _src);
		//!
		virtual bool Save(Stream* _dst);

		//!
		void SetName(const String& _name) { m_name = _name; }
		//!
		const String& GetName(void) { return m_name; }

	protected:
 		String m_name;
	};

	//----------------------------------------------------------------------------//
	// Image
	//----------------------------------------------------------------------------//

	//!
	typedef SharedPtr<class Image> ImagePtr;

	//!
	class Image : public Resource
	{
	public:
		RTTI("Image");

		//!
		Image(void) = default;
		//!
		~Image(void);

		//!
		bool Realloc(uint _width, uint _height, uint _depth, uint _channels);

		//!
		uint Width(void) { return m_size.x; }
		//!
		uint Height(void) { return m_size.y; }
		//!
		uint Depth(void) { return m_depth; }
		//!
		uint Channels(void) { return m_channels; }
		//!
		uint8* Layer(uint _index);

		//! \sa	Resource::Load
		//!	jpeg, png, bmp, hdr, psd, tga, gif
		bool Load(Stream* _src) override;
		//! \sa	Resource::Save
		bool Save(Stream* _dst) override;

	protected:
		IntVector2 m_size;
		uint m_depth = 1;
		uint m_channels = 4;
		uint8* m_pixels = nullptr;
		int m_compressedFormat = 0; // TODO:
	};

	//----------------------------------------------------------------------------//
	// PixelFormat
	//----------------------------------------------------------------------------//

	struct PixelFormat
	{
		enum Enum
		{
			R8,
			RG8,
			RGB8,
			RGBA8,

			// compressed formats
			DXT1,
			DXT5,
		};

		//!
		static bool IsCompressed(Enum _format);
		//!
		static Enum GetCompressedFormat(Enum _format);
		//!
		static Enum GetUncompressedFormat(Enum _format);
	};

	//----------------------------------------------------------------------------//
	// Texture
	//----------------------------------------------------------------------------//

	class Texture : public Resource
	{
	public:
		RTTI("Texture");

		enum class Type
		{
			Default, //!< 2D
			Volume,	//!< 3D
		};

		//!
		void Create(Type _type, PixelFormat::Enum _format);
		//!
		void Destroy(void);
		//!
		void Realloc(uint _width, uint _height, uint _depth);
		//!
		void Write(int _x, int _y, int _z, uint _w, uint _h, uint _d, PixelFormat::Enum _format, const void* _data);

		//! \sa	Resource::Load, Image::Load
		bool Load(Stream* _src) override;

		void _Bind(uint _slot);

	protected:
		Type m_type = Type::Default;
		PixelFormat::Enum m_format = PixelFormat::RGBA8;
		IntVector2 m_size = { 0, 0 };
		uint m_depth = 1;
		uint m_handle = 0;
	};

	//----------------------------------------------------------------------------//
	// Engine
	//----------------------------------------------------------------------------//

#define gEngine Engine::Instance

	//!
	class Engine : public Singleton<Engine>
	{
	public:
		//!
		Engine(void);
		//!
		~Engine(void);

		//
		const IntVector2& WindowSize(void) { return m_size; }

		// [LOOP]

		//!
		bool IsOpened(void) { return m_opened; }
		//!
		bool UserRequireExit(void) { return m_userRequireExit; }
		//!
		void RequireExit(bool _exit = true);

		//!
		void BeginFrame(void);
		//!
		void EndFrame(void);

		//!
		void SetVSync(bool _vsync);

		// [TIME]

		//!	\return current time in seconds
		double CurrentTime(void);
		//! \return scaled frame time in seconds
		float FrameTime(void) { return m_frameTime; }
		//!	\return unscaled frame time in seconds
		float UnscaledFrameTime(void){ return m_frameTime; }
		//!
		void TimeScale(float _scale);
		//!
		float TimeScale(void) { return m_timeScale; }

		// [FILE SYSTEM]

		void AddPath(const String& _path);

		bool FileExists(const String& _name, String* _path = nullptr);

		StreamPtr OpenFile(const String& _name, FileStream::Mode _mode = FileStream::Mode::ReadOnly);

		// [RESOURCES]

		//!
		Resource* GetResource(const char* _type, const String& _name, uint _typeid = 0);
		//!
		Resource* GetTempResource(const char* _type, const String& _name, uint _typeid = 0);
		//!
		template <class T> T* GetResource(const String& _name)
		{
			return static_cast<T*>(GetResource(T::TypeName, _name, T::TypeID));
		}
		//!
		template <class T> T* GetTempResource(const String& _name)
		{
			return static_cast<T*>(GetTempResource(T::TypeName, _name, T::TypeID));
		}

		// [DRAW]

		//!
		void Begin2D(const Vector2& _cameraPos, float _zoom = 1);
		//!
		void Flush(void);
		//!
		void Clear(FrameBufferType::Enum _buffers, const Vector4& _color = { 0, 0, 0, 0 }, float _depth = 1, int _stencil = 0xff);
		//!
		void Draw(PrimitiveType::Enum _type, const Vertex* _vertices, uint _count, Texture* _texture, uint _mode);
		//!
		Vertex* AddBatch(PrimitiveType::Enum _type, uint _count, Texture* _texture, uint _mode);

	protected:
		//!
		static long __stdcall _WindowCallback(void* _wnd, uint _msg, uint _wParam, long _lParam);
		//!
		long _HandleMessage(uint _msg, uint _wParam, long _lParam);


		int m_wndcls = 0;
		void* m_window = nullptr;
		void* m_dc = nullptr;
		void* m_rc = nullptr;
		bool m_fullscreen = false;
		bool m_active = false;

		IntVector2 m_size = { 0, 0 };

		bool m_opened = false;
		bool m_userRequireExit = false;

		bool m_vsync = true;

		double m_prevTime = 0;
		float m_timeScale = 1;
		float m_frameTime = 0;
		float m_unscaledFrameTime = 0;

		PrimitiveType::Enum m_batchType = PrimitiveType::Points;
		SharedPtr<Texture> m_texture;
		uint m_batchMode = 0;
		//Array<Vertex> m_batch;
		Vertex* m_batch = nullptr;
		uint m_batchSize = 0;
		uint m_batchMaxSize = 0xffff;

		HashMap<uint, String> m_paths;

		HashMap<uint, HashMap<uint, ResourcePtr>> m_resources;
	};


} // namespace Easy2D