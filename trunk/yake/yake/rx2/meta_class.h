#ifndef meta_class_h
#define meta_class_h

#include <vector>
#include <yake/base/templates/yakePointer.h>
#include "class_registry.h"
#include "meta_object.h"
#include "meta_hooks.h"
#include "type_info.h"

namespace rx
{

// is used for the instance an meta object resp. its typed fields
template<class T>
struct attach_field
{
	typedef typed_field<T> typed;

	static void attach( meta_object & obj, meta_field & field, bool clone )
	{
		if( clone )
		{
			typed & this_field = dynamic_cast< typed& >( field );
			obj.add_field< typed >( *new typed( obj, this_field.name_, this_field.value_, this_field.flags_ ) );
		}
		else
		{
			field.object_ = &obj;
			obj.add_field< typed >( dynamic_cast< typed& >( field ) );
		}
	}
};

class meta_class
{
public: // types
	/* fields */
	// we need the smart pointer here, because the deconstructor cannot take care of
	// deleting (there can be n copies at the same time [ for instance when define<>
	// is using the copy constructor multiple times ], so we can't just delete the
	// objects without counting references within other meta_class copies)
	typedef std::vector< std::pair<
		yake::base::templates::SharedPtr< meta_field >, 
		void(*)(meta_object&, meta_field&, bool) > > fields_list;

	/* event and handler traits */
	typedef std::vector< TypeInfo > arg_types;
	typedef std::map< std::string, arg_types > traits;

public: // constructors
	meta_class()
	{
    register_class( *this );
	}

	meta_class( std::string name )
		: name_( name )
	{
		register_class( *this );
	}

	template< typename T1 >
	meta_class( std::string name, T1 & t1 )
		: name_( name )
	{
		register_class( *this );
		add_field( t1 );
	}

	meta_class( const meta_class & copy )
		: name_( copy.name_ ),
			fields_( copy.fields_ ),
			handler_traits_( copy.handler_traits_ )
	{
		register_class( *this );       
	}
    
	~meta_class()
	{
		unregister_class( *this );
	}

public: // field management
	template< typename T >
	meta_class & add_field( std::string field_name, T default_value = T() )
	{
		return add_field( field_name, default_value, none );
	}

	template< typename T, int flags >
	meta_class & add_field( std::string field_name, T default_value = T() )
	{
		return add_field( field_name, default_value, flags );
	}

	// used by script hook
	template< typename T >
	meta_class & add_field( std::string field_name, T default_value, int flags )
	{
		// add field to container with typed attach method
		fields_.push_back( std::make_pair(
			new typed_field<T>( field_name, default_value, flags ),
			attach_field<T>::attach ) );
		//  return reference to this
		return *this;
	}

	template< typename T >
	meta_class & add_field( T & field )
	{
		// add field to container with typed attach method
		fields_.push_back( std::make_pair(
			&field,
			attach_field<T::value_type>::attach ) );
		//  return reference to this
		return *this;
	}

	template< typename T >
	typed_field<T> & field( std::string name )
	{
		for( fields_list::iterator iter = fields_.begin();
			iter != fields_.end(); iter++ )
		{
      if( iter->first->name_ == name )
				return static_cast< typed_field<T>& >( *iter->first.get() );
		}
		throw exception();
	}

public: // events and handlers
	template< typename T1 >
	meta_class & add_handler( const std::string & name )
  {
		arg_types args;
    args.push_back( typeid( T1 ) );
    handler_traits_.insert( traits::value_type( name, args ) );
		return *this;
  }

	template< typename T1 >
	meta_class & add_event( const std::string & name )
  {
		arg_types args;
    args.push_back( typeid( T1 ) );
    event_traits_.insert( traits::value_type( name, args ) );
		return *this;
  }

public: // object creation	
	friend meta_object & create( const meta_class & meta_class_, std::string object_name );	
	template< typename T1, typename T2, typename T3 >
	friend meta_object & create( const meta_class & meta_class_, std::string object_name, T1 & f1, T2 & f2, T3 & f3 );

public: // info
	std::string & get_name() const
	{
		return name_;
	}

private:
	mutable std::string name_;
	fields_list fields_;
  traits handler_traits_;
  traits event_traits_;
};

// used by regular meta object instancing
meta_object & create( const meta_class & meta_class_, std::string object_name );

// used by c++ classes
template< typename T1, typename T2, typename T3 >
meta_object & create( const meta_class & meta_class_, std::string object_name, T1 & f1, T2 & f2, T3 & f3 )
{
	meta_class & non_const_class = const_cast< meta_class & >( meta_class_ );

	meta_object * obj = new meta_object( object_name );

	// set name
	f1.field_name_ = non_const_class.fields_[0].first->get_name();
	// call the according attach method for this field ( using reference )
	( non_const_class.fields_[0].second )( *obj, f1 );
	f1.field_name_ = non_const_class.fields_[1].first->get_name();
	( non_const_class.fields_[1].second )( *obj, f2 );
	f1.field_name_ = non_const_class.fields_[2].first->get_name();
	( non_const_class.fields_[2].second )( *obj, f3 );

	return *obj;
}

template< typename T >
T define( const std::string & name )
{
  return T( name );
}

} // namespace rx

#endif