#ifndef meta_class_h
#define meta_class_h

#include <vector>

#include <boost/type_traits/remove_reference.hpp>

#include <yake/base/templates/yakePointer.h>

#include "class_registry.h"
#include "meta_object.h"
#include "meta_hooks.h"
#include "type_info.h"
#include "instance.h"
#include "events.h"

namespace rx
{

struct meta_object_holder;

// This is used for attaching clones of meta_class-fields to the newly created meta object
template<typename T>
struct attach
{
	typedef typed_field<T> field_type;
	static void clone_field( meta_object & obj, meta_field & field )
	{
		// add clone to meta object
		field_type & this_field = dynamic_cast< field_type& >( field );
		obj.add_field< T >( this_field.name_, this_field.value_, this_field.flags_ );
	}

	typedef event<T> event_type;
	static void clone_event( meta_object & obj, event_base & event, std::string & name )
	{
		// add clone to meta object
		event_type & this_event = dynamic_cast< event_type& >( event );
		obj.add_event( name, *new event_type( this_event ) ); // todo delete, same problem with c++ fields, see dcon metaobject
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
	typedef std::vector
	< 
		std::pair
		<
			yake::base::templates::SharedPtr< meta_field >, 
			void(*)(meta_object&, meta_field&) 
		> 
	> fields_list;

	// todo: cleanup with triar<> make_triar or struct
	typedef std::vector
	< 
		std::pair
		<
			yake::base::templates::SharedPtr< event_base >,
			std::pair
			<
				void(*)(meta_object&, event_base&, std::string&),
				std::string
			>
		> 
	> events_list;

	/* event and handler traits */
	typedef std::vector< TypeInfo > arg_types;
	typedef std::vector< std::pair< std::string, arg_types > > traits;

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
	meta_class & add_field( std::string field_name, 
		typename boost::remove_reference<T>::type default_value = 
			boost::remove_reference<T>::type() )
	{
		return add_field( field_name, default_value, none );
	}

	template< typename T, int flags >
	meta_class & add_field( std::string field_name, 
		typename boost::remove_reference<T>::type default_value = 
			boost::remove_reference<T>::type() )
	{
		return add_field( field_name, default_value, flags );
	}

	// used by script hook
	template< typename T >
	meta_class & add_field( std::string field_name, T default_value, int flags )
	{
		// add field to container with typed attach method
		fields_.push_back
		( 
			std::make_pair
			(
				new typed_field<T>( field_name, default_value, flags ),
				attach<T>::clone_field 
			) 
		);
		//  return reference to this
		return *this;
	}

	template< typename T >
	meta_class & add_field( T & field )
	{
		// add field to container with typed attach method
		fields_.push_back
		( 
			std::make_pair
			(
				&field,
				attach<T::value_type>::clone_field 
			) 
		);
		//  return reference to this
		return *this;
	}

	template< typename T >
	typed_field<T> & field( std::string name ) const
	{
		for( fields_list::const_iterator iter = fields_.begin();
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
		// create traits for validation
		arg_types args;
    args.push_back( typeid( T1 ) );
		handler_traits_.push_back( std::make_pair( name, args ) );
		return *this;
  }

	template< typename T1 >
	meta_class & add_event( const std::string & name )
  {
		// create traits for validation
		arg_types args;
    args.push_back( typeid( T1 ) );
		event_traits_.push_back( std::make_pair( name, args ) );
		// create event object
		events_.push_back
		( 
			std::make_pair
			(
				new event<T1>(),
				std::make_pair
				(
					attach<T1>::clone_event,
					name
				)
			) 
		);
		return *this;
  }

public: // object creation	
	friend meta_object_holder;
	friend meta_object & instance( const meta_class & meta_class_, std::string object_name );	

public: // info
	std::string & get_name() const
	{
		return name_;
	}

public:
	mutable std::string name_;
	fields_list fields_;
	events_list events_;
  traits handler_traits_;
  traits event_traits_;
};

template< typename T >
T define( const std::string & name )
{
  return T( name );
}

} // namespace rx

#endif