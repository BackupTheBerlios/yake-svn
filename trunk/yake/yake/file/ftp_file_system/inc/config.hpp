#ifndef _FTP_FILE_SYSTEM_CONFIG_HPP_
#define _FTP_FILE_SYSTEM_CONFIG_HPP_

#include <yake/base/yakePrerequisites.h>

#if defined( YAKE_FTP_EXPORTS )
#	define YAKE_FTP_API DLLEXPORT
#else
#	define YAKE_FTP_API DLLIMPORT
#endif

namespace filesystem
{
namespace virtualfs
{

// Used archive identifier
static const std::string archive_id = "ftp";

// Cache directory
static const std::string ftp_cache = "ftp_cache";

} // virtualfs
} // filesystem

#endif //_ftp_file_system_CONFIG_HPP_
