#include <inc/config.hpp>
#include <inc/operations.hpp>
#include <common/inc/exception.hpp>

#include <boost/scoped_array.hpp>
#include <boost/detail/workaround.hpp>

#include <cstring>
#include <iostream>


#ifdef BOOST_NO_STDC_NAMESPACE
namespace std { using ::strcmp; using ::remove; using ::rename; }
#endif

namespace fs  = filesystem;
namespace nfs = filesystem::nativefs;

// BOOST_POSIX or BOOST_WINDOWS specify which API to use.
# if !defined( BOOST_WINDOWS ) && !defined( BOOST_POSIX )
#   if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
#     define BOOST_WINDOWS
#   else
#     define BOOST_POSIX
#   endif
# endif

# if defined(BOOST_WINDOWS)
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

//  helpers  -----------------------------------------------------------------//

namespace
{
#ifdef BOOST_POSIX

# define BOOST_HANDLE DIR *
# define BOOST_INVALID_HANDLE_VALUE 0
# define BOOST_SYSTEM_DIRECTORY_TYPE struct dirent *

  inline const char *  find_first_file( const char * dir,
    BOOST_HANDLE & handle, BOOST_SYSTEM_DIRECTORY_TYPE & )
  // Returns: 0 if error, otherwise name
  {
    const char * dummy_first_name = ".";
    return ( (handle = ::opendir( dir ))
      == BOOST_INVALID_HANDLE_VALUE ) ? 0 : dummy_first_name;
  }  

  inline void find_close( BOOST_HANDLE handle )
  {
    assert( handle != BOOST_INVALID_HANDLE_VALUE );
    ::closedir( handle );
  }

  inline const char * find_next_file(
    BOOST_HANDLE handle, const fs::path & ph, BOOST_SYSTEM_DIRECTORY_TYPE & )
  // Returns: if EOF 0, otherwise name
  // Throws: if system reports error
  {

//  TODO: use readdir_r() if available, so code is multi-thread safe.
//  Fly-in-ointment: not all platforms support readdir_r().

    struct dirent * dp;
    errno = 0;
    if ( (dp = ::readdir( handle )) == 0 )
    {
      if ( errno != 0 )
      {
        throw(
          fs::filesystem_error(
            "boost::filesystem::directory_iterator::operator++",
            ph, errno ) );
      }
      else { return 0; } // end reached
    }
    return dp->d_name;
  }
#else // BOOST_WINDOWS

# define BOOST_HANDLE HANDLE
# define BOOST_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE
# define BOOST_SYSTEM_DIRECTORY_TYPE WIN32_FIND_DATAA

  inline const char *  find_first_file( const char * dir,
    BOOST_HANDLE & handle, BOOST_SYSTEM_DIRECTORY_TYPE & data )
  // Returns: 0 if error, otherwise name
  {
//    std::cout << "find_first_file " << dir << std::endl;
    std::string dirpath( std::string(dir) + "/*" );
    return ( (handle = ::FindFirstFileA( dirpath.c_str(), &data ))
      == BOOST_INVALID_HANDLE_VALUE ) ? 0 : data.cFileName;
  }  

  inline void find_close( BOOST_HANDLE handle )
  {
//    std::cout << "find_close" << std::endl;
    assert( handle != BOOST_INVALID_HANDLE_VALUE );
    ::FindClose( handle );
  }

  inline const char * find_next_file(
	  BOOST_HANDLE handle, const filesystem::path & ph,
    BOOST_SYSTEM_DIRECTORY_TYPE & data )
  // Returns: 0 if EOF, otherwise name
  // Throws: if system reports error
  {
    if ( ::FindNextFileA( handle, &data ) == 0 )
    {
      if ( ::GetLastError() != ERROR_NO_MORE_FILES )
      {
        throw( fs::filesystem_error(
          "boost::filesystem::directory_iterator::operator++",
          ph.branch_path(), fs::detail::system_error_code() ) );
      }
      else { return 0; } // end reached
     }
    return data.cFileName;
  }

#endif

  
  nfs::directory_iterator end_itr;

  bool is_empty_directory( const fs::path & dir_path )
  {
    return nfs::directory_iterator(dir_path) == end_itr;
  }

