namespace filesystem
{
namespace virtualfs
{

/*// Get an element from a C stream.
template<class _Elem> inline
	bool IFile::_getc( _Elem& _Ch )
{
	return ( read( &_Ch, sizeof (_Elem), 1 ) == 1 );
}

// Get a char element from a C stream.
template<> inline 
	bool IFile::_getc( char& _Byte )
{
	int _Meta;
	if ((_Meta = getc()) == EOF)
		return (false);
	else
	{	// got one, convert to char
		_Byte = (char)_Meta;
		return (true);
	}
}

// Put back an arbitrary element to a C stream (always fail).
template<class IFile::_Elem> inline
	bool IFile::_ungetc( const _Elem&, Handle )
{	
	return (false);
}

// Put back a char element to a C stream.
template<> inline 
	bool IFile::_ungetc( const char& _Byte )
{	
	return (ungetc((unsigned char)_Byte) != EOF);
}

// put back a signed char element to a C stream.
template<> inline 
	bool IFile::_ungetc( const signed char& _Byte )
{	
	return (ungetc((unsigned char)_Byte) != EOF);
}

// put back an unsigned char element to a C stream.
template<> inline 
	bool IFile::_ungetc( const unsigned char& _Byte )
{	
	return (ungetc(_Byte) != EOF);
}*/

} // virtualfs
} // filesystem