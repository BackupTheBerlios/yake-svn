#ifndef _NATIVEFS_CONFIG_
#define _NATIVEFS_CONFIG_

//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================

#define YAKE_FILESYSTEM_DECL

namespace filesystem
{
namespace nativefs
{

enum handle { HANDLE_FORCE_DWORD = 0xFFFFFFFF };

} // namespace nativefs
} // namespace filesystem

#endif