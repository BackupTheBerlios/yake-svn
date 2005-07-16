#ifndef _COMMON_CONFIG_
#define _COMMON_CONFIG_

//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================
namespace filesystem
{

#define YAKE_FILESYSTEM_DECL // todo

//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
// Numeric Types.
typedef unsigned long dword;
typedef unsigned char byte;

// The Handle Type.
enum handle
{ HANDLE_FORCE_DWORD = 0xFFFFFFFF };

} // filesystem

#endif