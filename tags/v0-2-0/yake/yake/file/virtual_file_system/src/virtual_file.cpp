#include <inc/virtual_file.hpp>
#include <inc/virtual_archive.hpp>
#include <inc/virtual_file_system.hpp>
#include <virtualfs/inc/operations.hpp>

namespace filesystem
{
namespace virtualfs
{

virtual_file::virtual_file( const virtual_archive & ar )
	: archive_( const_cast<virtual_archive&>(ar) ), data_(), 
	is_loaded_(false), 
	compressed_size_(0), 
	uncompressed_size_(0),
	data_position_(0)
{

}

virtual_file::virtual_file( const path & ph, const virtual_archive & ar )
	: archive_( const_cast<virtual_archive&>(ar) ), 
	file_name_( ph.leaf() ),
	data_(), 
	is_loaded_(false), 
	compressed_size_(0), 
	uncompressed_size_(0),
	data_position_(0)
{

}

virtual_file::virtual_file( const path & ph, std::ios_base::openmode mode, 
	virtual_archive & ar )
	: file_name_( ph.string() ), 
	archive_(ar), 
	data_(), 
	data_position_(0),
	is_loaded_(false)
{

}

virtual_file::~virtual_file()
{
  // still loaded?
	if( is_loaded_ )
		close();
}

void virtual_file::load_data_from_stream( in_archive * iarchive )
{
	if( is_loaded_ ) throw exception();

	// set position to data offset
	iarchive->is.seekg( archive_.get_data_offset().operator +( data_offset_ ), std::ios_base::beg );

	// deserialize data size
	int data_size;
	iarchive->operator>> ( data_size );

	// deserialize data buffer
	data_.resize( data_size );
	iarchive->load_binary( &( data_[0] ), data_size );

	/*char * buffer = new char[ data_size ];
	iarchive->load_binary( buffer, data_size );

	for( int i = 0; i < data_size; i++ )
	{
		std::cout << buffer[i];
	}
	std::cout << "END";*/

	is_loaded_ = true;
}

void virtual_file::load_data_from_file( const path & ph )
{
	if( is_loaded_ ) throw exception();

	// set own file name
  file_name_ = ph.leaf();

	// read file
	static byte chunk[ chunk_size ];
	handle source_file = open_file( ph );
	dword read;

	do
	{
    read = read_file( source_file, chunk, chunk_size );
    data_.reserve( data_.size() + read );
		for( dword index = 0; index < read; index++ )
			data_.push_back( chunk[ index ] );
	}
	while( read > 0 );

	close_file( source_file );
	is_loaded_ = true;
}

void virtual_file::set_data_offset( dword offset )
{
	data_offset_ = offset;
}

dword virtual_file::get_data_size()
{
  return data_.size();
}


const std::string & virtual_file::get_name() const
{
   return file_name_;
}

const virtual_archive & virtual_file::get_archive() const
{
   return archive_;
}

void virtual_file::close()
{
	// not loaded?
  if( !is_loaded_ )
		throw( 
			filesystem_error( "virtual_file::close",
				other_error ) );

	// unload
	data_.clear();
	data_position_ = 0;

	// data is not loaded
	is_loaded_ = false;
}

const file_system & virtual_file::get_file_system() const
{
	return virtual_file_system::instance();
}

int virtual_file::read( unsigned char * buffer, dword bytes_to_read )
{
	if( !is_loaded_ ) throw exception();

	// eof?
	if( data_position_ == data_.size() )
		return 0;

	// still enough bytes?
  if( ( data_position_ + bytes_to_read ) > data_.size() )
		bytes_to_read = data_.size() - data_position_;

	// copy data
  memcpy( buffer, &( data_[0] ), bytes_to_read );

	// move position
	data_position_ += bytes_to_read;

	// return read bytes
	return bytes_to_read;
}

std::streamoff virtual_file::seek( std::streamoff off, std::ios_base::seekdir way )
{
  // Advances the read/write head by off characters, 
  // returning the new position, where the offset is 
  // calculated from:
  //  - the start of the sequence if way == ios_base::beg
  //  - the current position if way == ios_base::cur
  //  - the end of the sequence if way == ios_base::end
  if( way == std::ios_base::beg )
  {
    data_position_ = static_cast<dword>( off ); 
  }
  else if( way == std::ios_base::cur )
  {
    data_position_ += static_cast<dword>( off );           
  }
  else if( way == std::ios_base::end )
  {
    data_position_ = data_.size() - static_cast<dword>( off ); 
  }
  return data_position_;
}

int virtual_file::read_complete_file( unsigned char * buffer, dword bytes_to_read )
{
	// todo
	return 0;
}

} // virtualfs
} // filesystem