  unsigned long remove_all_aux( const fs::path & ph )
  {
    unsigned long count = 1;
    if ( !nfs::symbolic_link_exists( ph ) // don't recurse symbolic links
      && nfs::is_directory( ph ) )
    {
      for ( nfs::directory_iterator itr( ph );
            itr != end_itr; ++itr )
      {
        count += remove_all_aux( *itr );
      }
    }
    nfs::remove( ph );
    return count;
  }

} // unnamed namespace

  namespace filesystem
  {
  namespace nativefs
  {
    namespace detail
    {

//  dir_itr_imp  -------------------------------------------------------------// 

      class dir_itr_imp
      {
      public:
        path              entry_path;
        BOOST_HANDLE      handle;

        ~dir_itr_imp()
        {
          if ( handle != BOOST_INVALID_HANDLE_VALUE ) find_close( handle );
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

      YAKE_FILESYSTEM_DECL void dir_itr_init( dir_itr_imp_ptr & m_imp,
                                               const path & dir_path )
      {
        m_imp.reset( new dir_itr_imp );
        BOOST_SYSTEM_DIRECTORY_TYPE scratch;
        const char * name = 0;  // initialization quiets compiler warnings
        if ( dir_path.empty() )
        m_imp->handle = BOOST_INVALID_HANDLE_VALUE;
        else
          name = find_first_file( dir_path.native_directory_string().c_str(),
            m_imp->handle, scratch );  // sets handle

        if ( m_imp->handle != BOOST_INVALID_HANDLE_VALUE )
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

      YAKE_FILESYSTEM_DECL path & dir_itr_dereference(
        const dir_itr_imp_ptr & m_imp )
      {
        assert( m_imp.get() ); // fails if dereference end iterator
        return m_imp->entry_path;
      }

      YAKE_FILESYSTEM_DECL void dir_itr_increment( dir_itr_imp_ptr & m_imp )
      {
        assert( m_imp.get() ); // fails on increment end iterator
        assert( m_imp->handle != BOOST_INVALID_HANDLE_VALUE ); // reality check

        BOOST_SYSTEM_DIRECTORY_TYPE scratch;
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

//  free functions  ----------------------------------------------------------//

    YAKE_FILESYSTEM_DECL bool exists( const path & ph )
    {
#   ifdef BOOST_POSIX
      struct stat path_stat;
      return ::stat( ph.string().c_str(), &path_stat ) == 0;  
#   else
      return ::GetFileAttributesA( ph.string().c_str() ) != 0xFFFFFFFF;
#   endif
    }

    // suggested by Walter Landry
    YAKE_FILESYSTEM_DECL bool symbolic_link_exists( const path & ph )
    {
#   ifdef BOOST_POSIX
      struct stat path_stat;
      return ::lstat( ph.native_file_string().c_str(), &path_stat ) == 0
        && S_ISLNK( path_stat.st_mode );
#   else
      return false; // Windows has no O/S concept of symbolic links
                    // (.lnk files are an application feature, not
                    // a Windows operating system feature)
#   endif
    }

    YAKE_FILESYSTEM_DECL bool is_directory( const path & ph )
    {
#   ifdef BOOST_POSIX
      struct stat path_stat;
      if ( ::stat( ph.native_directory_string().c_str(), &path_stat ) != 0 )
        throw( filesystem_error(
          "boost::filesystem::is_directory",
          ph, fs::detail::system_error_code() ) );
      return S_ISDIR( path_stat.st_mode );
#   else
      DWORD attributes = ::GetFileAttributesA( ph.native_directory_string().c_str() );
      if ( attributes == 0xFFFFFFFF )
        throw( filesystem_error(
          "boost::filesystem::is_directory",
          ph, fs::detail::system_error_code() ) );
      return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#   endif
    }

    YAKE_FILESYSTEM_DECL bool _is_empty( const path & ph )
    {
#   ifdef BOOST_POSIX
      struct stat path_stat;
      if ( ::stat( ph.string().c_str(), &path_stat ) != 0 )
        throw( filesystem_error(
          "boost::filesystem::is_empty",
          ph, fs::detail::system_error_code() ) );
      
      return S_ISDIR( path_stat.st_mode )
        ? is_empty_directory( ph )
        : path_stat.st_size == 0;
#   else
      WIN32_FILE_ATTRIBUTE_DATA fad;
      if ( !::GetFileAttributesExA( ph.string().c_str(),
        ::GetFileExInfoStandard, &fad ) )
        throw( filesystem_error(
          "boost::filesystem::is_empty",
          ph, fs::detail::system_error_code() ) );
      
      return ( fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        ? is_empty_directory( ph )
        :( !fad.nFileSizeHigh && !fad.nFileSizeLow );
#   endif
    }

    YAKE_FILESYSTEM_DECL std::time_t last_write_time( const path & ph )
    {
      // Works for both Windows and POSIX
      struct stat path_stat;
      if ( ::stat( ph.string().c_str(), &path_stat ) != 0 )
        throw( filesystem_error(
          "boost::filesystem::last_write_time",
          ph, fs::detail::system_error_code() ) );
      return path_stat.st_mtime;
    }

    YAKE_FILESYSTEM_DECL void last_write_time( const path & ph, const std::time_t new_time )
    {
      // Works for both Windows and POSIX
      ::utimbuf buf;
      buf.actime = std::time_t();
      buf.modtime = new_time;
      if ( ::utime( ph.string().c_str(),
        new_time == std::time_t() ? 0 : &buf ) != 0 )
        throw( filesystem_error(
          "boost::filesystem::last_write_time",
          ph, fs::detail::system_error_code() ) );
    }

    YAKE_FILESYSTEM_DECL void create_directory( const path & dir_path )
    {
#   ifdef BOOST_POSIX
      if ( ::mkdir( dir_path.native_directory_string().c_str(),
        S_IRWXU|S_IRWXG|S_IRWXO ) != 0 )
#   else
      if ( !::CreateDirectoryA( dir_path.native_directory_string().c_str(), 0 ) )
#   endif
        throw( filesystem_error(
          "boost::filesystem::create_directory",
          dir_path, fs::detail::system_error_code() ) );
    }

    YAKE_FILESYSTEM_DECL bool remove( const path & ph )
    {
      if ( exists( ph ) )
      {
#   ifdef BOOST_POSIX
        if ( std::remove( ph.string().c_str() ) != 0 )
        {
          int error = fs::detail::system_error_code();
          // POSIX says "If the directory is not an empty directory, rmdir()
          // shall fail and set errno to EEXIST or ENOTEMPTY."
          // Linux uses ENOTEMPTY, Solaris uses EEXIST.
          if ( error == EEXIST ) error = ENOTEMPTY;
          throw( filesystem_error(
            "boost::filesystem::remove", ph, error ) );
        }
#   else
        if ( is_directory( ph ) )
        {
          if ( !::RemoveDirectoryA( ph.string().c_str() ) )
            throw( filesystem_error(
              "boost::filesystem::remove",
              ph, fs::detail::system_error_code() ) );
        }
        else
        {
          if ( !::DeleteFileA( ph.string().c_str() ) )
            throw( filesystem_error(
              "boost::filesystem::remove",
              ph, fs::detail::system_error_code() ) );
        }
#   endif
        return true;
      }
      return false;
    }

    YAKE_FILESYSTEM_DECL unsigned long remove_all( const path & ph )
    {
      return exists( ph )|| symbolic_link_exists( ph )
        ? remove_all_aux( ph ) : 0;
    }

    YAKE_FILESYSTEM_DECL void rename( const path & old_path,
                 const path & new_path )
    {
#   ifdef BOOST_POSIX
      if ( exists( new_path ) // POSIX is too permissive so must check
        || std::rename( old_path.string().c_str(), new_path.string().c_str() ) != 0 )
#   else
      if ( !::MoveFileA( old_path.string().c_str(), new_path.string().c_str() ) )
#   endif
        throw( filesystem_error(
          "boost::filesystem::rename",
          old_path, new_path, fs::detail::system_error_code() ) );
    }

    YAKE_FILESYSTEM_DECL void copy_file( const path & from_file_ph,
                    const path & to_file_ph )
    {
#   ifdef BOOST_POSIX
      // TODO: Ask POSIX experts if this is the best way to copy a file

      const std::size_t buf_sz = 32768;
      boost::scoped_array<char> buf( new char [buf_sz] );
      int infile=0, outfile=0;  // init quiets compiler warning
      struct stat from_stat;

      if ( ::stat( from_file_ph.string().c_str(), &from_stat ) != 0
        || (infile = ::open( from_file_ph.string().c_str(),
                             O_RDONLY )) < 0
        || (outfile = ::open( to_file_ph.string().c_str(),
                              O_WRONLY | O_CREAT | O_EXCL,
                              from_stat.st_mode )) < 0 )
      {
        if ( infile != 0 ) ::close( infile );
        throw( filesystem_error(
          "boost::filesystem::copy_file",
          from_file_ph, to_file_ph, fs::detail::system_error_code() ) );
      }

      ssize_t sz;
      while ( (sz = ::read( infile, buf.get(), buf_sz )) > 0
        && (sz = ::write( outfile, buf.get(), sz )) > 0 ) {}

      ::close( infile );
      ::close( outfile );

      if ( sz != 0 )
#   else
      if ( !::CopyFileA( from_file_ph.string().c_str(),
                      to_file_ph.string().c_str(), /*fail_if_exists=*/true ) )
#   endif
        throw( filesystem_error(
          "boost::filesystem::copy_file",
          from_file_ph, to_file_ph, fs::detail::system_error_code() ) );
    }

    YAKE_FILESYSTEM_DECL path current_path()
    {
#   ifdef BOOST_POSIX
      long path_max = ::pathconf( ".", _PC_PATH_MAX );
      if ( path_max < 1 )
        throw(
          filesystem_error( "boost::filesystem::current_path",
            "_PC_PATH_MAX < 1" ) );
      boost::scoped_array<char>
        buf( new char[static_cast<std::size_t>(path_max)] );
      if ( ::getcwd( buf.get(), static_cast<std::size_t>(path_max) ) == 0 )
#   else
      DWORD sz;
      if ( (sz = ::GetCurrentDirectoryA( 0, static_cast<char*>(0) )) == 0 )
        throw(
          filesystem_error( "boost::filesystem::current_path",
            "size is 0" ) );
      boost::scoped_array<char> buf( new char[sz] );
      if ( ::GetCurrentDirectoryA( sz, buf.get() ) == 0 )
#   endif
        throw(
          filesystem_error( "boost::filesystem::current_path", path(),
            fs::detail::system_error_code() ) );
      return path( buf.get(), native );
    }

    YAKE_FILESYSTEM_DECL const path & initial_path()
    {
      static path init_path;
      if ( init_path.empty() ) init_path = current_path();
      return init_path;
    }

    YAKE_FILESYSTEM_DECL path system_complete( const path & ph )
    {
#   ifdef BOOST_WINDOWS
      if ( ph.empty() ) return ph;
      char buf[MAX_PATH];
      char * pfn;
      std::size_t len = ::GetFullPathNameA( ph.string().c_str(),
                                            sizeof(buf) , buf, &pfn );
      if ( !len )
        { throw(
            filesystem_error( "boost::filesystem::system_complete",
              ph, "size is 0" ) ); }
      buf[len] = '\0';
      return path( buf, native );
#   else
      return (ph.empty() || ph.is_complete())
        ? ph : current_path() / ph;
#   endif
    }
    
    YAKE_FILESYSTEM_DECL path complete( const path & ph, const path & base )
    {
      assert( base.is_complete()
        && (ph.is_complete() || !ph.has_root_name()) ); // precondition
#   ifdef BOOST_WINDOWS
      if (ph.empty() || ph.is_complete()) return ph;
      if ( !ph.has_root_name() )
        return ph.has_root_directory()
          ? path( base.root_name(), native ) / ph
          : base / ph;
      return base / ph;
#   else
      return (ph.empty() || ph.is_complete()) ? ph : base / ph;
#   endif
    }

/*
app
ate
binary
in
out
trunc 

a _O_WRONLY | _O_APPEND (usually _O_WRONLY | _O_CREAT | _O_APPEND) 
a+ _O_RDWR | _O_APPEND (usually _O_RDWR | _O_APPEND | _O_CREAT ) 
R _O_RDONLY 
r+ _O_RDWR 
W _O_WRONLY (usually _O_WRONLY | _O_CREAT | _O_TRUNC) 
W+ _O_RDWR (usually _O_RDWR | _O_CREAT | _O_TRUNC) 
B _O_BINARY 
T _O_TEXT 
C None 
N None 
S _O_SEQUENTIAL 
R _O_RANDOM 
T _O_SHORTLIVED 
D _O_TEMPORARY 
*/

namespace 
{

	struct open_mode_xlate 
	{ 
		std::ios_base::openmode mode_enum; 
		char mode_string; 
	};

	const open_mode_xlate mode_table[] =
	{
		{ std::ios_base::app,    'a' }, // todo
		{ std::ios_base::ate,    '?' },
		{ std::ios_base::binary, 'b' },
		{ std::ios_base::in,     'r' },
		{ std::ios_base::out,    'w' },
		{ std::ios_base::trunc,  '?' }
	};

	char lookup_single_mode( std::ios_base::openmode mode_enum )
	{
      for ( const open_mode_xlate * cur = &mode_table[0];
          cur != mode_table + sizeof(mode_table)/sizeof(open_mode_xlate); 
          ++cur )
      {
					if ( mode_enum == cur->mode_enum ) return cur->mode_string;
      }

      throw( 
				filesystem_error( "yake::filesystem::nativefs::lookup_mode",
						not_found_error ) );
	}

	std::string lookup_mode( std::ios_base::openmode mode_enum )
  {
			std::string flags;
			for( int i = 0; i <= 5; i++ )
          if( mode_enum & (1 << i) )
              flags += lookup_single_mode( 
								static_cast< std::ios_base::openmode >( 1 << i ) );
			return flags;
	}

} // unnamed namespace
// todo : use boost fs error system for system errrors

	YAKE_FILESYSTEM_DECL handle open_file( const path & ph, 
					const std::ios_base::openmode mode )
	{
#   ifdef BOOST_WINDOWS
		FILE * stream = fopen( ph.string().c_str(), lookup_mode( mode ).c_str() );
		if( stream == NULL )
            throw( filesystem_error( 
							"yake::filesystem::nativefs::open_file",
                	"Invalid file stream." ) );
		return ( handle )( dword )( stream );
#   else
		return // todo: same as above?
#   endif        
	}

	YAKE_FILESYSTEM_DECL void close_file( handle file_handle )
	{
		if( file_handle == NULL )
            throw(
				filesystem_error( "yake::filesystem::nativefs::close_file",
                	"Invalid file stream." ) );
#   ifdef BOOST_WINDOWS
		if( fclose( reinterpret_cast<FILE*>( file_handle ) ) != 0 ) // todo:BOOST_HANDLE
            throw( filesystem_error( 
							"yake::filesystem::nativefs::close_file",
                	"Error while closing the file stream." ) );
#   else
		// todo: same as above?
#   endif        
	}

	YAKE_FILESYSTEM_DECL size_t read_file( handle file_handle,
					unsigned char* buffer, dword bytes_to_read )
	{
		if( file_handle == NULL )
      throw( filesystem_error( 
				"yake::filesystem::nativefs::read_file",
                	"Invalid file stream." ) );

		if( bytes_to_read > 0 && buffer == NULL )
      throw( filesystem_error( 
				"yake::filesystem::nativefs::read_file",
                	"Invalid buffer pointer." ) );

#   ifdef BOOST_WINDOWS
		if( bytes_to_read <= 0 ) return 0;

		return fread( buffer, sizeof( char ), bytes_to_read,
            reinterpret_cast<FILE*>( file_handle ) );
#   else
		// todo: same as above?
#   endif        
	}


	YAKE_FILESYSTEM_DECL int read_complete_file( handle file_handle, 
				 unsigned char* buffer, dword bytes_to_read )
	{
		return 0;
	} // todo

	YAKE_FILESYSTEM_DECL size_t write_file( handle file_handle,
					unsigned char* buffer, dword bytes_to_write )
	{
		if( file_handle == NULL )
      throw( filesystem_error( 
				"yake::filesystem::nativefs::write_file",
                	"Invalid file stream." ) );

		if( bytes_to_write > 0 && buffer == NULL )
      throw( filesystem_error( 
				"yake::filesystem::nativefs::write_file",
					"Invalid buffer pointer." ) );

		// todo test read only

#   ifdef BOOST_WINDOWS
		if( bytes_to_write <= 0 ) return 0;

		size_t bytes_written = fwrite( 
			buffer, sizeof( char ), bytes_to_write,
            reinterpret_cast<FILE*>( file_handle ) );

		if( bytes_written == 0 )
			throw( filesystem_error( 
				"yake::filesystem::nativefs::write_file",
					"Error while reading." ) );

		return bytes_written;
#   else
		// todo: same as above?
#   endif        
	}

namespace
{
	struct seek_dir 
	{ 
		std::ios_base::seekdir std_origin; 
		int stdio_origin; 
	};

	const seek_dir seek_table[] =
	{
		{ std::ios_base::beg, SEEK_SET },
		{ std::ios_base::cur, SEEK_CUR },
		{ std::ios_base::end, SEEK_END },
	};
} // unnamed namespace

	YAKE_FILESYSTEM_DECL std::streamoff seek_file( handle this_file, 
				 std::streamoff off, std::ios_base::seekdir way )
	{
		if( this_file == NULL )
      throw( filesystem_error( 
				"yake::filesystem::nativefs::write_file",
                	"Invalid file stream." ) );

#   ifdef BOOST_WINDOWS
		// set position
		if( fseek( reinterpret_cast<FILE*>( this_file ),
					static_cast<long>( off ), 
					seek_table[ way ].stdio_origin ) != 0 )
		{
			throw( filesystem_error( 
				"yake::filesystem::nativefs::seek_file",
					"Error while seeking." ) );
		}

		// get position
		return ftell( reinterpret_cast<FILE*>( this_file ) );

#   else
		// todo: same as above?
#   endif        
	}

} // nativefs
} // filesystem