#ifndef _VIRTUALFS_IFSTREAM_
#define _VIRTUALFS_IFSTREAM_

#include "operations.hpp"
#include "create_filebuf.hpp"

namespace filesystem
{
namespace virtualfs
{

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
		: m_pStreambuf(0), _Mysb(m_pStreambuf)
	{	// construct unopened
	}

	basic_ifstream( std::streambuf& _rStreambuf )
		: m_pStreambuf(&_rStreambuf), _Mysb(m_pStreambuf)
	{	// construct with streambuffer
		_Mysb::init( m_pStreambuf );
	}

	explicit basic_ifstream( const path & file_path,
		ios_base::openmode mode = ios_base::in,
		int prot = (int)ios_base::_Openprot )
		: m_pStreambuf(0), _Mysb(m_pStreambuf)
	{	// construct with named file and specified mode
		open( file_path, mode | ios_base::in, prot );
	}

	explicit basic_ifstream( handle this_file )
		: m_pStreambuf(0), _Mysb(m_pStreambuf)
	{	// construct with specified C stream
		m_pStreambuf = new filebuf( this_file );
	}

	virtual ~basic_ifstream()
	{	// destroy the object
		delete m_pStreambuf;
	}

	_Myfb *rdbuf() const
	{	// return pointer to file buffer
		return m_pStreambuf;
	}

	void open( const path & file_path, ios_base::open_mode mode )
	{	// open named file with specified mode (old style)
		open( file_path, (ios_base::openmode)mode, (int)ios_base::_Openprot );
	}

	void open( const path & file_path,
		ios_base::openmode mode = ios_base::in,
		int prot = (int)ios_base::_Openprot )
	{	// open a C stream with specified mode
		m_pStreambuf = &create_filebuf( file_path, mode/*, prot*/ ); // todo
		_Mysb::init( m_pStreambuf ); // todo, plattform independent? use setstate?

	}

private:
	std::streambuf* m_pStreambuf;
};

typedef basic_ifstream< char, std::char_traits< char > > ifstream;

} // virtualfs
} // filesystem

#endif // _VIRTUALFS_IFSTREAM_