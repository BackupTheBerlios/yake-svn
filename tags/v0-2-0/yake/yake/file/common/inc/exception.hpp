#ifndef _COMMON_EXCEPTION_
#define _COMMON_EXCEPTION_

#include <exception>
#include <string>

#include <boost/shared_ptr.hpp>

#include "config.hpp"
#include "path.hpp"


namespace filesystem
{

    namespace detail
    {
      YAKE_FILESYSTEM_DECL int system_error_code(); // artifact of POSIX and WINDOWS error reporting
    }

    enum error_code
    {
      no_error = 0,
      system_error,     // system generated error; if possible, is translated
                        // to one of the more specific errors below.
      other_error,      // library generated error
      security_error,   // includes access rights, permissions failures
      read_only_error,
      io_error,
      path_error,
      not_found_error,
      not_directory_error,
      busy_error,       // implies trying again might succeed
      already_exists_error,
      not_empty_error,
      is_directory_error,
      out_of_space_error,
      out_of_memory_error,
      out_of_resource_error,
			not_implemented
    };

	class YAKE_FILESYSTEM_DECL filesystem_error : public std::exception
    {
    public:

      filesystem_error(
        const std::string & who,
        const std::string & message ); // assumed to be error_code::other_error

      filesystem_error(
        const std::string & who,
        const path & path1,
        const std::string & message ); // assumed to be error_code::other_error

      filesystem_error(
        const std::string & who,
        const path & path1,
        int sys_err_code );

      filesystem_error(
        const std::string & who,
        const path & path1,
        const path & path2,
        int sys_err_code );

      filesystem_error(
        const std::string & who,
        error_code err_code );

      filesystem_error(
        const std::string & who,
				const path & path1,
        error_code err_code );

      ~filesystem_error() throw();

      virtual const char * what() const throw();

      int             native_error() const { return m_sys_err; }
      // Note: a value of 0 implies a library (rather than system) error
      error_code      error() const { return m_err; }
      const std::string &  who() const; // name of who throwing exception
      const path &    path1() const; // argument 1 to who; may be empty()
      const path &    path2() const; // argument 2 to who; may be empty()

    private:
      class             m_imp;
	  boost::shared_ptr<m_imp> m_imp_ptr; // todo
      int               m_sys_err;
      error_code        m_err;
    };


} // filesystem


#endif