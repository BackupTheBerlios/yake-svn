#include <inc/config.hpp>
#include <inc/operations.hpp>
#include <common/inc/exception.hpp>

#include <boost/scoped_array.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/utility.hpp>

#include <cstring>
#include <iostream>


#ifdef BOOST_NO_STDC_NAMESPACE
namespace std { using ::strcmp; using ::remove; using ::rename; }
#endif

namespace fs  = filesystem;

# if defined(YAKE_WINDOWS)
#   include "windows.h"
#   if defined(__BORLANDC__) || defined(__MWERKS__)
#     if defined(__BORLANDC__)
        using std::time_t;
#     endif
#     include "utime.h"
#   else
#     include "sys/utime.h"
#   endif

// For Windows, the xxxA form of various function names is used to avoid
// inadvertently getting wide forms of the functions. (The undecorated
// forms are actually macros, so can misfire if the user has various
// other macros defined. There was a bug report of this happening.)

# else
#   include "dirent.h"
#   include "unistd.h"
#   include "fcntl.h"
#   include "utime.h"
# endif

#include <sys/stat.h>  // last_write_time() uses stat()
#include <string>
#include <cstdio>      // for remove, rename
#include <cerrno>
#include <cassert>

namespace filesystem
{
namespace virtualfs
{

	// todo style

// operations ---------------------------------------------------------//
bool exists( const path & ph )
{	
	std::cout << "===>ops:exists " << ph.string() << std::endl;
	// check whether entity exists or not
	if( nativefs::exists( ph ) )
		return true;

	file_system_ptr system( resolve::instance().resolve_fs_for( ph ) );
	return system.get() != NULL;

	//return resolve::instance().exists_fs_for( ph ) && 
	//	resolve::instance().resolve_fs_for( ph )->exists( ph );
}

void create_directory( const path & ph )
{ 
		std::cout << "===>ops:create_directory" << std::endl;
	// create a new native or virtual directory
	resolve::instance().resolve_fs_for( ph.branch_path() )->create_directory( ph );
}

bool remove( const path & ph )
{ // remove a new native or virtual directory or file

	std::cout << "===>ops:remove" << std::endl;

	if( nativefs::exists( ph ) )
		return nativefs::remove( ph );

	return resolve::instance().resolve_fs_for( ph )->remove( ph );
}

unsigned long remove_all( const path & ph )
{
		std::cout << "===>ops:remove_all" << std::endl;

	if( nativefs::exists( ph ) )
		return nativefs::remove_all( ph );

	return resolve::instance().resolve_fs_for( ph )->remove_all( ph );
}

void rename( const path & from_path, const path & to_path ) // todo names
{ // rename a new native or virtual directory or file

	if( nativefs::exists( from_path ) )
		return nativefs::rename( from_path, to_path );

	resolve::instance().resolve_fs_for( from_path )->rename( from_path, to_path );
}

void copy_file( const path& _rFrom_File, const path& _rTo_File )
{ // copy a native or virtual file
	resolve::instance().resolve_fs_for( _rTo_File )->copy_file( _rFrom_File, _rTo_File );
}

// misc operations ---------------------------------------------------------//

path current_path() { return path( "todo" ); }
const path & initial_path() { return path( "todo" ); }

// file operations ---------------------------------------------------------//

// todo operations are just for vfs now, we need a vfs with native c file api
handle open_file( const path & ph, const std::ios_base::openmode mode )
{ 
	std::cout << "===>ops:open_file " << ph.string() << std::endl;
	// open file system 
	return resolve::instance().resolve_fs_for( ph )->open( ph, mode ); 
};

int read_file( handle this_file, unsigned char* buffer, dword bytes_to_read )
{ 
	std::cout << "===>ops:read_file" << std::endl;
	// read from virtual file
	return ( (file*) (dword) this_file )->read( buffer, bytes_to_read );
};

void close_file( handle this_file )
{ // close virtual file
	file * my_file = (file*) (dword) this_file;
	const_cast<file_system&>( my_file->get_file_system() ).close( *my_file );
}

// archive operations ---------------------------------------------------------//
void create_archive_from( const path & archive, const path & dir )
{ // create archive using a virtual file system
	std::string ph( archive.string() );
	ph.erase( 0, ph.find( "." ) + 1 );
	return resolve::instance().resolve_fs_for_extension( 
		ph )->create_archive_from( archive, dir );     
}


//  directory_iterator helpers  ----------------------------------------------//

yake::base::templates::Pointer<file_system> current_fs;

