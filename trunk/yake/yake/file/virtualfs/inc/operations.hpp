#ifndef _VIRTUALFS_OPERATIONS_
#define _VIRTUALFS_OPERATIONS_

#include "config.hpp"
#include "file_system.hpp"
#include "resolve.hpp"

#include <boost/iterator.hpp>
#include <boost/shared_ptr.hpp>

// todo: safe reinterpret cast see mdcpp

namespace filesystem
{
namespace virtualfs
{

// basic ---------------------------------------------------------//
YAKE_VIRTUALFS_API bool exists( const path & ph );

YAKE_VIRTUALFS_API void create_directory( const path & ph );

YAKE_VIRTUALFS_API bool remove( const path & ph );
YAKE_VIRTUALFS_API unsigned long remove_all( const path & ph );

YAKE_VIRTUALFS_API void rename( const path & from_path,
                 const path & to_path );



// information ---------------------------------------------------------//
YAKE_VIRTUALFS_API path current_path();
YAKE_VIRTUALFS_API const path & initial_path();

// file operations ---------------------------------------------------------//
YAKE_VIRTUALFS_API handle open_file( const path & ph, const std::ios_base::openmode = std::ios_base::in );
YAKE_VIRTUALFS_API int read_file( handle this_file, unsigned char* buffer, dword bytes_to_read );
YAKE_VIRTUALFS_API int read_complete_file( handle this_file, unsigned char* buffer, dword bytes_to_read );
YAKE_VIRTUALFS_API void close_file( handle this_file );
YAKE_VIRTUALFS_API void copy_file( const path&, const path& );

// archive operations ---------------------------------------------------------//
YAKE_VIRTUALFS_API void create_archive_from( const path & archive, const path & dir );


//  directory_iterator helpers  ----------------------------------------------//
//    forwarding functions avoid need for BOOST_FILESYSTEM_DECL for class
//    directory_iterator, and so avoid iterator_facade DLL template problems
    namespace detail
    {
      class dir_itr_imp;
      // shared_ptr provides shallow-copy semantics required for InputIterators
      typedef boost::shared_ptr< dir_itr_imp > dir_itr_imp_ptr;
      YAKE_VIRTUALFS_API void dir_itr_init( dir_itr_imp_ptr & m_imp,
                                               const path & dir_path );
      YAKE_VIRTUALFS_API path & dir_itr_dereference(
                                               const dir_itr_imp_ptr & m_imp );
      YAKE_VIRTUALFS_API void dir_itr_increment( dir_itr_imp_ptr & m_imp );
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

} // virtualfs
} // filesystem

#endif