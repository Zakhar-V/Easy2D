
#include "Easy2D.hpp"

#include "GLDevice.hpp"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Windows.h>
#include <GL/gl_Load.h>

#pragma comment(lib, "opengl32.lib")

#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // https://raw.githubusercontent.com/nothings/stb/master/stb_image.h


namespace Easy2D
{

	//----------------------------------------------------------------------------//
	// Image
	//----------------------------------------------------------------------------//
	
	/*
	typedef struct gif_result_t {
	int delay;
	unsigned char *data;
	struct gif_result_t *next;
} gif_result;

STBIDEF unsigned char *stbi_xload(char const *filename, int *x, int *y, int *frames)
{
	FILE *f;
	stbi__context s;
	unsigned char *result = 0;

	if (!(f = stbi__fopen(filename, "rb")))
		return stbi__errpuc("can't fopen", "Unable to open file");

	stbi__start_file(&s, f);

	if (stbi__gif_test(&s))
	{
		int c;
		stbi__gif g;
		gif_result head;
		gif_result *prev = 0, *gr = &head;

		memset(&g, 0, sizeof(g));
		memset(&head, 0, sizeof(head));

		*frames = 0;

		while (gr->data = stbi__gif_load_next(&s, &g, &c, 4))
		{
			if (gr->data == (unsigned char*)&s)
			{
				gr->data = 0;
				break;
			}

			if (prev) prev->next = gr;
			gr->delay = g.delay;
			prev = gr;
			gr = (gif_result*) stbi__malloc(sizeof(gif_result));
			memset(gr, 0, sizeof(gif_result));
			++(*frames);
		}

		STBI_FREE(g.out);

		if (gr != &head)
			STBI_FREE(gr);

		if (*frames > 0)
		{
			*x = g.w;
			*y = g.h;
		}

		result = head.data;

		if (*frames > 1)
		{
			unsigned int size = 4 * g.w * g.h;
			unsigned char *p = 0;

			result = (unsigned char*)stbi__malloc(*frames * (size + 2));
			gr = &head;
			p = result;

			while (gr)
			{
				prev = gr;
				memcpy(p, gr->data, size);
				p += size;
				*p++ = gr->delay & 0xFF;
				*p++ = (gr->delay & 0xFF00) >> 8;
				gr = gr->next;

				STBI_FREE(prev->data);
				if (prev != &head) STBI_FREE(prev);
			}
		}
	}
	else
	{
		result = stbi__load_main(&s, x, y, frames, 4);
		*frames = !!result;
	}

	fclose(f);
	return result;
}*/
	//----------------------------------------------------------------------------//
	Image::~Image(void)
	{
		if (m_pixels)
			free(m_pixels);
	}
	//----------------------------------------------------------------------------//
	bool Image::Realloc(uint _width, uint _height, uint _depth, uint _channels)
	{
		if (_depth == 0)
			_depth = 1;
		_channels = Clamp<uint>(_channels, 1, 4);

		if (m_size.x == _width && m_size.y == _height && m_depth == _depth && m_channels == _channels)
			return true;

		if (m_pixels)
			delete[] m_pixels;

		m_size.x = _width;
		m_size.y = _height;
		m_depth = _depth;
		m_channels = _channels;
		m_pixels = (uint8*)malloc(_width * _height * _depth * _channels);

		if(!m_pixels)
		{
			LOG("Error: Unable to reallocate image \"%s\" (%d bytes)", m_name.c_str(), _width * _height * _depth * _channels * sizeof(m_pixels[0]));
			m_size.x = 0;
			m_size.y = 0;
			m_pixels = nullptr;
			return false;
		}

		return true;
	}
	//----------------------------------------------------------------------------//
	uint8* Image::Layer(uint _index)
	{
		ASSERT(_index < m_depth);
		return m_pixels + m_size.x * m_size.y * m_channels * _index;
	}
	//----------------------------------------------------------------------------//
	bool Image::Load(Stream* _src)
	{
		ASSERT(_src != nullptr);

		stbi_io_callbacks _cb =
		{
			//read
			[](void* _ud, char* _dst, int _size)
		{
			Stream* _src = reinterpret_cast<Stream*>(_ud);
			return (int)_src->Read(_dst, _size);
		},
			//skip
			[](void* _ud, int _n)
		{
			Stream* _src = reinterpret_cast<Stream*>(_ud);
			_src->Seek(_n, Stream::SeekOrigin::Current);
		},
			//eof
			[](void* _ud)
		{
			Stream* _src = reinterpret_cast<Stream*>(_ud);
			return (int)_src->EoF();
		}
		};

		int _w = 0, _h = 0, _c = 0;

		uint8* _data = stbi_load_from_callbacks(&_cb, _src, &_w, &_h, &_c, 0);
	
		if (m_pixels)
			free(m_pixels);

		m_pixels = _data;
		m_size.x = _w;
		m_size.y = _h;
		m_depth = 1;
		m_channels = _c;

		if (!_data)
		{
			LOG("Error: Unable to load image \"%s\": %s", m_name.c_str(), stbi_failure_reason());
			return false;
		}

		return true;
	}
	//----------------------------------------------------------------------------//
	bool Image::Save(Stream* _dst)
	{
		return Resource::Save(_dst);
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	// PixelFormat
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	bool PixelFormat::IsCompressed(Enum _format)
	{
		switch (_format)
		{
		case DXT1:
		case DXT5:
			return true;
		}
		return false;
	}
	//----------------------------------------------------------------------------//
	PixelFormat::Enum PixelFormat::GetCompressedFormat(Enum _format)
	{
		switch (_format)
		{
		case R8:
		case RG8:
		case RGB8:
			return DXT1;
		case RGBA8:
			return DXT5;
		}
		return _format;
	}
	//----------------------------------------------------------------------------//
	PixelFormat::Enum PixelFormat::GetUncompressedFormat(Enum _format)
	{
		switch (_format)
		{
		case DXT1:
			return RGB8;
		case DXT5:
			return RGBA8;
		}
		return _format;
	}
	//----------------------------------------------------------------------------//


	struct GLPixelFormatDesc
	{
		uint bpp;
		uint iformat;
		uint type;
		uint format;
	}
	const GLPixelFormat[]=
	{
		{ 8, GL_LUMINANCE8, GL_UNSIGNED_BYTE, GL_LUMINANCE }, // R8
		{ 16, GL_LUMINANCE8_ALPHA8, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA }, // RG8
		{ 24, GL_RGB8, GL_UNSIGNED_BYTE, GL_RGB }, // RGB8
		{ 32, GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA }, // RGBA8
		{ 4, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, GL_RGB }, // DXT1
		{ 8, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, GL_RGBA }, // DXT5
	};

	//----------------------------------------------------------------------------//
	// Texture
	//----------------------------------------------------------------------------//

	const uint GLTextureType[] =
	{
		GL_TEXTURE_2D, // Default
		GL_TEXTURE_3D // Volume
	};

	const uint GLUnusedTextureSlot = 5;

	//----------------------------------------------------------------------------//
	void Texture::Create(Type _type, PixelFormat::Enum _format)
	{
		Destroy();
		m_type = _type;
		m_format = _format;
		glGenTextures(1, &m_handle);
		_Bind(1);
	}
	//----------------------------------------------------------------------------//
	void Texture::Destroy(void)
	{
		if (m_handle)
		{
			glDeleteTextures(1, &m_handle);
			m_handle = 0;
		}
	}
	//----------------------------------------------------------------------------//
	void Texture::Realloc(uint _width, uint _height, uint _depth)
	{
		if (_depth == 0)
			_depth = m_depth;
		if (m_type == Type::Default)
			_depth = 1;

		m_size.x = _width;
		m_size.y = _height;
		m_depth = _depth;

		const GLPixelFormatDesc& _pf = GLPixelFormat[m_format];

		_Bind(GLUnusedTextureSlot);
		if (m_type == Type::Default)
		{
			glTexImage2D(GLTextureType[(uint)m_type], 0, _pf.iformat, _width, _height, 0, _pf.format, _pf.type, nullptr);
		}
		else
		{
			glTexImage3D(GLTextureType[(uint)m_type], 0, _pf.iformat, _width, _height, _depth, 0, _pf.format, _pf.type, nullptr);
		}
	}
	//----------------------------------------------------------------------------//
	void Texture::Write(int _x, int _y, int _z, uint _w, uint _h, uint _d, PixelFormat::Enum _format, const void* _data)
	{
		const GLPixelFormatDesc& _pf = GLPixelFormat[_format];

		_Bind(GLUnusedTextureSlot);
		if (PixelFormat::IsCompressed(_format))
		{
			uint _size = (_pf.bpp * _w * _h * _d) >> 3;

			if (m_type == Type::Default)
			{
				glCompressedTexSubImage2D(GLTextureType[(uint)m_type], 0, _x, _y, _w, _h, _pf.iformat, _size, _data);
			}
			else
			{
				glCompressedTexSubImage3D(GLTextureType[(uint)m_type], 0, _x, _y, _z, _w, _h, _d, _pf.iformat, _size, _data);
			}
		}
		else
		{
			if (m_type == Type::Default)
			{
				glTexSubImage2D(GLTextureType[(uint)m_type], 0, _x, _y, _w, _h, _pf.format, _pf.type, _data);
			}
			else
			{
				glTexSubImage3D(GLTextureType[(uint)m_type], 0, _x, _y, _z, _w, _h, _d, _pf.format, _pf.type, _data);
			}
		}
	}
	//----------------------------------------------------------------------------//
	void Texture::_Bind(uint _slot)
	{
		glActiveTexture(GL_TEXTURE0 + _slot);
		glBindTexture(GLTextureType[(uint)m_type], m_handle);
	}
	//----------------------------------------------------------------------------//
	bool Texture::Load(Stream* _src)
	{
		ASSERT(_src != nullptr);

		Type _type = Type::Default;
		String _source;
		bool _useCompression = false;
		StreamPtr _imgSrc = _src;
		bool _flipX = false, _flipY = false;

		String _ext = PathUtils::Extension(_src->Name());
		if (!StringUtils::Cmpi(_ext.c_str(), "json"))
		{
			Json _desc;
			if (!_desc.Load(_src))
			{
				LOG("Error: Unable to load Texture \"%s\" from \"%s\"", m_name.c_str(), _src->Name().c_str());
				return false;
			}

			_type = StringUtils::Cmpi(_desc["Type"].AsString().c_str(), "volume") ? Type::Default : Type::Volume;
			_source = _desc["Source"];
			_flipX = _desc["FlipX"];
			_flipY = _desc["FlipY"];
			_useCompression = _desc["UseCompression"];

			_imgSrc = gFileSystem->OpenFile(_source);
		}

		ImagePtr _img = new Image;
		if (!_img->Load(_imgSrc))
		{
			LOG("Error: Unable to load Texture \"%s\" from \"%s\"", m_name.c_str(), _src->Name().c_str());
			return false;
		}

		PixelFormat::Enum _format;
		switch (_img->Channels())
		{
		case 1:
			_format = PixelFormat::R8;
			break;
		case 2:
			_format = PixelFormat::RG8;
			break;
		case 3:
			_format = PixelFormat::RGB8;
			break;
		case 4:
			_format = PixelFormat::RGBA8;
			break;
		}

		Create(_type, _useCompression ? PixelFormat::GetCompressedFormat(_format) : _format);
		Realloc(_img->Width(), _img->Height(), _img->Depth());
		Write(0, 0, 0, _img->Width(), _img->Height(), _img->Depth(), _format, _img->Layer(0));

		// temp
		glGenerateMipmap(GLTextureType[(uint)m_type]);

		return true;
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
		new Time;
		new FileSystem;
		new GLDevice;
		new ResourceCache;

		System::SendEvent(SystemEvent::Startup);

		// load opengl
		{
			wglSwapIntervalEXT = reinterpret_cast<decltype(wglSwapIntervalEXT)>(wglGetProcAddress("wglSwapIntervalEXT"));

		}

		SetVSync(true);


		m_batch = new Vertex[m_batchMaxSize];

		Object::Register<Image>();
		Object::Register<Texture>();
	}
	//----------------------------------------------------------------------------//
	Engine::~Engine(void)
	{
		// TODO
		glFlush();
		glFinish();

		System::SendEvent(SystemEvent::Shutdown, nullptr, false);

		delete gResources;
		delete gDevice;
		delete gFileSystem;
		delete gTime;
	}
	//----------------------------------------------------------------------------//
	void Engine::BeginFrame(void)
	{
		System::SendEvent(SystemEvent::BeginFrame);

		m_texture = nullptr;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);

		glViewport(0, 0, gDevice->WindowSize().x, gDevice->WindowSize().y);

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), reinterpret_cast<uint8*>(m_batch) + offsetof(Vertex, color));

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(3, GL_FLOAT, sizeof(Vertex), reinterpret_cast<uint8*>(m_batch) + offsetof(Vertex, tc));

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), reinterpret_cast<uint8*>(m_batch) + offsetof(Vertex, pos));
	}
	//----------------------------------------------------------------------------//
	void Engine::EndFrame(void)
	{
		Flush();

		System::SendEvent(SystemEvent::EndFrame);

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
	void Engine::Begin2D(const Vector2& _cameraPos, float _zoom)
	{
		Flush();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, gDevice->WindowSize().x, gDevice->WindowSize().y, 0, 0, 1);

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
			if (m_texture)
			{
				m_texture->_Bind(0);
				glEnable(GL_TEXTURE_2D); // temp
			}
			else
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
				glDisable(GL_TEXTURE_2D);
			}
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

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}