  inline const char *  find_first_file( const char * dir,
    YAKE_HANDLE & handle, YAKE_SYSTEM_DIRECTORY_TYPE & data )
  // Returns: 0 if error, otherwise name
  {
		current_fs = resolve::instance().resolve_fs_for( path( dir ) );
		return current_fs->find_first_file( dir, handle, data );
  }  

  inline void find_close( YAKE_HANDLE handle )
  {
		// close search
		current_fs->find_close( handle );
		// unload
		current_fs.reset();
  }

  inline const char * find_next_file(
	  YAKE_HANDLE handle, const filesystem::path & ph,
    YAKE_SYSTEM_DIRECTORY_TYPE & data )
  // Returns: 0 if EOF, otherwise name
  // Throws: if system reports error
  {
		return current_fs->find_next_file( handle, ph, data );
  }


    namespace detail
    {

//  dir_itr_imp  -------------------------------------------------------------// 

      class dir_itr_imp
      {
      public:
        path              entry_path;
        YAKE_HANDLE      handle;

        ~dir_itr_imp()
        {
          if ( handle != YAKE_INVALID_HANDLE_VALUE ) find_close( handle );
        }
      };

//  dot_or_dot_dot  ----------------------------------------------------------//

      inline bool dot_or_dot_dot( const char * name )
      {
# if !BOOST_WORKAROUND( __BORLANDC__, BOOST_TESTED_AT(0x0564) )
        return std::strcmp( name, "." ) == 0
            || std::strcmp( name, ".." ) == 0;
# else
        // Borland workaround for failure of intrinsics to be placed in
        // namespace std with certain combinations of compiler options.
        // To ensure test coverage, the workaround is applied to all
        // configurations, regardless of option settings.
        return name[0]=='.'
          && (name[1]=='\0' || (name[1]=='.' && name[2]=='\0'));
# endif
      }

//  directory_iterator implementation  ---------------------------------------//

      void dir_itr_init( dir_itr_imp_ptr & m_imp,
                                               const path & dir_path )
      {
        m_imp.reset( new dir_itr_imp );
        YAKE_SYSTEM_DIRECTORY_TYPE scratch;
        const char * name = 0;  // initialization quiets compiler warnings
        if ( dir_path.empty() )
        m_imp->handle = YAKE_INVALID_HANDLE_VALUE;
        else
          name = find_first_file( dir_path.native_directory_string().c_str(),
            m_imp->handle, scratch );  // sets handle

        if ( m_imp->handle != YAKE_INVALID_HANDLE_VALUE )
        {
          m_imp->entry_path = dir_path;
          // append name, except ignore "." or ".."
          if ( !dot_or_dot_dot( name ) )
          { 
            m_imp->entry_path.m_path_append( name, no_check );
          }
          else
          {
            m_imp->entry_path.m_path_append( "dummy", no_check );
            dir_itr_increment( m_imp );
          }
        }
        else
        {
          throw( filesystem_error(  
            "boost::filesystem::directory_iterator constructor",
            dir_path, fs::detail::system_error_code() ) );
        }  
      }

      path & dir_itr_dereference(
        const dir_itr_imp_ptr & m_imp )
      {
        assert( m_imp.get() ); // fails if dereference end iterator
        return m_imp->entry_path;
      }

      void dir_itr_increment( dir_itr_imp_ptr & m_imp )
      {
        assert( m_imp.get() ); // fails on increment end iterator
        assert( m_imp->handle != YAKE_INVALID_HANDLE_VALUE ); // reality check

        YAKE_SYSTEM_DIRECTORY_TYPE scratch;
        const char * name;

        while ( (name = find_next_file( m_imp->handle,
          m_imp->entry_path, scratch )) != 0 )
        {
          // append name, except ignore "." or ".."
          if ( !dot_or_dot_dot( name ) )
          {
            m_imp->entry_path.m_replace_leaf( name );
            return;
          }
        }
        m_imp.reset(); // make base() the end iterator
      }
    } // namespace detail

} // virtualfs
} // filesystem