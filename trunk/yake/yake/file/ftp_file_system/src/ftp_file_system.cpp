#include <virtualfs/inc/operations.hpp>
#include <virtualfs/inc/root.hpp>

#include <inc/ftp_file_system.hpp>
#include <inc/ftp_file_system_register.hpp>

#include <string>
#include <iostream>
#include <algorithm>

namespace filesystem
{
namespace virtualfs
{

ftp_file_system::ftp_file_system()
 : cache_dir_( ( nativefs::initial_path() / ftp_cache ).string(), native )
{
	// create cache dirctory
	if( virtualfs::exists( cache_dir_ ) )
		virtualfs::remove_all( cache_dir_ );
	virtualfs::create_directory( cache_dir_ );
}

ftp_file_system::~ftp_file_system()
{
	// all files should be deleted using close()
	for( open_files::iterator iter( open_files_.begin() );
		iter != open_files_.end();
		iter++ )
	{
		delete iter->second;
	}

	// remove cache directory
	virtualfs::remove_all( nativefs::initial_path() / ftp_cache );
}

void ftp_file_system::create_directory( const path & ph )
{
}

// we don't need to check whether the file exists or not since the
// resolving approach already used file_system::exist
handle ftp_file_system::open( const path & ph, std::ios_base::openmode mode )
{ std::cout << "ftp_file_system::open " << ph.string() << std::endl;
	// todo check mode read only

	// todo create needed cache dirs
	// todo check if ph has a shared root path and delete it
	//if( ph.has_branch_path() )
	//	nativefs::create_directory( cache_dir_ / ph.branch_path() );

	// download file
	std::string local_file( ( cache_dir_ / ph.leaf() ).string() );

	// the ftp has a current work directory, so we have to use the leaf
	ftp_client_.get_file( ph.leaf(), local_file );

	// open file
	ftp_file * file = new ftp_file( local_file );
	open_files_.insert( open_files::value_type( local_file, file ) );

	// return new ftp file
	return static_cast< handle >( 
		reinterpret_cast< dword >( file ) );
}

void ftp_file_system::close( file & this_file )
{
	// close, remove from map and delete file 
  this_file.close();
	open_files_.erase( this_file.get_name() );
	delete &this_file;
}

// todo cleanup
bool ftp_file_system::exists( const path & ph )
{ std::cout << "ftp_file_system::exists " << ph.string() << std::endl;
	// no root paths defined and no absolute file name?
 	if( get_num_root_paths() == 0 && !ph.has_root_path() )
		return false;

	// absolute?
	if( ph.string().find( "ftp:/" ) != string::npos ) // todo has_root_path() does not work for ftp:/
	{
		// for each root path
		for( root_path_list::const_iterator iter = get_root_paths().begin(); 
			iter != get_root_paths().end(); 
			iter++ )
		{
			// find root string within absolute path
			if( ph.string().find( "ftp:/myfriend:myfriend@machine1.myip.us/yake_ftp" ) != string::npos
				&& ph.string().find( "ftp:/myfriend:myfriend@machine1.myip.us/yake_ftp.vfs" ) == string::npos
				&& ph.string().find( "ftp:/myfriend:myfriend@machine1.myip.us/yake_ftp/ftp_cheeze.vfs" ) == string::npos )//path( iter->first, no_check ).string() ) != string::npos ) // todo path converts ftp:// to ftp:/
				{ std::cout << "== true" << std::endl;	return true; }; // todo further checks and combine this code with that one below        
		}
	}
	else
	{
		// for each root path
		for( root_path_list::const_iterator iter = get_root_paths().begin(); 
			iter != get_root_paths().end(); 
			iter++ )
		{
			if( iter->second == shared_path )
			{
				// connect to ftp
				if( !ftp_client_.is_connected() )
					ftp_client_.connect( iter->first );

					// do we have to change the working dir?
					if( ph.has_branch_path() ) 
					{
						ftp_client_.change_dir( ph.branch_path().string() );
						if( ftp_client_.exists( ph.leaf() ) )	{ std::cout << "== true" << std::endl;	return true; }
					}
					else
					{
						ftp_client_.change_to_root();
						if( ftp_client_.exists( ph.string() ) )	{ std::cout << "== true" << std::endl;	return true; }
					}
			}
		}
	}

	// unknown
	return false;
}

void ftp_file_system::create_archive_from( const path & archive, const path & dir )
{

}

bool ftp_file_system::remove( const path & ph )
{
  throw( filesystem_error(
    "ftp_file_system::remove",
			read_only_error ) );
	return 0;
}

unsigned long ftp_file_system::remove_all( const path & ph )
{
  throw( filesystem_error( 
		"ftp_file_system::remove_all",
			read_only_error ) );
	return 0;
}

void ftp_file_system::rename( const path & from_path, const path & to_path )
{
  throw( filesystem_error( 
		"ftp_file_system::rename",
			read_only_error ) );
}

void ftp_file_system::copy_file( const path & from_file_ph, const path & to_file_ph )
{
		// todo
}

const char * ftp_file_system::find_first_file( 
	const char * dir, 
	YAKE_HANDLE & hdl, 
	YAKE_SYSTEM_DIRECTORY_TYPE & data )
{
	ftp_client_.list_files( dir, ftp_files_ );
	assert( ftp_files_.size() > 0 );
	return ( file_iterator_ = ftp_files_.begin() )->c_str();
}

void ftp_file_system::find_close( YAKE_HANDLE hdl )
{
	ftp_files_.clear();
}

const char * ftp_file_system::find_next_file( 
	YAKE_HANDLE hdl, 
	const path & ph, 
	YAKE_SYSTEM_DIRECTORY_TYPE & data )
{
	return ( ++file_iterator_ != ftp_files_.end() ) ? file_iterator_->c_str() : 0;
}

} // virtualfs
} // filesystem