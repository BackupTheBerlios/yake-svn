#include <common/inc/exception.hpp>
#include <nativefs/inc/operations.hpp>
#include <inc/ftp_file_system.hpp>
#include <inc/ftp_file.hpp>

namespace filesystem
{
namespace virtualfs
{

ftp_file::ftp_file( const std::string & file_name) 
	: file_name_(file_name)
{
	handle_ = nativefs::open_file( path( file_name, native ) );
	is_loaded_ = true;
}

ftp_file::~ftp_file()
{
  // still loaded? this should not happen
	if( is_loaded_ )
	{
		close();  
		throw exception();
	}
}

const std::string & ftp_file::get_name() const
{
   return file_name_;
}

void ftp_file::close()
{
	// not loaded?
  if( !is_loaded_ )
		throw( 
			filesystem_error( "virtual_file::close",
				other_error ) );

	// close file handle
	nativefs::close_file( handle_ );

	// delete local cache file
	nativefs::remove( path( file_name_, native ) );

	// data is not loaded
	is_loaded_ = false;
}

const file_system & ftp_file::get_file_system() const
{
	return ftp_file_system::instance();
}

int ftp_file::read( unsigned char * buffer, dword bytes_to_read )
{
	if( !is_loaded_ ) throw exception();

	// return read bytes
	return nativefs::read_file( handle_, buffer, bytes_to_read );
}

int ftp_file::read_complete_file( unsigned char * buffer, dword bytes_to_read )
{
	// read complete file todo
	// return virtualfs::read_complete_file( handle_, buffer, bytes_to_read );
	return 0;
}

} // virtualfs
} // filesystem