// use static read write functions within partial specialized structs for int, float etc
// default struct = error, unknown type, not supported by network
// see metaclass

struct observer
{
	virtual ~observer() {}
};

template< typename T >
struct tnl_extrapolation_observer : public observer
{
	typedef typed_field<T> field_type;

	tnl_extrapolation_observer( field_type & field )
		: field_( field )
	{    
	}

	bool is_override_needed( T client_value )
	{
		// todo: ( |soll - ist| / soll ) > min_error
		// ( max(soll, ist ) - min( soll, ist ) / soll
    return ( ( field_value_ / client_value ) > field_.min_error_ );
	}

	void input_from_server( T server_value )
	{
		if( ( field_.value_ / server_value > field_.min_error_ ) &&
			( field_.value_ / server_value < field_.max_error_ )
		{ // interpolate client value

		}
		else
		{ // override client value
      field_.value_ = server_value;    			
		}
	}

	field_type field_;
};

struct tnl_hook : public NetObject
{

	// Let's assume that the approach adds int, float, string to the
	// meta object, all we have to do in order to get the type informations
	// of these three meta fields is to store the "types" in the same order.
	// Whenever a value changes, we simply send the position of the field
	// within objects' field container and we can lookup the according type
	// in our field_types container. Finally we get the actual field and it's type
	typedef std::vector< type_info > field_types;
	typedef std::vector< meta_field* > dirty_fields; 
	typedef std::map< int, observer* > observers;

	tnl_hook() {}
	virtual ~tnl_hook() {}

	void on_add_field( float_field & field )
	{
		// todo push_back( typeid(float) ) or more generic;
		field_types_.push_back( typeid(float) );

		if( ( field.flags_ & extrapolate ) && ( field.flags_ & server ) )
		{
			observers_.insert( 
				observers::value_type(
					get_field_position( field ), new tnl_extrapolation_observer<float>( field ) ) );
		}
	}

	// tnl callback
	void on_network_input()
	{
		unsigned int position = stream->ReadUInt();

    switch( field_types_[ position ] )
		{
				case typeid(float):
				{
					float_field & field = static_cast< float_field& >( get_field( position ) );
					float new_value = Stream->ReadFloat();
          if( field.flags_ & extrapolate )
					{
						if( is_server() )
						{	// compare server and arrived client data
							if( static_cast< tnl_extrapolation_observer<float>* >( 
								observers_[ position ] )->is_override_needed(	new_value ) )
							{ // we need to override client data
								dirty_fields_.push_back( &field );
							}
						}
						else
						{	// override client with server data
							static_cast< tnl_extrapolation_observer<float>* >( 
								observers_[ position ] )->input_from_server( new_value );	
            }
					}
					break;
				}


			} // switch
	}

	// tnl callback
	void on_network_output()
	{
		for( dirty_fields::iterator iter = dirty_fields_.begin();
			iter != dirty_fields.end(); iter++ )
		{
			// write field position
			unsigned int position = get_field_position( *iter );
			stream->writeUInt( position );
			// write actual value
      switch( field_types_[ position ] )
			{
				case typeid(float):
					stream->writeFloat( 
						static_cast< float_field* >( 
							iter )->value_ );
					break;
			}
		}

	}

	inline void on_change_field( float_field & field )
	{
		if( ( field.flags_ & predict ) && is_server() )
		{

		}
		else
		{
      dirty_fields_.push_back( &field );
		}
	}

	dirty_fields dirty_fields_;
	field_types field_types_;
	observers observers_;
};