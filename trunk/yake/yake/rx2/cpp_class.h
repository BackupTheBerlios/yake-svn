struct moveable
{
	virtual ~moveable() {}

	int position_;
};

// todo meta_object deserialization should set cpp_class members

struct cpp_class : public moveable
{
public:
	cpp_class( std::string object_name ) 
		: meta_object_( meta_class_.create_object( 
			object_name, test_int, test_string, test_float ) )
	{
	}

	typed_field<int> test_int;
	typed_field<std::string> test_string;
	typed_field<float> test_float;
	
public:
	static void build_meta_class()
	{
		// register fields
		meta_class_.add_field<int&>( "test_int", position_ );
		meta_class_.add_field<std::string>( "test_string" );
		meta_class_.add_field<float>( "test_float" );      
	}	

	static meta_class & get_class()
	{
		return meta_class_;
	}


	meta_object & get_object()
	{
		return meta_object_;
	}

private:
	static meta_class meta_class_;
	meta_object & meta_object_;
};

/* todo
the static class calls the meta_class constructor which calls
the register_class function of the class registry and this is
not initialised at this time, so we need a singleton class
registry/yake registry */
meta_class cpp_class::meta_class_( "cpp_class" );

namespace
{
	static struct cpp_class_initor
	{
		cpp_class_initor()
		{
			static int counter = 0;
			if( counter++ > 0 ) return;
			cpp_class::build_meta_class();
		} \
	} g_cpp_class_initor;
} // nameless