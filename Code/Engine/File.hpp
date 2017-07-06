#pragma once

#include "System.hpp"

namespace Easy2D
{
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
	// FileSystem
	//----------------------------------------------------------------------------//

#define gFileSystem Easy2D::FileSystem::Instance

	class FileSystem : public Module<FileSystem>
	{
	public:
		//!
		FileSystem(void);
		//!
		~FileSystem(void);

		void AddPath(const String& _path);

		bool FileExists(const String& _name, String* _path = nullptr);

		StreamPtr OpenFile(const String& _name, FileStream::Mode _mode = FileStream::Mode::ReadOnly);

	protected:
		HashMap<uint, String> m_paths;
	};

	//----------------------------------------------------------------------------//
	// 
	//----------------------------------------------------------------------------//
}
