#ifndef _FTP_FILE_SYSTEM_HPP_
#define _FTP_FILE_SYSTEM_HPP_

#include <yake/base/templates/yakeRegistry.h>
#include <yake/base/templates/yakeSingleton.h>

#include <common/inc/exception.hpp>
#include <virtualfs/inc/file_system.hpp>

#include <ftp_wrapper/inc/ftp_client.hpp>

#include "config.hpp"
#include "ftp_file.hpp"

namespace filesystem
{
namespace virtualfs
{

class YAKE_FTP_API ftp_file_system : public file_system
{
	typedef std::map< std::string, ftp_file* > open_files;

public:
	ftp_file_system();
	~ftp_file_system();

public:
	handle open( const path & ph, std::ios_base::openmode mode );
	void close( file & this_file );

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

private:
	// data
	open_files open_files_;
	ftp_client ftp_client_;
	path cache_dir_;
	// iterators
	file_list ftp_files_;
	file_list::iterator file_iterator_;

YAKE_BUILD_SINGLETON( ftp_file_system )
YAKE_DECLARE_CONCRETE( ftp_file_system, archive_id )
};

} // virtualfs
} // filesystem

#endif // _FTP_FILE_SYSTEM_HPP_
