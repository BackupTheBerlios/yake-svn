#ifndef _VIRTUALFS_CREATE_FILEBUF_HPP_
#define _VIRTUALFS_CREATE_FILEBUF_HPP_

#include "config.hpp"
#include "filebuf.hpp"
#include <nativefs/inc/filebuf.hpp>

namespace filesystem
{
namespace virtualfs
{

enum force_system
{
	force_virtual = true,
	force_native = false
};

YAKE_VIRTUALFS_API std::streambuf & create_filebuf( const path & ph, std::ios_base::openmode 
										 mode = std::ios_base::in, force_system force_virtual = force_native );


} // virtualfs
} // filesystem

#endif // _VIRTUALFS_CREATE_FILEBUF_HPP_