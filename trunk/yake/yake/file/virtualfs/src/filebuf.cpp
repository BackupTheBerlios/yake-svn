#include <inc/filebuf.hpp>
#include <inc/operations.hpp>

namespace filesystem
{
namespace virtualfs
{

seekable_ressource::file_object::file_object( const handle this_file )
	: handle_( this_file )
{
}

seekable_ressource::file_object::~file_object()
{
	close_file( handle_ );
}


seekable_ressource::seekable_ressource( const path & ph, const std::ios_base::openmode mode )
	: file_object_( new file_object( open_file( ph, mode ) ) )
{
	std::cout << "seekable_ressource::seekable_ressource" << std::endl;
}

seekable_ressource::seekable_ressource( const seekable_ressource & copy )
{
	file_object_ = copy.file_object_;
}

seekable_ressource::~seekable_ressource()
{
	std::cout << "seekable_ressource::~seekable_ressource" << std::endl;
	//close_file( handle_ ); todo: iostream lib calls the dcon several times? why? wait for re from author
}

std::streamsize seekable_ressource::read( char * s, std::streamsize n ) 
{
	std::cout << "seekable_ressource::read(buffer, " << n << ")" << std::endl;
  // Reads up to n characters from the controlled 
  // sequence into the buffer s, returning the number 
  // of characters read. Returning a value less than n 
  // indicates end-of-sequence.			
  return read_file( file_object_->handle_, reinterpret_cast<unsigned char*>( s ), n );
}
 
void seekable_ressource::write( const char * s, std::streamsize n ) 
{
  // Writes n characters from the buffer s to the 
  // controlled sequence.
  throw exception();
}
 
std::streamoff seekable_ressource::seek( std::streamoff off, std::ios_base::seekdir way ) 
{
	std::cout << "seekable_ressource::seek(" << off << "," << way << ")" << std::endl;
  // Advances the read/write head by off characters, 
  // returning the new position, where the offset is 
  // calculated from:
  //  - the start of the sequence if way == ios_base::beg
  //  - the current position if way == ios_base::cur
  //  - the end of the sequence if way == ios_base::end
  return seek_file( file_object_->handle_, off, way );
}

} // virtualfs
} // filesystem