#ifndef _VIRTUALFS_IFSTREAM_
#define _VIRTUALFS_IFSTREAM_

#include "operations.hpp"
#include "create_filebuf.hpp"

namespace filesystem
{
namespace virtualfs
{

using namespace std;

template<class _Elem,	class _Traits>
class basic_ifstream
		: public basic_istream<_Elem, _Traits>
{
public:
	typedef basic_ifstream<_Elem, _Traits> _Myt;
	typedef basic_filebuf<_Elem, _Traits> _Myfb;
	typedef basic_ios<_Elem, _Traits> _Myios;
	typedef basic_istream<_Elem, _Traits> _Mysb;

	basic_ifstream()
		: streambuf_(0), _Mysb(streambuf_)
	{	// construct unopened
	}

	basic_ifstream( std::streambuf & buf )
		: streambuf_(&buf), _Mysb(streambuf_)
	{	// construct with streambuffer
		_Mysb::init( streambuf_ );
	}

	explicit basic_ifstream( const path & file_path,
		ios_base::openmode mode = ios_base::in,
		int prot = (int)ios_base::_Openprot )
		: streambuf_(0), _Mysb(streambuf_)
	{	// construct with named file and specified mode
		open( file_path, mode, prot );
	}

	explicit basic_ifstream( handle this_file )
		: streambuf_(0), _Mysb(streambuf_)
	{	// construct with specified C stream
		streambuf_ = new filebuf( this_file );
	}

	virtual ~basic_ifstream()
	{	// destroy the object
		delete streambuf_;
	}

	_Myfb * rdbuf() const
	{	// return pointer to file buffer
		return streambuf_;
	}

	void open( const path & file_path, ios_base::open_mode mode )
	{	// open named file with specified mode (old style)
		open( file_path, 
			static_cast<ios_base::openmode>(mode), 
			static_cast<int>( ios_base::_Openprot ) );
	}

	void open( const path & file_path,
		ios_base::openmode mode = ios_base::in,
		int prot = static_cast<int>( ios_base::_Openprot ) )
	{	// open a C stream with specified mode
		streambuf_ = &create_filebuf( file_path, mode/*, prot*/ ); // todo
		_Mysb::init( streambuf_ ); // todo, plattform independent? use setstate?

	}

private:
	std::streambuf * streambuf_;
};

typedef basic_ifstream< char, std::char_traits< char > > ifstream;

} // virtualfs
} // filesystem

#endif // _VIRTUALFS_IFSTREAM_