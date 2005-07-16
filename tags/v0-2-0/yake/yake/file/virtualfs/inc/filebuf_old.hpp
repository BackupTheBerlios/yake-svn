#ifndef _VIRTUALFS_FILEBUF_
#define _VIRTUALFS_FILEBUF_

#include "file_system.hpp"
#include "operations.hpp"

namespace filesystem
{
namespace virtualfs
{

using namespace std; // todo hmm?

template< class _Elem,	class _Traits >
class YAKE_VIRTUALFS_API basic_filebuf
	: public std::basic_streambuf< _Elem, _Traits >
{
// Types ---------------------------------------------------------//
public:
	// Template
	typedef basic_filebuf<_Elem, _Traits> _Myt;
	typedef std::basic_streambuf<_Elem, _Traits> _Mysb;
	typedef std::codecvt<_Elem, char, typename _Traits::state_type> _Cvt;

	typedef _Elem char_type;
	typedef std::vector<_Elem> char_buffer_type;
	typedef _Traits traits_type;
	typedef typename traits_type::int_type int_type;
	typedef typename traits_type::pos_type pos_type;
	typedef typename traits_type::off_type off_type;

	// reasons for a call to init
	enum Initfl
	{	Newfl, Openfl, Closefl };

// Class ---------------------------------------------------------//
public:
	virtual ~basic_filebuf()
	{ // Close handle (file)
		if( m_Close )
			close();
	}




	static const streamsize pbSize  = 4;
	static const streamsize bufSize = 16;



	explicit basic_filebuf( size_t _buffer_size = bufSize )
		: /*_Mysb(_Noinit),*/ m_hFile(static_cast<handle>(0)), m_buffer(_buffer_size,0)
	{	// constructor

		setg( &(m_buffer[0]) + pbSize,      // beginning of putback area
					&(m_buffer[0]) + pbSize,      // read position
				  &(m_buffer[0]) + pbSize );    // end position   
	}

	basic_filebuf( _Uninitialized )
		: _Mysb(std::_Noinit), m_hFile(static_cast<Handle>(0)))
	{	// construct uninitialized
	}


// Methods ---------------------------------------------------------//
	bool is_open() const
	{	// test if C stream has been opened
		return (m_hFile != 0);
	}

	_Myt* open( const path & file_path,
		std::ios_base::openmode mode,
		int _prot = (int)ios_base::_Openprot )
	{	// open a C stream with specified mode			
		if( m_hFile != 0 || ( m_hFile = open_file( file_path, mode/*, _prot*/ ) ) == 0 )
			return (0);	// open failed

		return (this);	// open succeeded
	}

	_Myt* open( const path & file_path, ios_base::open_mode mode )
	{	// open a C stream with specified mode (old style)
		return ( open( file_path, (ios_base::openmode)mode ) );
	}

	_Myt* close()
	{	// close the C stream
		if( m_hFile != 0  )
		{
			close_file( m_hFile );
			// close succeeded, tidy up
			//init( 0, Closefl );
			return (this);
		}
		else
			return (0);
	}

	/*virtual ~basic_filebuf()
	{ // Close handle (file)
		if( m_Close )
			close();
	}

	basic_filebuf()
		: _Mysb(_Noinit), m_hFile(0)
	{	// constructor
	}

	basic_filebuf( _Uninitialized )
		: _Mysb(_Noinit), m_hFile(0)
	{	// construct uninitialized
	}

	bool is_open() const
	{	// test if C stream has been opened
		return (m_hFile != 0);
	}

	_Myt* open( const path& _rPath,
		std::ios_base::openmode _mode,
		int _prot = (int)ios_base::_Openprot )
	{	// open a C stream with specified mode
		Handle _hFile;
		if( m_hFile != 0 || (_hFile = virtualfs::fopen( _rPath, _mode, _prot ) ) == 0 )
			return (0);	// open failed

		init( _hFile, Openfl );
		initcvt((_Cvt *)&_USE(_Mysb::getloc(), _Cvt));
		return (this);	// open succeeded
	}

	_Myt* open( const path& _rPath, ios_base::open_mode _mode )
	{	// open a C stream with specified mode (old style)
		return ( m_hFilesys->open( _rPath, (ios_base::openmode)_mode ) );
	}

	_Myt* close()
	{	// close the C stream
		if( m_hFile != 0 && _Endwrite() && m_hFilesys->close( m_hFile ) == 0 )
		{	// close succeeded, tidy up
			init( 0, Closefl );
			return (this);
		}
		else
			return (0);
	}*/

protected:


