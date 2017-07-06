#pragma once

#include "Base.hpp"

#include "Object.hpp"
#include "System.hpp"

#include "File.hpp"
#include "Time.hpp"

#include "Json.hpp"

typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;

namespace Easy2D
{
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


		// [FILE SYSTEM]

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

		SDL_Window* m_window = nullptr;
		SDL_GLContext m_context = nullptr;
		IntVector2 m_size = { 0, 0 };

		bool m_opened = false;
		bool m_userRequireExit = false;

		bool m_vsync = true;

		PrimitiveType::Enum m_batchType = PrimitiveType::Points;
		SharedPtr<Texture> m_texture;
		uint m_batchMode = 0;
		//Array<Vertex> m_batch;
		Vertex* m_batch = nullptr;
		uint m_batchSize = 0;
		uint m_batchMaxSize = 0xffff;


		HashMap<uint, HashMap<uint, ResourcePtr>> m_resources;

		Time m_time;
		FileSystem m_fileSystem;
	};


} // namespace Easy2D