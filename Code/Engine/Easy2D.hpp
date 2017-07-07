#pragma once

#include "Base.hpp"

#include "Object.hpp"
#include "System.hpp"

#include "File.hpp"
#include "Time.hpp"

#include "Json.hpp"
#include "Math.hpp"

#include "Device.hpp"

#include "Resource.hpp"

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// 
	//----------------------------------------------------------------------------//


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

		// [LOOP]

		//!
		void BeginFrame(void);
		//!
		void EndFrame(void);

		//!
		void SetVSync(bool _vsync);

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

		bool m_vsync = true;

		PrimitiveType::Enum m_batchType = PrimitiveType::Points;
		SharedPtr<Texture> m_texture;
		uint m_batchMode = 0;
		//Array<Vertex> m_batch;
		Vertex* m_batch = nullptr;
		uint m_batchSize = 0;
		uint m_batchMaxSize = 0xffff;
	};


} // namespace Easy2D