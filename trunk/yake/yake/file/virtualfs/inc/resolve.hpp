#ifndef _VIRTUALFS_RESOLVE_
#define _VIRTUALFS_RESOLVE_

#include "config.hpp"
#include "filebuf.hpp"
#include "root.hpp"

#include <nativefs/inc/filebuf.hpp>
#include <nativefs/inc/operations.hpp>

#include <yake/base/templates/yakePointer.h>
#include <yake/base/templates/yakeSingleton.h>

//#include <boost/tuple/tuple.hpp>

namespace filesystem
{
namespace virtualfs
{

struct file_system;

struct YAKE_VIRTUALFS_API resolve
{
	/*typedef yake::base::templates::Pointer<file_system> file_system_ptr;
	typedef boost::tuple< file_system::resolving_type, std::string, file_system_ptr > system_entry;
	typedef std::vector< system_entry > system_list;*/

	typedef std::pair< std::string, file_system_ptr > system_entry;
	typedef std::vector< system_entry > system_list;

public:
	resolve()
	{
		// add application path
		add_root_path( nativefs::initial_path() );
	}

	~resolve()
	{
	}

public:
	// find the fs for a specific path
	file_system_ptr resolve_fs_for( const path & ph );

	// find the fs for an archive ending used for creating archive files
	file_system_ptr resolve_fs_for_extension( const std::string & ph );

	// check whether a fs for a specific path exists or not
	//bool exists_fs_for( const path & ph );

public:
	// load file system from registry
	void load_fs( const std::string & identifier )
	{
		// find creator
		for( file_system::RegistryType::iterator iter( file_system::getRegistry().begin() );
			iter != file_system::getRegistry().end();
			iter++ )
		{
			// create
			if( iter->first == identifier )
			{
				system_list_.push_back( system_entry( iter->first, iter->second->create() ) );
				return;
			}
		}
		// unknown
		throw exception();
	}

	// load using instance (used by singleton file system impls)
	void load_fs( const std::string & identifier, const file_system_ptr system )
	{
		// find creator
		for( file_system::RegistryType::iterator iter( file_system::getRegistry().begin() );
			iter != file_system::getRegistry().end();
			iter++ )
		{
			// create
			if( iter->first == identifier )
			{
				system_list_.push_back( system_entry( iter->first, system ) );
				return;
			}
		}
		// unknown
		throw exception();
	}

	void unload_fs( const std::string & identifier )
	{
		// destroy list of file systems
    for( system_list::iterator iter_sys( system_list_.begin() );
			iter_sys != system_list_.end();
			iter_sys++ )
		{
			// unload shared ptr
			if( iter_sys->first == identifier ) 
        return iter_sys->second.reset();
		}
		// unknown
		throw exception();
	}

	//file_system_ptr resolve_fs( const path & ph );

private:
	system_list system_list_;

YAKE_BUILD_SINGLETON( resolve )
};

} // virtualfs
} // filesystem


#endif