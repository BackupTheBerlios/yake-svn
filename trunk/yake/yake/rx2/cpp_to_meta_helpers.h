#ifndef _CPP_TO_META_HELPERS_
#define _CPP_TO_META_HELPERS_

// 3 fields, 1 ref
#define RX_DEFINE_ONE_REF_THREE_FIELDS( type1, name1, ref1, type2, name2, type3, name3, type4, name4 )\
public:\
typed_field<type1> name1;\
typed_field<type2> name2;\
typed_field<type3> name3;\
typed_field<type4> name4;\
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
	  instance<meta_object>( meta_class_, object_name )\
			.add_field( set_ref( name1, ref1 ) )\
			.add_field( name2 )\
			.add_field( name3 )\
			.add_field( name4 );\
}\
RX_DEFINE_CLASS_OBJECT()

// 1 event, 1 handler
#define RX_DEFINE_ONE_EVENT_ONE_HANDLER( type1, name1, type2, name2 )\
public:\
event<type1> name1;\
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
	  instance<meta_object>( meta_class_, object_name )\
			.add_event( &name1 )\
			.add_handler<type2>( name2, this );\
}\
RX_DEFINE_CLASS_OBJECT()

// meta class and object
#define RX_DEFINE_CLASS_OBJECT()\
public:\
static meta_class & get_class(){ return meta_class_; }\
meta_object & get_object(){ return *meta_object_.get(); }\
\
private:\
	static meta_class meta_class_;\
	meta_object_ptr meta_object_;

// construct meta object
#define RX_INIT( object_name ) rx_init( object_name );

// static initor for meta class
#define RX_DEFINE( class_name )\
meta_class class_name::meta_class_( #class_name );\
\
namespace\
{\
	static struct class_name##_initor\
	{\
	class_name##_initor()\
		{\
			static int counter = 0;\
			if( counter++ > 0 ) return;\
			class_name::rx_construct_meta_class();\
		}\
	} g_##class_name##_initor;\
} // nameless

#endif // _CPP_TO_META_HELPERS_