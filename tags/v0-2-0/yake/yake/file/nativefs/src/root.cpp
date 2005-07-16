#include "inc/root.hpp"

namespace filesystem
{

// Container holding normalized root paths 
// ( paths without trailing seperator ).
static root_path_list root_paths;

// Adds a Root Path.
YAKE_FILESYSTEM_DECL void add_root_path( const path & root_path )
{
	// Not an absolute path?	
	if( !root_path.is_complete() )
		throw_exception( filesystem_error( 
			"add_root_path", root_path, path_error ) );

	// Already in the list?
	if( find( root_paths.begin(), root_paths.end(), 
		const_cast< path * >( &root_path )->normalize().string() ) 
		!= root_paths.end() )
		throw_exception( filesystem_error( "add_root_path",
			root_path, already_exists_error ) );

	// Add the Root Path.
	root_paths.push_back( root_path.string() );
}

// Removes a Root Path.
YAKE_FILESYSTEM_DECL void remove_root_path( const path & root_path )
{
	// Not in the list yet?
	if( find( root_paths.begin(), root_paths.end(), 
		const_cast< path * >( &root_path )->normalize().string() ) 
		== root_paths.end() )
		throw_exception( filesystem_error( 
			"remove_root_path", root_path, not_found_error ) );	

	// Remove the Root Path.
	root_paths.erase( find( root_paths.begin(), root_paths.end(), 
		const_cast< path * >( &root_path )->normalize().string() ) );
}

// Removes a Root Path.
YAKE_FILESYSTEM_DECL void remove_root_path( dword index )
{
	// Invalid parameter?
	if( index >= root_paths.size() )
		throw_exception( filesystem_error( 
			"remove_root_path", not_found_error ) );	

	// Remove the Root Path.
	root_paths.erase( root_paths.begin() + index );
}

// Returns the Number of Root Paths.
YAKE_FILESYSTEM_DECL dword get_num_root_paths()
{
	return static_cast< dword >( root_paths.size() );
}

// Returns the Root path for the specified Index.
YAKE_FILESYSTEM_DECL path get_root_path( dword index )
{
	// Invalid Parameter?
	if( index >= root_paths.size() )
		throw_exception( filesystem_error( 
			"get_root_path", not_found_error ) );	

	// Get the Root Path.
	return path( root_paths[ index ] );
}

// Returns all Root Paths.
YAKE_FILESYSTEM_DECL root_path_list & get_root_paths()
{
	// Get the Root Paths.
	return root_paths;
}

} // filesystem