	int pbackfail (int_type c) 
	{
  if ( gptr() != eback() )
	{
    gbump (-1);
    if ( c != EOF )
      *(gptr()) = c;
    return c;
  } 
	else
    return EOF;
	}


	// todo: messy workaround since the vfs does not support buffering (yet)
	virtual int_type underflow()
	{
		if ( gptr() && ( gptr() < egptr()))
			return * reinterpret_cast<unsigned char *>( gptr() );
     
			 int n_putback = static_cast<int>( gptr() - eback() > pbSize ? pbSize : gptr() - eback() );
				
			memcpy( &(m_buffer[0]) + (pbSize - n_putback), 
							gptr() - n_putback, 
							n_putback * sizeof(char_type)	);

			int num = read_file( 
				 m_hFile, 
				 reinterpret_cast<unsigned char*>( &(m_buffer[0])+pbSize ), 
				 static_cast<dword>((m_buffer.size()-pbSize)*sizeof(char_type)) );

        if (num <= 0) // ERROR or EOF
           return EOF;
    
        // reset buffer pointers
        setg( &(m_buffer[0]) + (pbSize - n_putback),   // beginning of putback area
              &(m_buffer[0]) + pbSize,                 // read position
              &(m_buffer[0]) + pbSize + num);          // end of buffer
    
         // return next character
         return * reinterpret_cast<unsigned char *>( gptr() );  
	}

