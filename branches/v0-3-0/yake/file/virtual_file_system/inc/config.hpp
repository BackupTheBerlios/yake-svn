#ifndef _VIRTUAL_FILE_SYSTEM_CONFIG_HPP_
#define _VIRTUAL_FILE_SYSTEM_CONFIG_HPP_

#include <boost/archive/binary_iarchive.hpp> //text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <yake/base/yakePrerequisites.h>
#include <yake/base/templates/yakePointer.h>

#if defined( YAKE_VFS_EXPORTS )
#	define YAKE_VFS_API DLLEXPORT
#else
#	define YAKE_VFS_API DLLIMPORT
#endif

namespace filesystem
{
namespace virtualfs
{

// Used archive identifier
static const std::string archive_id = "vfs";

// Used serialization archives
typedef boost::archive::binary_iarchive in_archive;
typedef boost::archive::binary_oarchive out_archive;

} // virtualfs
} // filesystem

#endif //_VIRTUAL_FILE_SYSTEM_CONFIG_HPP_