#ifndef _VIRTUALFS_FILE_HPP_
#define _VIRTUALFS_FILE_HPP_

#include <string>

#include "common/inc/path.hpp"

#include <yake/base/yakeString.h>

#include "config.hpp"
#include "file_system.hpp"

namespace filesystem
{
namespace virtualfs
{

struct file_system;

struct file
{
public:
	enum file_seek_origin	
	{
		FILE_SEEK_SET,
		FILE_SEEK_CURRENT,
		FILE_SEEK_END
	};

public:
	virtual ~file()
	{}

public:
	// the name of the file
	virtual const std::string & get_name() const = 0;

	// the according file system
	virtual const file_system & get_file_system() const = 0;

	// close a stream
	virtual void close() = 0;

	// the output buffer is phisically written to the file
	//virtual void flush() = 0;

	// ------------------------------------------------------------//
	// read block of data from a stream
	virtual int read( unsigned char* buffer, dword bytes_to_read ) = 0;

	// read the whole file from a stream
	virtual int read_complete_file( unsigned char* buffer, dword bytes_to_read = 0 ) = 0;      

	/*// get the next character.
	virtual int getc() = 0;

	// Get an element from a C stream.
	template<class _Elem> inline 
		bool _getc( _Elem& );

	// Get a char element from a C stream.
	template<> inline 
		bool _getc( char& );

	// Push a character back into stream.
	virtual int ungetc( int ) = 0;

	// Put back an arbitrary element to a C stream (always fail).
	template<class _Elem> inline
		bool _ungetc( const _Elem& );

	// Put back a char element to a C stream.
	template<> inline 
		bool _ungetc( const char& );
	
	// Put back a signed char element to a C stream.
	template<> inline 
		bool _ungetc( const signed char& );

	// Put back an unsigned char element to a C stream.
	template<> inline 
		bool _ungetc( const unsigned char& );*/

	// ------------------------------------------------------------//
	// Reposition stream's position indicator.
	/*virtual void seek( long, VFS_SeekOrigin = VFS_SEEK_SET ) = 0;

	// Return the current position in a stream.
	virtual long tell() = 0;

	// Get position in a stream.
	virtual int getPos( fpos_t* ) = 0;

	// Reposition file pointer to a saved location.
	virtual int setPos( fpos_t* ) = 0;

	// ------------------------------------------------------------//
	// Sizing.
	virtual bool resize( long ) = 0;
	virtual long getSize() const = 0;*/
};

} // virtualfs
} // filesystem

#include "file.inl"

#endif // _VIRTUALFS_FILE_HPP_