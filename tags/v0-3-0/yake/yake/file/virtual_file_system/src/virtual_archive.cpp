#include <inc/virtual_archive.hpp>

namespace filesystem
{
namespace virtualfs
{

const std::string & virtual_archive::get_file_name() const
{
   return file_name_;
}

const std::string & virtual_archive::get_archive_name() const
{
   return archive_name_;
}

bool virtual_archive::contains_file( const std::string & file_name )
{
  return files_.find( file_name ) != files_.end();
}

void virtual_archive::add_virtual_file_from( const path & ph )
{
	static dword current_offset = 0;
	// load file data
	virtual_file * file = new virtual_file( *this );
	file->load_data_from_file( ph );
	// set offset
	file->set_data_offset( current_offset );
	current_offset += file->get_data_size();
	// add file
	files_.insert( archive_file_map::value_type( file->get_name(), file ) );
}

virtual_file & virtual_archive::load_file_from_stream( const std::string & file_name )
{
	// exists?
	if( files_.find( file_name ) == files_.end() ) throw exception();
	// load file data
	virtual_file & file = *files_[ file_name ];
	file.load_data_from_stream( archive_ );
	// return loaded file
	return file;	
}

} // virtualfs
} // filesystem