#ifndef _NATIVE_FILE_HPP_
#define _NATIVE_FILE_HPP_

#include <common/inc/exception.hpp>
#include <nativefs/inc/operations.hpp>
#include <virtualfs/inc/file_system.hpp>
#include <virtualfs/inc/config.hpp>

namespace filesystem
{
namespace virtualfs
{

class native_file : public file
{
public:
	native_file( const path & ph, std::ios_base::openmode mode );

public:
	const std::string & get_name() const
	{
    return m_filename.string();
	}

	void close();

	const file_system & get_file_system() const;

	int read( unsigned char * buffer, dword bytes_to_read );
	int read_complete_file( unsigned char * buffer, dword bytes_to_read = 0 );

	std::streamoff seek( std::streamoff off, std::ios_base::seekdir way );

	/* todo: implement buffered stuff, maybe the vfs implementation should handle this
	using internal ptrs etc.
	see glibc, genops.c => _IO_default_pbackfail, _IO_sungetc, _IO_sputbackc
	int getc()
	{
		char ch;
		read( &ch, sizeof (char), 1 );
		return static_cast< int >( ch );
	}

	int ungetc( int )
	{
		// http://rafb.net/paste/results/dhvVg125.html

		int result;

		if (fp->_IO_read_ptr > fp->_IO_read_base
				&& (unsigned char)fp->_IO_read_ptr[-1] == (unsigned char)c)
		{
				fp->_IO_read_ptr--;
				result = (unsigned char) c;
    }
		else
			result = _IO_PBACKFAIL (fp, c);

		if (result != EOF)
			fp->_flags &= ~_IO_EOF_SEEN;

		return result;
	}*/

private:
	path & m_filename;
	nativefs::handle handle_;
};

} // virtualfs
} // filesystem

#endif // _NATIVE_FILE_HPP_