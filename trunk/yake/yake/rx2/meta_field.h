#ifndef meta_field_h
#define meta_field_h

#include "type_info.h"

#include <typeinfo>
#include <string>

namespace rx
{

class meta_object;

// todo, different flags, use hex
enum field_flags
{
	none					= 1,
	save					= 2,
	load					= 4,
	replicate			= 8,
	server				= 16,
	client				= 32,
	uncompressed	= 64,
	compressed		= 128,
	copy					= 256,
	interpolate		= 512,
	extrapolate		= 1024,
	predict				= 2048
};

struct meta_field
{	
public: // constructors
	meta_field( int flags = none )
		: object_(0), 
			flags_(flags)
	{}

	meta_field( meta_object & object, 
		std::string name, int flags = none )
		: object_( &object ), 
			name_( name ),
			flags_(flags)
	{}

	meta_field( std::string name,	int flags = none )
		: name_( name ), 
			object_(0), 
			flags_(flags)
	{}

	virtual ~meta_field() {}

public: // info
	virtual const TypeInfo type() = 0;
  virtual std::string value_as_string() = 0;
	std::string as_string();

	meta_object & get_object()
	{
		if( !object_ ) throw exception();
		return *object_;
	}

	std::string & get_name()
	{
		return name_;
	}

public: // data
	meta_object * object_;
	std::string name_;
	int flags_;
};

} // namespace rx

#endif