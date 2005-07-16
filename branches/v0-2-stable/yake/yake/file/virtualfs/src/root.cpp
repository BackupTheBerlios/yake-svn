#include <inc/root.hpp>

namespace filesystem
{
namespace virtualfs
{

// Container holding normalized root paths 
// ( paths without trailing seperator ).
static root_path_list root_paths;

// Find pair.
root_path_list::iterator find( root_entry::first_type value )
{
	for( root_path_list::iterator iter = root_paths.begin();
		iter != root_paths.end();
		iter++ )
	{
    if( iter->first == value )
			return iter;
	}
	return root_paths.end();
}

// Adds a root path.
void add_root_path( const path & root_path, path_type type )
{
	// Not an absolute path?	
	if( type == local_path 
		&& !const_cast<path&>( root_path ).normalize().is_complete() )
	{
		throw( filesystem_error( 
			"add_root_path", root_path, path_error ) );
	}

	// Already in the list?
	if( find( root_path.string() ) != root_paths.end() )
	{
		throw( filesystem_error( "add_root_path",
			root_path, already_exists_error ) );
	}

	// Add the root path.
	root_paths.push_back( root_entry( root_path.string(), type ) );
}

// Adds a root path.
void add_root_path( const std::string root_path, path_type type )
{
	// Not an absolute path?	
	if( type == local_path && !path( root_path ).normalize().is_complete() )
		throw( filesystem_error( 
			"add_root_path", root_path, path_error ) );

	// Already in the list?
	if( find( root_path ) != root_paths.end() )
	{
		throw( filesystem_error( "add_root_path",
			root_path, already_exists_error ) );
	}

	// Add the root path.
	root_paths.push_back( root_entry( root_path, type ) );
}

// Removes a root path.
void remove_root_path( const path & root_path )
{
	// Not in the list yet?
	if( find(	const_cast< path * >( &root_path )->normalize().string() ) 
		== root_paths.end() )
	{
		throw( filesystem_error( 
			"remove_root_path", root_path, not_found_error ) );
	}

	// Remove the root path.
	root_paths.erase( find( const_cast< path * >( &root_path )->normalize().string() ) );
}

// Removes a root path.
void remove_root_path( dword index )
{
	// Invalid parameter?
	if( index >= root_paths.size() )
		throw( filesystem_error( 
			"remove_root_path", not_found_error ) );	

	// Remove the root path.
	root_paths.erase( root_paths.begin() + index );
}

// Returns the number of root paths.
dword get_num_root_paths()
{
	return static_cast< dword >( root_paths.size() );
}

// Returns the root path for the specified index.
path get_root_path( dword index )
{
	// Invalid parameter?
	if( index >= root_paths.size() )
		throw( filesystem_error( 
			"get_root_path", not_found_error ) );	

	// Get the root path.
	return path( root_paths[ index ].first );
}

// Returns all root paths.
const root_path_list & get_root_paths()
{
	// Get the root paths.
	return root_paths;
}

} // namespace virtualfs
} // namespace filesystem