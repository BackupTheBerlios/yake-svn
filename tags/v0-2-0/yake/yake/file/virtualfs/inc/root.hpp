#ifndef _ROOT_HPP_
#define _ROOT_HPP_

#include <algorithm>
#include <vector>
#include <string>

#include <common/inc/path.hpp>
#include <common/inc/exception.hpp>

#include "config.hpp"

namespace filesystem
{
namespace virtualfs
{

enum path_type
{
  local_path,
	shared_path
};

typedef std::pair< std::string, path_type > root_entry;
typedef std::vector< root_entry > root_path_list;

// root paths ---------------------------------------------------//
YAKE_VIRTUALFS_API void add_root_path( const path & root_path, path_type type = local_path );
YAKE_VIRTUALFS_API void add_root_path( const std::string root_path, path_type type = local_path );
YAKE_VIRTUALFS_API void remove_root_path( const path & root_path );
YAKE_VIRTUALFS_API void remove_root_path( dword index );
YAKE_VIRTUALFS_API dword get_num_root_paths();
YAKE_VIRTUALFS_API path get_root_path( dword index );
YAKE_VIRTUALFS_API const root_path_list & get_root_paths();

} // namespace virtualfs
} // namespace filesystem

#endif // _ROOT_HPP_