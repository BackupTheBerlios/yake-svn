#ifndef _VIRTUAL_FILE_SYSTEM_HPP_
#define _VIRTUAL_FILE_SYSTEM_HPP_

#include <yake/base/templates/yakeSingleton.h>

#include <common/inc/exception.hpp>
#include <nativefs/inc/operations.hpp>
#include <virtualfs/inc/file_system.hpp>
#include <virtualfs/inc/root.hpp>

#include "virtual_archive.hpp"
#include "config.hpp"

namespace filesystem
{
namespace virtualfs
{

class YAKE_VFS_API virtual_file_system : public file_system
{
	typedef std::map< std::string, virtual_archive* > open_archives;

public:
	virtual_file_system();
	~virtual_file_system();

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
	std::string construct_file_name( const std::string & ph, const virtual_archive & ar );
	virtual_archive & get_opened_archive( const path & ph );
	virtual_archive & get_or_open_archive(  const path & absolute_archive_path );
	bool exists_path( const path & ph );

private:
	open_archives open_archives_;
	virtual_archive::file_iterator file_iterator_;
	virtual_archive::file_iterator file_iterator_end_;

YAKE_BUILD_SINGLETON( virtual_file_system )
YAKE_DECLARE_CONCRETE( virtual_file_system, archive_id )
};

} // virtualfs
} // filesystem

#endif // _VIRTUAL_FILE_SYSTEM_HPP_