#ifndef _NATIVEFS_OPERATIONS_
#define _NATIVEFS_OPERATIONS_ // todo paths

#include "config.hpp"
#include <common/inc/path.hpp> // todo dirs like yake

#include <boost/shared_ptr.hpp>
#include <boost/iterator.hpp>

#include <string>
#include <ctime>

// todo yake
namespace filesystem
{
namespace nativefs
{

//  query functions  ---------------------------------------------------------//
YAKE_FILESYSTEM_DECL bool exists( const path & ph );
YAKE_FILESYSTEM_DECL bool symbolic_link_exists( const path & ph );
YAKE_FILESYSTEM_DECL bool is_directory( const path & ph );
YAKE_FILESYSTEM_DECL bool is_empty( const path & ph );

YAKE_FILESYSTEM_DECL std::time_t last_write_time( const path & ph );
YAKE_FILESYSTEM_DECL void last_write_time( const path & ph, 
					const std::time_t new_time );

//  operations  --------------------------------------------------------------//
YAKE_FILESYSTEM_DECL void create_directory( const path & directory_ph );

YAKE_FILESYSTEM_DECL bool remove( const path & ph );
YAKE_FILESYSTEM_DECL unsigned long remove_all( const path & ph );

YAKE_FILESYSTEM_DECL void rename( const path & from_path,
                 const path & to_path );

YAKE_FILESYSTEM_DECL void copy_file( const path & from_file_ph,
                 const path & to_file_ph );

YAKE_FILESYSTEM_DECL path current_path();
YAKE_FILESYSTEM_DECL const path & initial_path();

YAKE_FILESYSTEM_DECL path system_complete( const path & ph );
YAKE_FILESYSTEM_DECL path complete( const path & ph, 
                 const path & base = initial_path() );

// file ---------------------------------------------------------//

// todo just complete and add root functionality to vfs::nativefs
YAKE_FILESYSTEM_DECL handle open_file( const path & ph, 
		         const std::ios_base::openmode 
					mode = std::ios_base::in );

YAKE_FILESYSTEM_DECL void close_file( handle file_handle );

YAKE_FILESYSTEM_DECL size_t read_file( handle file_handle, 
				 unsigned char* buffer, dword bytes_to_read );

YAKE_FILESYSTEM_DECL int read_complete_file( handle file_handle, 
				 unsigned char* buffer, dword bytes_to_read );

YAKE_FILESYSTEM_DECL size_t write_file( handle file_handle, 
				 unsigned char* buffer, dword bytes_to_write );

YAKE_FILESYSTEM_DECL std::streamoff seek_file( handle this_file, 
				 std::streamoff off, std::ios_base::seekdir way );

// todo: flush -> closeall

//  directory_iterator helpers  ----------------------------------------------//
//    forwarding functions avoid need for BOOST_FILESYSTEM_DECL for class
//    directory_iterator, and so avoid iterator_facade DLL template problems
    namespace detail
    {
      class dir_itr_imp;
      // shared_ptr provides shallow-copy semantics required for InputIterators
      typedef boost::shared_ptr< dir_itr_imp > dir_itr_imp_ptr;
      YAKE_FILESYSTEM_DECL void dir_itr_init( dir_itr_imp_ptr & m_imp,
                                               const path & dir_path );
      YAKE_FILESYSTEM_DECL path & dir_itr_dereference(
                                               const dir_itr_imp_ptr & m_imp );
      YAKE_FILESYSTEM_DECL void dir_itr_increment( dir_itr_imp_ptr & m_imp );
    } // namespace detail

//  directory_iterator  ------------------------------------------------------//
    class directory_iterator
      : public boost::iterator_facade<
      directory_iterator,
      path,
      boost::single_pass_traversal_tag >
    {
    public:
      directory_iterator(){}  // creates the "end" iterator
      explicit directory_iterator( const path & p )
        { detail::dir_itr_init( m_imp, p ); }

    private:
      detail::dir_itr_imp_ptr  m_imp;
      friend class boost::iterator_core_access;
      reference dereference() const 
        { return detail::dir_itr_dereference( m_imp ); }
      void increment()
        { detail::dir_itr_increment( m_imp ); }
      bool equal( const directory_iterator & rhs ) const
        { return m_imp == rhs.m_imp; }
    };


} // nativefs
} // filesystem

#endif