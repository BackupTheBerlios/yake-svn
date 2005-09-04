#ifndef _VIRTUALFS_CONFIG_HPP_
#define _VIRTUALFS_CONFIG_HPP_

// BOOST_POSIX or BOOST_WINDOWS specify which API to use.
# if !defined( YAKE_WINDOWS ) && !defined( YAKE_POSIX )
#   if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
#     define YAKE_WINDOWS
#   else
#     define YAKE_POSIX
#   endif
# endif

#	if defined( YAKE_WINDOWS )
#		include "windows.h"
#		define YAKE_HANDLE HANDLE
#		define YAKE_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE
#		define YAKE_SYSTEM_DIRECTORY_TYPE WIN32_FIND_DATAA
# endif

#include <yake/base/yakePrerequisites.h>

#if defined( YAKE_VIRTUALFS_EXPORTS )
#	define YAKE_VIRTUALFS_API DLLEXPORT
#else
#	define YAKE_VIRTUALFS_API DLLIMPORT
#endif

//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
namespace filesystem
{
namespace virtualfs
{

//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
// Numeric Types.
typedef unsigned long				dword;
typedef unsigned char				byte;

// The Handle Type.
enum handle { handle_force_dword = 0xFFFFFFFF };

} // namespace virtualfs
} // namespace filesystem

#endif // _VIRTUALFS_CONFIG_HPP_