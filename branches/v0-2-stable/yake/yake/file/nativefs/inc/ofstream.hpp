#ifndef _NATIVEFS_OFSTREAM_
#define _NATIVEFS_OFSTREAM_

#include <common/inc/path.hpp>
#include <fstream>

namespace filesystem
{
namespace nativefs
{

template < class charT, class traits = std::char_traits<charT> >
class basic_ofstream : public std::basic_ofstream<charT,traits>
{
public:
	basic_ofstream() {}

      explicit basic_ofstream( const path & file_ph )
        : std::basic_ofstream<charT,traits>(
        file_ph.native_file_string().c_str(), std::ios_base::out ) {}
      basic_ofstream( const path & file_ph,
        std::ios_base::openmode mode )
        : std::basic_ofstream<charT,traits>(
        file_ph.native_file_string().c_str(), mode ) {}
      void open( const path & file_ph )
      {
        std::basic_ofstream<charT,traits>::open(
          file_ph.native_file_string().c_str(), std::ios_base::out );
      }
      void open( const path & file_ph,
        std::ios_base::openmode mode )
      {
        std::basic_ofstream<charT,traits>::open(
          file_ph.native_file_string().c_str(), mode );
      }
      virtual ~basic_ofstream() {}
};

typedef basic_ofstream<char> ofstream;

} // nativefs
} // filesystem

#endif