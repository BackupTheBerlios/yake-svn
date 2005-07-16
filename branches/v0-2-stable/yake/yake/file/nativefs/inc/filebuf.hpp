#ifndef _NATIVEFS_FILEBUF_
#define _NATIVEFS_FILEBUF_

#include <common/inc/path.hpp>
#include <fstream>

namespace filesystem
{
namespace nativefs
{

template < class charT, class traits = std::char_traits<charT> >
class basic_filebuf : public std::basic_filebuf<charT,traits>
{
public:
	virtual ~basic_filebuf() {}

	std::basic_filebuf<charT,traits> * open( const path & file_ph,
        std::ios_base::openmode mode )
	{
		// todo root paths, syntax
        return std::basic_filebuf<charT,traits>::open(
          file_ph.native_file_string().c_str(), mode );
    }

};

typedef basic_filebuf<char> filebuf;


} // nativefs
} // filesystem

#endif