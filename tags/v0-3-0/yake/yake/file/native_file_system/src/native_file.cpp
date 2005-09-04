#include <inc/native_file_system.hpp>

#include <string>
#include <iostream>

namespace filesystem
{
namespace virtualfs
{

native_file::native_file( 
	const path & ph, 
	std::ios_base::openmode mode )
	: m_filename(const_cast<path&>(ph))
{
  handle_ = filesystem::nativefs::open_file( ph, mode );
}

void native_file::close()
{
	filesystem::nativefs::close_file( handle_ );
}

const file_system & native_file::get_file_system() const
{
	return native_file_system::instance();
}

int native_file::read( unsigned char * buffer, dword bytes_to_read )
{
  return filesystem::nativefs::read_file( handle_, buffer, bytes_to_read );
}

int native_file::read_complete_file( unsigned char * buffer, dword bytes_to_read )
{
	return filesystem::nativefs::read_complete_file( handle_, buffer, bytes_to_read );
}

std::streamoff native_file::seek( std::streamoff off, std::ios_base::seekdir way )
{
	return filesystem::nativefs::seek_file( handle_, off, way );  
}

} // virtualfs
} // filesystem