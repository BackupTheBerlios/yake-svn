#ifndef _VIRTUALFS_FILEBUF_
#define _VIRTUALFS_FILEBUF_

#include <yake/base/templates/yakePointer.h>

#include <iostream>

#include <boost/io/stream_facade.hpp>
#include <boost/io/categories.hpp>

#include <common/inc/path.hpp>
#include "config.hpp"

namespace filesystem
{
namespace virtualfs
{

struct YAKE_VIRTUALFS_API seekable_ressource 
{
private:
	// workaround: we need this small file handle holder for 
	// reference counting using the shared pointer,
	// because boost::io is using the copy constructor
	// several times and we can't just put the close_file
	// call into the deconstructor.
	struct file_object
	{
		file_object( const handle this_file );
		~file_object();
		handle handle_;
	};

public:
    typedef char char_type;
    typedef boost::io::seekable_resource_tag category;

		seekable_ressource() {}
		seekable_ressource( const path & ph, 
			const std::ios_base::openmode mode = std::ios_base::in );
    seekable_ressource( const seekable_ressource & copy );
		~seekable_ressource();

public:
    std::streamsize read( char * s, std::streamsize n );
    void write( const char * s, std::streamsize n );
    std::streamoff seek( std::streamoff off, std::ios_base::seekdir way );
 
private:
	yake::base::templates::SharedPtr<file_object> file_object_;
};

//typedef basic_filebuf< char, std::char_traits< char > > filebuf;
typedef YAKE_VIRTUALFS_API boost::io::streambuf_facade<seekable_ressource> filebuf;

} // virtualfs
} // filesystem

#endif // _VIRTUALFS_FILEBUF_