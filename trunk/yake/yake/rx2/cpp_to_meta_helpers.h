#ifndef _CPP_TO_META_HELPERS_
#define _CPP_TO_META_HELPERS_

#define DEFINE_RX_FIELDS_ONE_REF( type1, name1, ref1, type2, name2, type3, name3, type4, name4 )\
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
static meta_class & get_class(){ return meta_class_; }\
meta_object & get_object(){ return *meta_object_.get(); }\
\
private:\
	static meta_class meta_class_;\
	meta_object_ptr meta_object_;\
\
void rx_init( const std::string & object_name )\
{\
meta_object_ =\
	instance<meta_object>( meta_class_, object_name )\
			.add_field( set_ref( name1, ref1 ) )\
			.add_field( name2 )\
			.add_field( name3 )\
			.add_field( name4 );\
}

#define INIT_RX( object_name ) rx_init( object_name );

#define DEFINE_RX( class_name )\
meta_class class_name::meta_class_( #class_name );\
\
namespace\
{\
	static struct class_name_initor\
	{\
		class_name_initor()\
		{\
			static int counter = 0;\
			if( counter++ > 0 ) return;\
			class_name::rx_construct_meta_class();\
		}\
	} g_class_name_initor;\
} // nameless

#endif // _CPP_TO_META_HELPERS_