	/*virtual int_type overflow(int_type _Meta = _Traits::eof())
	{	// put an element to stream
			throw exception();
	}

	virtual int_type pbackfail(int_type _Meta = _Traits::eof())
	{	// put an element back to stream
		if (_Mysb::gptr() != 0
			&& _Mysb::eback() < _Mysb::gptr()
			&& (_Traits::eq_int_type(_Traits::eof(), _Meta)
			|| _Traits::eq_int_type(_Traits::to_int_type(_Mysb::gptr()[-1]),
				_Meta)))
		{	// just back up position
			_Mysb::_Gndec();
			return (_Traits::not_eof(_Meta));
		}
		else if (_hFile == 0 || _Traits::eq_int_type(_Traits::eof(), _Meta))
			return (_Traits::eof());	// no open C stream or EOF, fail
		else if (_Pcvt == 0 && m_hFilesys->_ungetc(_Traits::to_char_type(_Meta), _hFile))
			return (_Meta);	// no facet and unget succeeded, return
		else if (_Mysb::gptr() != &_Mychar)
		{	// putback to _Mychar
			_Mychar = _Traits::to_char_type(_Meta);
			_Mysb::setg(&_Mychar, &_Mychar, &_Mychar + 1);
			return (_Meta);
		}
		else
			return (_Traits::eof());	// nowhere to put back
	}

	virtual int_type underflow()
	{	// get an element from stream, but don't point past it
		int_type _Meta;
		if (_Mysb::gptr() != 0 && _Mysb::gptr() < _Mysb::egptr())
			return (_Traits::to_int_type(*_Mysb::gptr()));	// return buffered
		else if (_Traits::eq_int_type(_Traits::eof(), _Meta = uflow()))
			return (_Meta);	// uflow failed, return EOF
		else
		{	// get a char, don't point past it
			pbackfail(_Meta);
			return (_Meta);
		}
	}

	virtual int_type uflow()
		{	// get an element from stream, point past it
		if (_Mysb::gptr() != 0
			&& _Mysb::gptr() < _Mysb::egptr())
			return (_Traits::to_int_type(
				*_Mysb::_Gninc()));	// return buffered
		else if (_hFile == 0)
			return (_Traits::eof());	// no open C stream, fail
		else if (_Pcvt == 0)
			{	// no codecvt facet, just get it
			_Elem _Ch = 0;
			return (m_hFilesys->_getc(_Ch, _hFile) ? _Traits::to_int_type(_Ch)
				: _Traits::eof());
			}
		else
			for (_State0 = _State, _Mystr->erase(); ; )
				{	// get using codecvt facet
				_Elem _Ch, *_Dest;
				const char *_Source;
				ptrdiff_t _Nleft;
				int _Meta = m_hFilesys->fgetc(_hFile);

				if (_Meta == EOF)
					return (_Traits::eof());	// partial char?

				_Mystr->append(1, (char)_Meta);	// append byte and convert
				switch (_Pcvt->in(_State,
					&*_Mystr->begin(), &*_Mystr->begin() + _Mystr->size(),
						_Source, &_Ch, &_Ch + 1, _Dest))
				{	// test result of converting one element
				case codecvt_base::partial:
					_Mystr->erase((size_t)0,	// partial, not done yet
						(size_t)(_Source - &*_Mystr->begin()));
					break;

				case codecvt_base::noconv:
					if (_Mystr->size() < sizeof (_Elem))
						break;	// no conversion, but need more chars

					memcpy(&_Ch, &*_Mystr->begin(),
						sizeof (_Elem));	// copy raw bytes to element
					return (_Traits::to_int_type(_Ch));	// return result

				case codecvt_base::ok:
					for (_Nleft = &*_Mystr->begin() + _Mystr->size() - _Source;
						0 < _Nleft; )
						m_hFilesys->ungetc(_Source[--_Nleft], _hFile);
					return (_Traits::to_int_type(_Ch));	// return result

				default:
					return (_Traits::eof());	// conversion failed
				}
				}
		}

	virtual pos_type seekoff(off_type _Off,
		ios_base::seekdir _Way,
		ios_base::openmode =
			(ios_base::openmode)(ios_base::in | ios_base::out))
	{	// change position by _Off
		fpos_t _Fileposition;

		if (_Mysb::egptr() <= _Mysb::gptr()	// nothing buffered
			|| _Mysb::gptr() != &_Mychar	// nothing putback
			|| _Way != ios_base::cur)		// not a relative seek
			;	// don't have to worry about putback character
		else if (_Pcvt == 0)
			_Off -= (off_type)sizeof (_Elem);	// back up over _Elem bytes
		else
			{	// back up over converted bytes
			for (ptrdiff_t _Nleft = _Mystr->size(); 0 < _Nleft; )
				m_hFilesys->ungetc(_Mystr->operator[](--_Nleft), m_hFile);
			_Mystr->erase();
			_State = _State0;
			}
		if (_hFile == 0 || !_Endwrite()
			|| (_Off != 0 || _Way != ios_base::cur)
				&& m_hFilesys->seek(m_hFile, (long)_Off, _Way) != 0
			|| m_hFilesys->getpos(m_hFile, &_Fileposition) != 0)
			return (pos_type(_BADOFF));	// report failure
		if (_Mysb::gptr() == &_Mychar)
			_Mysb::setg(&_Mychar, &_Mychar, &_Mychar);	// discard any putback
		return (_POS_TYPE_FROM_STATE(pos_type, _State,
			_Fileposition));	// return new position
	}

	virtual pos_type seekpos(pos_type _Pos,
		ios_base::openmode =
			(ios_base::openmode)(ios_base::in | ios_base::out))
		{	// change position to _Pos
		fpos_t _Fileposition = _POS_TYPE_TO_FPOS_T(_Pos);
		off_type _Off = (off_type)_Pos - _FPOSOFF(_Fileposition);

		if (m_hFile == 0 || !_Endwrite()
			|| _pVirtualFs->setpos(_hFile, &_Fileposition) != 0
			|| _Off != 0 && m_hFilesys->seek(m_hFile, (long)_Off, SEEK_CUR) != 0
			|| m_hFilesys->getpos(m_hFile, &_Fileposition) != 0)
			return (pos_type(_BADOFF));	// report failure
		if (_Mystr != 0)
			{	// restore state
			_State = _POS_TYPE_TO_STATE(_Pos);
			_Mystr->erase();
			}
		if (_Mysb::gptr() == &_Mychar)
			_Mysb::setg(&_Mychar, &_Mychar, &_Mychar);	// discard any putback
		return (_POS_TYPE_FROM_STATE(pos_type, _State,
			_Fileposition));	// return new position
		}

	virtual _Mysb *setbuf(_Elem *_Buffer, streamsize _Count)
		{	// offer _Buffer to C stream
		return (m_hFile == 0 || setvbuf(m_hFile, (char *)_Buffer,
			_Buffer == 0 && _Count == 0 ? _IONBF : _IOFBF,
			_Count * sizeof (_Elem)) != 0 ? 0 : this);
		}

	virtual int sync()
		{	// synchronize C stream with external file
		return (m_hFile == 0
			|| _Traits::eq_int_type(_Traits::eof(), overflow())
			|| 0 <= m_hFilesys->flush(m_hFile) ? 0 : -1);
		}

	virtual void imbue(const locale& _Loc)
		{	// set locale to argument (capture nontrivial codecvt facet)
		initcvt((_Cvt *)&_USE(_Loc, _Cvt));
		}

	void init(Handle _hFile, _Initfl _Which)
		{	// initialize to C stream _hFile after {new, open, close}
		static typename _Traits::state_type _Stinit;	// initial state
		_Closef = _Which == _Openfl;
		_Wrotesome = false;

		_Mysb::init();	// initialize stream buffer base object

 #ifndef _IORCNT
  #define _IORCNT	_IOCNT	// read and write counts are the same
  #define _IOWCNT _IOCNT
 #endif // _IORCNT 

		if (_hFile != 0 && sizeof (_Elem) == 1)
			{	// point inside C stream with [first, first + count) buffer
			_Elem **_Pb = (_Elem **)&_hFile->_IOBASE;
			_Elem **_Pn = (_Elem **)&_hFile->_IOPTR;
			int *_Nr = (int *)&_hFile->_IORCNT;
			int *_Nw = (int *)&_hFile->_IOWCNT;
			_Mysb::init(_Pb, _Pn, _Nr, _Pb, _Pn, _Nw);
			}
		m_hFile = _hFile;
		_State = _Stinit;
		_State0 = _Stinit;
		_Pcvt = 0;	// pointer to codecvt facet
		}

	bool _Endwrite()
		{	// put shift to initial conversion state, as needed
		if (_Pcvt == 0 || !_Wrotesome)
			return (true);
		else
			{	// may have to put
			const int _STRING_INC = 8;
			char *_Dest;
			overflow();

			string _Str(_STRING_INC, '\0');
			for (; ; )
				switch (_Pcvt->unshift(_State,
					&*_Str.begin(), &*_Str.begin() + _Str.size(), _Dest))
				{	// test result of homing conversion
				case codecvt_base::ok:
					_Wrotesome = false;	// homed successfully

				case codecvt_base::partial:	// can fall through
					{	// put any generated bytes
					size_t _Count = _Dest - &*_Str.begin();
					if (0 < _Count && _Count !=
						m_hFilesys->fwrite(&*_Str.begin(), _Count, 1, m_hFile))
						return (false);	// write failed
					if (!_Wrotesome)
						return (true);
					_Str.append(_STRING_INC, '\0');	// try some more
					break;
					}

				case codecvt_base::noconv:
					return (true);	// nothing to do

				default:
					return (false);	// conversion failed
				}
			}
		}

	void initcvt(_Cvt *_Newpcvt)
		{	// initialize codecvt pointer
		if (_Newpcvt->always_noconv())
			_Pcvt = 0;	// nothing to do
		else
			{	// set up for nontrivial codecvt facet
			_Pcvt = _Newpcvt;
			_Mysb::init();	// reset any buffering
			if (_Mystr == 0)
				_Mystr = _NEW_CRT _STRING_CRT;	// buy conversion buffer
			}
		}

// Data ---------------------------------------------------------//
private:
	_Cvt *_Pcvt;													// pointer to codecvt facet (may be null)
	typename _Traits::state_type _State0;	// rollback for bad conversion
	_Elem _Mychar;												// putback character, when _ungetc fails
	_STRING_CRT *_Mystr;									// string to hold partial conversion sequences
	bool _Wrotesome;											// true if homing sequence may be needed
	typename _Traits::state_type _State;	// current conversion state*/

	bool m_Close;													// true if C stream must be closed
	handle m_hFile;		// todo style									  // pointer to C stream
	char_buffer_type m_buffer; // Added
};

typedef basic_filebuf< char, std::char_traits< char > > filebuf;


} // virtualfs
} // filesystem

#endif