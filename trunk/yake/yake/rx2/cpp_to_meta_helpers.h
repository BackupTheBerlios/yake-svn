#ifndef _CPP_TO_META_HELPERS_
#define _CPP_TO_META_HELPERS_

#include "static_initor.h"

// 3 fields, 1 ref
#define RX_DEFINE_ONE_REF_THREE_FIELDS( type1, name1, ref1, type2, name2, type3, name3, type4, name4 )\
public:\
rx::typed_field<type1> name1;\
rx::typed_field<type2> name2;\
rx::typed_field<type3> name3;\
rx::typed_field<type4> name4;\
\
static void rx_construct_meta_class()\
{\
	meta_class_\
		.add_field<type1>( #name1 )\
		.add_field<type2>( #name2 )\
		.add_field<type3>( #name3 )\
		.add_field<type4>( #name4 );\
}\
\
void rx_init( const std::string & object_name )\
{\
  meta_object_ =\
	  rx::instance_holder( meta_class_, object_name )\
			.add_field( set_ref( name1, ref1 ) )\
			.add_field( name2 )\
			.add_field( name3 )\
			.add_field( name4 );\
}\
RX_DEFINE_CLASS_OBJECT()

// 1 event, 1 handler
#define RX_DEFINE_ONE_EVENT_ONE_HANDLER( type1, name1, type2, name2 )\
public:\
rx::event<type1> name1;\
\
static void rx_construct_meta_class()\
{\
	meta_class_\
		.add_event<type1>( #name1 )\
		.add_handler<type2>( #name2 );\
}\
\
void rx_init( const std::string & object_name )\
{\
  meta_object_ =\
	 rx::instance_holder( meta_class_, object_name )\
			.add_event( name1 )\
			.add_handler<type2>( name2, this );\
}\
RX_DEFINE_CLASS_OBJECT()

// meta class and object
#define RX_DEFINE_CLASS_OBJECT()\
public:\
static rx::meta_class & get_class(){ return meta_class_; }\
rx::meta_object & get_object(){ return *meta_object_.get(); }\
\
private:\
	static rx::meta_class meta_class_;\
	rx::meta_object_ptr meta_object_;

// construct meta object
#define RX_INIT( object_name ) rx_init( object_name );

// static initor for meta class
#define RX_DEFINE( class_name )\
rx::meta_class class_name::meta_class_( #class_name );\
STATIC_INITOR_CL1( class_name, rx_construct_meta_class )

#endif // _CPP_TO_META_HELPERS_