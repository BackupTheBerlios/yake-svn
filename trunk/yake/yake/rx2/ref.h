#ifndef _REF_H_
#define _REF_H_

// todo not needed anymore

/*#include <iostream>

// pure ref test
int hello = 123;
ref<int> iref( hello );
assert( iref == hello );


template< typename T >
struct ref
{
	ref( T & reference ) : reference_(&reference)
	{
	}

	ref() : reference_(NULL)
	{
	}

	T & operator=( T const & value )
	{
		return *reference_ = value;
	}

	void set( T & wrapped_value )
	{
    reference_ = &wrapped_value;
	}

	T get()
	{
		return *reference_;
	}

	// explicit conversion
	operator T&()
	{
		return *reference_;
	}

  T * reference_;
};

template< typename T >
std::ostream& operator<<( std::ostream & stream, const ref<T> & reference )
{
	stream << *reference.reference_;
	return stream;    
}

template< typename T >
bool operator==( ref<T> & reference, T value )
{
	return *reference.reference_ == value;    
}*/

#endif