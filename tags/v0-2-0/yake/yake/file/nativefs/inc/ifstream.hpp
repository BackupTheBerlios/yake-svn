#ifndef _NATIVEFS_IFSTREAM_
#define _NATIVEFS_IFSTREAM_

#include <common/inc/path.hpp>
#include <fstream>

namespace filesystem
{
namespace nativefs
{   

template < class charT, class traits = std::char_traits<charT> >
class basic_ifstream : public std::basic_ifstream<charT,traits>
{
public:
	basic_ifstream() {}

	explicit basic_ifstream( const path & file_ph )
        : std::basic_ifstream<charT,traits>(
        file_ph.native_file_string().c_str(), std::ios_base::in ) {}

    basic_ifstream( const path & file_ph,
        std::ios_base::openmode mode )
        : std::basic_ifstream<charT,traits>(
        file_ph.native_file_string().c_str(), mode ) {}

	void open( const path & file_ph )
	{
        std::basic_ifstream<charT,traits>::open(
          file_ph.native_file_string().c_str(), std::ios_base::in );
	}

	void open( const path & file_ph,
        std::ios_base::openmode mode )
	{
        std::basic_ifstream<charT,traits>::open(
          file_ph.native_file_string().c_str(), mode );
	}

	virtual ~basic_ifstream() {}
};

typedef basic_ifstream<char> ifstream;

} // nativefs
} // filesystem

#endif