#include <inc/native_file_system.hpp>
#include <inc/native_file_system_register.hpp>

#include <string>
#include <iostream>

namespace filesystem
{
namespace virtualfs
{

native_file_system::native_file_system()
{
}

bool native_file_system::exists( const path & ph )
{ std::cout << "native_file_system::exists " << ph.string() << std::endl;
	return nativefs::exists( ph );
}

void native_file_system::create_archive_from( const path & archive, const path & dir )
{
  throw( 
		filesystem_error( "yake::filesystem::virtualfs::native_file::create_archive_from",
			not_implemented ) );
}

void native_file_system::create_directory( const path & ph )
{
  // The directory already exists?
	if( exists( ph ) )
		throw( 
			filesystem_error( "yake::filesystem::virtualfs::native_file_system::create_directory",
				already_exists_error ) );

	// Create it!
	nativefs::create_directory( ph );
}

bool native_file_system::remove( const path & ph )
{
  // The directory or file does exist? Remove it!
	return ( exists( ph ) ? nativefs::remove( ph ) : false );
}

unsigned long native_file_system::remove_all( const path & ph )
{
  // The directory or file does exist? Remove it!
	return ( exists( ph ) ? nativefs::remove_all( ph ) : 0 );
}

void native_file_system::rename( const path & from_path, const path & to_path )
{
  nativefs::rename( from_path, to_path );
}

void native_file_system::copy_file( const path & from_file_ph, const path & to_file_ph )
{
  nativefs::copy_file( from_file_ph, to_file_ph );
}

// todo plattform independent and stuff

const char * native_file_system::find_first_file( const char * dir, 
	YAKE_HANDLE & hdl, YAKE_SYSTEM_DIRECTORY_TYPE & data )
{
  std::string dirpath( std::string(dir) + "/*" );
		return ( (hdl = ::FindFirstFileA( dirpath.c_str(), &data ))
			== YAKE_INVALID_HANDLE_VALUE ) ? 0 : data.cFileName;
}

void native_file_system::find_close( YAKE_HANDLE hdl )
{
  assert( hdl != YAKE_INVALID_HANDLE_VALUE );
  ::FindClose( hdl );
}

const char * native_file_system::find_next_file( YAKE_HANDLE hdl, 
	const path & ph, YAKE_SYSTEM_DIRECTORY_TYPE & data )
{
	if( ::FindNextFileA( hdl, &data ) == 0 )
  {
    if( ::GetLastError() != ERROR_NO_MORE_FILES )
      throw( filesystem_error(
				"boost::filesystem::directory_iterator::operator++",
					ph.branch_path(), detail::system_error_code() ) );
    else return 0; // end reached
  }
  return data.cFileName;
}

} // virtualfs
} // filesystem