struct moveable
{
	virtual ~moveable() {}

	int position_;
};

// todo meta_object deserialization should set cpp_class members

struct cpp_class : public moveable
{
public:
	// we can use a chain of typed_fields here, as long as
	// they have the same order as within the meta_class
	// field container see build_meta_class
	cpp_class( std::string object_name ) 
	{
		meta_object_ = 
			create<meta_object>( meta_class_, name )
				.add_field( test_int )
				.add_field( test_string )
				.add_field( test_float )
				.add_handler<int>( "on_click", &cpp_class::on_click, this )
				.add_event( "clicked", &clicked )
	}

	// rx members
	typed_field<int> test_int;
	typed_field<std::string> test_string;
	typed_field<float> test_float;

	// todo we use a reference to an existing variable here, this is kind of the non-instrusive version
	typed_field<int&> position_wrapped;
	
public:
	static void build_meta_class()
	{
		// register fields
		meta_class_
			.add_field<int>( "test_int" )
		  .add_field<std::string>( "test_string" )
			.add_field<float>( "test_float" );      
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
	// one c++ class needs one static meta class
	static meta_class meta_class_;
	// but one c++ class can have n meta objects
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