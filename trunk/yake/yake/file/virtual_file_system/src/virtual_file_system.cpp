#include <inc/virtual_file_system.hpp>
#include <inc/virtual_file_system_register.hpp>

#include <nativefs/inc/ofstream.hpp>
#include <nativefs/inc/ifstream.hpp>
#include <virtualfs/inc/ifstream.hpp>
#include <virtualfs/inc/operations.hpp>

#include <string>
#include <iostream>
#include <algorithm>

#include <boost/utility.hpp>

namespace filesystem
{
namespace virtualfs
{

virtual_file_system::virtual_file_system()
{
}

virtual_file_system::~virtual_file_system()
{
	for( open_archives::iterator iter( open_archives_.begin() );
		iter != open_archives_.end();
		iter++ )
	{
		delete iter->second;
	}
}

void virtual_file_system::create_directory( const path & ph )
{
}

void virtual_file_system::close( file & this_file )
{
	// we don't delete the file because the 
	// virtual_archive cares about this
  this_file.close();
}

// we don't need to check whether the file exists or not since the
// resolving approach already used file_system::exist
handle virtual_file_system::open( const path & ph, std::ios_base::openmode mode )
{ std::cout << "virtual_file_system::open " << ph.string() << std::endl;
	// todo check mode read only

	// get archive and the load file
	virtual_archive & ar = get_opened_archive( ph );
	virtual_file & file = ar.load_file_from_stream( 
		construct_file_name( ph.string(), ar ) );
	return static_cast< handle >( 
		reinterpret_cast< dword >( &file ) );
}

std::string virtual_file_system::construct_file_name( 
	const std::string & ph, const virtual_archive & ar )
{
	// erase everything until we are "inside" the archive
	std::string file_name( ph );
	// we dont wont the archive file and the following separator
	file_name.erase( 0, file_name.find( ar.get_archive_name() ) + ar.get_archive_name().length() + 1 );
	return file_name;
}

virtual_archive & virtual_file_system::get_opened_archive( const path & ph )
{
	// no root paths defined and no absolute file name?
	if( get_num_root_paths() == 0 && !ph.has_root_path() )
		throw filesystem_error( "virtual_file_system::get_opened_archive",
			ph, not_found_error );

	// absolute?
	if( ph.has_root_path() )
	{
		// todo    
	}

	// for each root path
	for( root_path_list::const_iterator iter_root = get_root_paths().begin(); 
		iter_root != get_root_paths().end(); 
		iter_root++ )
	{
		// todo cleanup
		if( iter_root->first.find( "ftp" ) == string::npos )
		{

			// root + ph
			path absolute_path( iter_root->first, native );
			absolute_path /= ph; // ph has no native name check

			// iter through each part of the path and check if it's the archive file
			path partial_absolut_path( *absolute_path.begin(), native );
			for( path::iterator iter( absolute_path.begin() ); 
				iter != absolute_path.end();
				iter++, partial_absolut_path /= *iter )
			{
				// normalize
				std::string partial_absolut_path_str( partial_absolut_path.string() );
				std::transform( partial_absolut_path_str.begin(), partial_absolut_path_str.end(), 
					partial_absolut_path_str.begin(), tolower );

				// search
				open_archives::iterator iter_archive = open_archives_.find( partial_absolut_path_str + "." + archive_id );
				if( iter_archive != open_archives_.end() )
					return *iter_archive->second;
			}

		}
		else // ftp
		{
			// iter through each part of the path and check if it's the archive file
			path partial_absolut_path( *ph.begin(), no_check ); // todo: see above
			for( path::iterator iter( ph.begin() ); 
				iter != ph.end();
				iter++, partial_absolut_path /= *iter )
			{
				// normalize
				std::string partial_absolut_path_str( partial_absolut_path.string() );
				std::transform( partial_absolut_path_str.begin(), partial_absolut_path_str.end(), 
					partial_absolut_path_str.begin(), tolower );

				// search ... todo stored root path looks like "ftp:/blah" ... fix path with shared name check? => change append because name check is not used => no_check
				open_archives::iterator iter_archive = open_archives_.find( path( iter_root->first, no_check ).string() + "/" + partial_absolut_path_str + "." + archive_id );
				if( iter_archive != open_archives_.end() )
					return *iter_archive->second;
			}
		}


	}

	// unknown
	throw filesystem_error( "virtual_file_system::get_opened_archive",
		ph, not_found_error );  
}

virtual_archive & virtual_file_system::get_or_open_archive( const path & absolute_archive_path )
{
	std::string archive_string( absolute_archive_path.string() );
	std::transform( archive_string.begin(), archive_string.end(), archive_string.begin(), tolower );

	// open the archive if it isn't open yet
	open_archives::iterator iter = open_archives_.find( archive_string );
	if( iter != open_archives_.end() )
	{	// Return archive
		return *iter->second;
	}
	else
	{	
		// deserialize archive

		// todo clean up, we need the clear, otherwise nfs would be exluded because we are still in the resolve_fs_for loop
		resolve::instance().clear_exlusion();

	virtualfs::istream * in = new virtualfs::ifstream( absolute_archive_path );
	// todo: if we don't use no_codecvt, boost's archive will throw an exception while
	// trying to imbue the stream with a new locale respectively with a facet
	in_archive * ia = new in_archive( *in, boost::archive::no_codecvt );

		virtual_archive * archive = new virtual_archive( ia, archive_string );
		ia->operator >> ( *archive );

		// store
		open_archives_[ archive_string ] = archive;
		return *archive;
	}
}

bool virtual_file_system::exists_path( const path & ph )
{
  path current( *ph.begin(), native );
	for( path::iterator iter( ph.begin() ); 
		iter != ph.end();
		iter++, current /= *iter )
	{
		// todo: use vfs exists so that we can use ftp
		if( nativefs::exists( current ) ) continue;
		path absolute_path( current.string() + "." + archive_id, native );
		if( nativefs::exists( absolute_path ) )
		{
			// get or open the archive
			virtual_archive & archive = get_or_open_archive( absolute_path );

			// is the given path the archive directory? this is need by iteration
			if( iter == boost::prior( ph.end() ) ) return true;

			// check if it contains such a file
			std::string filename( ph.string() );
			// cut archive path, we dont want the '/'
			filename.erase( 0, current.string().length() + 1 );
			if( archive.contains_file( filename ) )	return true;
		}
	}
	return false;
}

// todo clean up, refactor
bool virtual_file_system::exists( const path & ph )
{ std::cout << "virtual_file_system::exists " << ph.string() << std::endl;

// no root paths defined and no absolute file name?
 	if( get_num_root_paths() == 0 && !ph.has_root_path() )
		return false;

	// todo cleanup
	if( ph.string().find( "ftp:/" ) != string::npos ) return false;

	// absolute?
	if( ph.has_root_path() )
		return exists_path( ph );

	// for each root path
	for( root_path_list::const_iterator iter = get_root_paths().begin(); 
		iter != get_root_paths().end(); 
		iter++ )
	{
    // shared todo
		if( iter->second == shared_path )
		{
			// we need at least archive/myfile to identify the archive
			if( !ph.has_branch_path() ) continue;

			// check each part for archive file
			path current( *ph.begin(), native );
			for( path::iterator iter_ph( ph.begin() );
				iter_ph != ph.end();
				iter_ph++, current /= *iter_ph )
			{
				// exists the archive on the remote machine?
				path remote_path( path( iter->first + "/" + current.string() + "." + archive_id, no_check ) );
				if( virtualfs::exists( remote_path ) )
				{
          // get or open the archive
					virtual_archive & archive = get_or_open_archive( remote_path );

          // todo: is the given path the archive directory? this is needed by iteration
					// if( iter == boost::prior( ph.end() ) ) return true;

					// todo check if it contains such a file
					//std::string filename( ph.string() );

					// todo cut archive path, we dont want the '/'
					//filename.erase( 0, current.string().length() + 1 );

					if( archive.contains_file( ph.leaf() ) )	{ std::cout << "== true" << std::endl;	return true; }
				}					
			}
		}
		else
		{
			// root + ph
			path absolute_path( iter->first, native );
			absolute_path /= ph; // ph has no native name check
			if( exists_path( absolute_path ) ) { std::cout << "== true" << std::endl;	return true; }
		}
	}

	// unknown
	return false;
}

void virtual_file_system::create_archive_from( const path & archive, const path & dir )
{
	// construct the archive and file relations
  virtual_archive ar;

	// add each entity
	virtualfs::directory_iterator end_itr;
	for( virtualfs::directory_iterator iter( dir );
		iter !=  end_itr;
		++iter )
	{
    ar.add_virtual_file_from( *iter );
	}
	
	// serialize
	nativefs::ofstream out( archive ); // todo we should use vfs::ofstream here, so that we can use ftp etc.
	out_archive oa( out );// std::cout );
	oa << ar;
	out.close();

	/*{
	nativefs::ofstream out( "test.vfs" );
	boost::archive::text_oarchive oa( out );
	char * buffer = new char[7];
	for( int i = 0; i < 7; i++ )
	{
		buffer[i] = 'a';
	}

	oa.save_binary( buffer, 7 );
	out.close();
	}
	{
	nativefs::ifstream in( "test.vfs" );
	boost::archive::text_iarchive ia( in );
	char * buffer = new char[7];
	ia.load_binary( buffer, 7 );
	in.close();

	std::cout << buffer[0];
	}*/
}

bool virtual_file_system::remove( const path & ph )
{
  throw( filesystem_error(
    "virtual_file_system::remove",
		read_only_error ) );
}

unsigned long virtual_file_system::remove_all( const path & ph )
{
  throw( filesystem_error( 
		"virtual_file_system::remove_all",
			read_only_error ) );
}

void virtual_file_system::rename( const path & from_path, const path & to_path )
{
  throw( filesystem_error( 
		"virtual_file_system::rename",
		read_only_error ) );
}

void virtual_file_system::copy_file( const path & from_file_ph, const path & to_file_ph )
{
		// todo
}

const char * virtual_file_system::find_first_file( 
	const char * dir, 
	YAKE_HANDLE & hdl, 
	YAKE_SYSTEM_DIRECTORY_TYPE & data )
{
	virtual_archive & ar = get_opened_archive( path( dir, no_check ) );
	file_iterator_ = ar.begin();
	file_iterator_end_ = ar.end();
	return file_iterator_->first.c_str();
}

void virtual_file_system::find_close( YAKE_HANDLE hdl )
{
}

const char * virtual_file_system::find_next_file( 
	YAKE_HANDLE hdl, 
	const path & ph, 
	YAKE_SYSTEM_DIRECTORY_TYPE & data )
{
	return ( ++file_iterator_ != file_iterator_end_ ) ? file_iterator_->first.c_str() : 0;
}

} // virtualfs
} // filesystem