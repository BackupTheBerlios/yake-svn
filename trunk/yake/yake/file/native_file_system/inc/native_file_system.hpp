#ifndef _NATIVE_FILE_SYSTEM_HPP_
#define _NATIVE_FILE_SYSTEM_HPP_

#include <yake/base/templates/yakeRegistry.h>
#include <yake/base/templates/yakeSingleton.h>

#include <common/inc/exception.hpp>
#include <nativefs/inc/operations.hpp>
#include <virtualfs/inc/file_system.hpp>
#include <virtualfs/inc/config.hpp>

#include "config.hpp"
#include "native_file.hpp"

namespace filesystem
{
namespace virtualfs
{

class YAKE_NFS_API native_file_system : public file_system
{
public:
	native_file_system();

public:
	handle open( const path & ph, std::ios_base::openmode mode )
	{
		return static_cast< handle >( reinterpret_cast< dword >( 
			new native_file( ph, mode ) ) ); 
 
	}

	void close( file & this_file )
	{
    this_file.close();
		delete &this_file;
	}

	/*resolving_type get_resolving_type() const
	{
    return implementation_resolving;
	};*/

	/*void flush( Handle _hFile )
	{
    VFS_Archive_Flush();
	}*/
	// ------------------------------------------------------------//

	// todo: Allocate buffer of size n, get char, check for bounds
	/*int getc( Handle _hFile );
	int ungetc( const unsigned char&, Handle _hFile );

	// ------------------------------------------------------------//
	void seek( Handle _hFile, long, SeekOrigin = SET );
	long tell( Handle _hFile );
	int getpos( Handle _hFile, fpos_t* );
	int setpos( Handle _hFile, fpos_t* );*/


	bool exists( const path & ph );

	void create_archive_from( const path & archive, const path & dir );

  void create_directory( const path & ph );

  bool remove( const path & ph );
  unsigned long remove_all( const path & ph );

  void rename( const path & from_path, const path & to_path );

  void copy_file( const path & from_file_ph, const path & to_file_ph );

	const char * find_first_file( const char * dir, YAKE_HANDLE & hdl, YAKE_SYSTEM_DIRECTORY_TYPE & data );
  void find_close( YAKE_HANDLE hdl );
	const char * find_next_file( YAKE_HANDLE hdl, const path & ph, YAKE_SYSTEM_DIRECTORY_TYPE & data );

YAKE_BUILD_SINGLETON( native_file_system )
YAKE_DECLARE_CONCRETE( native_file_system, archive_id )
};

} // virtualfs
} // filesystem

#endif // _NATIVE_FILE_SYSTEM_HPP_