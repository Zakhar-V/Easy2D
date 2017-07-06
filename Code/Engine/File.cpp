#include "File.hpp"
#ifdef _MSC_VER
#include <direct.h>
#else
// TODO
#endif

namespace Easy2D
{

	//----------------------------------------------------------------------------//
	// FileStream
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	FileStream::~FileStream(void)
	{
		Close();
	}
	//----------------------------------------------------------------------------//
	bool FileStream::Open(const String& _name, Mode _mode)
	{
		static const char* _modes[] =
		{
			"rb", // ReadOnly
			"rb+", // ReadWriteExistent
			"rb+", // ReadWrite
			"wb+", // Overwrite
		};

		Close();

		m_readOnly = _mode == Mode::ReadOnly;
		m_name = _name;
		m_handle = fopen(_name.c_str(), _modes[(int)_mode]);
		if (!m_handle && _mode == Mode::ReadWrite)
			m_handle = fopen(_name.c_str(), "wb+");

		if (!m_handle)
		{
			LOG("Error: Unable to %s file \"%s\"", (_mode == Mode::ReadWrite || _mode == Mode::Overwrite) ? "create" : "open", _name.c_str());
			return false;
		}

		uint _pos = Tell();
		Seek(0, SeekOrigin::End);
		m_size = Tell();
		Seek(_pos, SeekOrigin::Set);

		return true;
	}
	//----------------------------------------------------------------------------//
	void FileStream::Close(void)
	{
		if (m_handle)
		{
			fclose(m_handle);
			m_handle = nullptr;
		}
	}
	//----------------------------------------------------------------------------//
	bool FileStream::EoF(void)
	{
		return m_handle && feof(m_handle);
	}
	//----------------------------------------------------------------------------//
	void FileStream::Seek(int _offset, SeekOrigin _origin)
	{
		if (m_handle)
			fseek(m_handle, _offset, (int)_origin);
	}
	//----------------------------------------------------------------------------//
	uint FileStream::Tell(void)
	{
		return m_handle ? (uint)ftell(m_handle) : 0;
	}
	//----------------------------------------------------------------------------//
	uint FileStream::Read(void* _dst, uint _size)
	{
		ASSERT(!_size || _dst);
		return m_handle ? (uint)fread(_dst, 1, _size, m_handle) : 0;
	}
	//----------------------------------------------------------------------------//
	uint FileStream::Write(const void* _src, uint _size)
	{
		ASSERT(!_size || _src);
		return (m_handle && !m_readOnly) ? (uint)fwrite(_src, 1, _size, m_handle) : 0;
	}
	//----------------------------------------------------------------------------//
	void FileStream::Flush(void)
	{
		if (m_handle && !m_readOnly)
			fflush(m_handle);
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	// PathUtils
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	bool PathUtils::IsFullPath(const char* _path)
	{
		if (_path && _path[0])
		{
#ifdef _WIN32
			const char* _dev = strchr(_path, ':');
			return _dev && (_dev[1] == '\\' || _dev[1] == '/');
#else
			// TODO
#endif
		}
		return false;
	}
	//----------------------------------------------------------------------------//
	bool PathUtils::IsDelimeter(char _ch)
	{
#ifdef _WIN32
		return _ch == '\\' || _ch == '/';
#else
		// TODO
#endif
	}
	//----------------------------------------------------------------------------//
	String PathUtils::Extension(const char* _path)
	{
		if (_path)
		{
			String _ext;
			for (const char* i = _path + strlen(_path); --i >= _path; )
			{
				if (*i == '.')
				{
					while (*i++ != 0)
						_ext += *i;
					return _ext;
				}
			}
		}

		return StringUtils::EmptyString;
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	// FileSystem
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	FileSystem::FileSystem(void)
	{
		AddPath(""); // root dir
	}
	//----------------------------------------------------------------------------//
	FileSystem::~FileSystem(void)
	{

	}
	//----------------------------------------------------------------------------//
	void FileSystem::AddPath(const String& _path)
	{
		String _fp;

		if (!PathUtils::IsFullPath(_path.c_str()))
		{
			char _cd[4096];
			getcwd(_cd, sizeof(_cd));
			size_t _len = strlen(_cd);
			if (!PathUtils::IsDelimeter(_cd[_len - 1]))
				_cd[_len] = '/', _cd[_len + 1] = 0;

			_fp = _cd + _path;
		}
		else
		{
			_fp = _path;
		}

		if (!PathUtils::IsDelimeter(_fp.back()))
			_fp += "/";

		uint _hash = StringUtils::Hash(_fp.c_str());
		if (m_paths.find(_hash) == m_paths.end())
		{
			LOG("Add Path \"%s\" as \"%s\"", _path.c_str(), _fp.c_str());
			m_paths[_hash] = _fp;
		}
	}
	//----------------------------------------------------------------------------//
	bool FileSystem::FileExists(const String& _name, String* _path)
	{
		if (PathUtils::IsFullPath(_name.c_str()))
		{
			FILE* _test = fopen(_name.c_str(), "rb");
			if (_test)
			{
				fclose(_test);
				if (_path)
					*_path = _name;
				return true;
			}
		}
		else
		{
			for (const auto& i : m_paths)
			{
				ASSERT(PathUtils::IsFullPath(i.second.c_str()));
				if (FileExists(i.second + _name, _path))
					return true;
			}
		}
		return false;
	}
	//----------------------------------------------------------------------------//
	StreamPtr FileSystem::OpenFile(const String& _name, FileStream::Mode _mode)
	{
		FileStreamPtr _file = new FileStream;
		String _path;

		if (FileExists(_name, &_path) || _mode == FileStream::Mode::Overwrite || _mode == FileStream::Mode::ReadWrite)
		{
			_file->Open(_path, _mode);
		}
		else
		{
			LOG("Error: File \"%s\" not found", _name.c_str());
		}

		return _file.Cast<Stream>();
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}
