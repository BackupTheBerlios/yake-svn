#ifndef _VIRTUALFS_FILESYSTEM_HPP_
#define _VIRTUALFS_FILESYSTEM_HPP_

#include <string>

#include "common/inc/path.hpp"

#include <yake/base/templates/yakeRegistry.h>
#include <yake/base/yakeString.h>

#include "config.hpp"
#include "file.hpp"


namespace filesystem
{
namespace virtualfs
{

struct file;

struct YAKE_VIRTUALFS_API file_system
{
public:
	virtual ~file_system();
	
public:
	/*enum resolving_type
	{
		registry_resolving,
    implementation_resolving
	};

	virtual resolving_type get_resolving_type() const = 0;*/

	// open a file
	virtual handle open( const path & ph, std::ios_base::openmode mode ) = 0;

	// close the file
	virtual void close( file & this_file ) = 0;

	// is the directory or file available
	virtual bool exists( const path & ph ) = 0;

	// create virtual file system from native directories
	virtual void create_archive_from( const path & archive, const path & dir ) = 0;



  virtual void create_directory( const path & ph ) = 0;

  virtual bool remove( const path & ph ) = 0;
  virtual unsigned long remove_all( const path & ph ) = 0;

  virtual void rename( const path & from_ph, const path & to_ph ) = 0;

  virtual void copy_file( const path & from_file_ph, const path & to_file_ph ) = 0;

	// iteration.
	virtual const char * find_first_file( const char * dir, YAKE_HANDLE & hdl, YAKE_SYSTEM_DIRECTORY_TYPE & data ) = 0;
  virtual void find_close( YAKE_HANDLE hdl ) = 0;
  virtual const char * find_next_file( YAKE_HANDLE hdl, const path & ph, YAKE_SYSTEM_DIRECTORY_TYPE & data ) = 0;

	YAKE_DECLARE_REGISTRY_0( filesystem::virtualfs::file_system, yake::base::String )
};

typedef yake::base::templates::Pointer<file_system> file_system_ptr;

} // virtualfs
} // filesystem

#endif // _VIRTUALFS_FILESYSTEM_HPP_