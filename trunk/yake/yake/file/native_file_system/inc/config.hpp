#ifndef _NATIVE_FILE_SYSTEM_CONFIG_HPP_
#define _NATIVE_FILE_SYSTEM_CONFIG_HPP_

#include <yake/base/yakePrerequisites.h>
#include <yake/base/templates/yakePointer.h>

#if defined( YAKE_NFS_EXPORTS )
#	define YAKE_NFS_API DLLEXPORT
#else
#	define YAKE_NFS_API DLLIMPORT
#endif

namespace filesystem
{
namespace virtualfs
{

// Used archive identifier
static const std::string archive_id = "nfs";

} // virtualfs
} // filesystem

#endif //_NATIVE_FILE_SYSTEM_CONFIG_HPP_