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
  m_handle = filesystem::nativefs::open_file( ph, mode );
}

void native_file::close()
{
	filesystem::nativefs::close_file( m_handle );
}

const file_system & native_file::get_file_system() const
{
	return native_file_system::instance();
}

int native_file::read( unsigned char * buffer, dword bytes_to_read )
{
  return filesystem::nativefs::read_file( m_handle, buffer, bytes_to_read );
}

int native_file::read_complete_file( unsigned char * buffer, dword bytes_to_read )
{
	return filesystem::nativefs::read_complete_file( m_handle, buffer, bytes_to_read );
}

} // virtualfs
} // filesystem