#ifndef _YAKE_SAMPLES_COMMON_LIBRARY_MANAGER_H_
#define _YAKE_SAMPLES_COMMON_LIBRARY_MANAGER_H_

// stl
#include <vector>
#include <string>
// boost
#include <boost/shared_ptr.hpp>
// yake
#include <yake/base/yakeLibrary.h>

namespace yake 
{
namespace samples 
{
namespace common 
{

/* library manager */
struct library_manager
{
	typedef std::vector< boost::shared_ptr<base::Library> > library_list;

  void load_library(const char * lib_name)
	{ m_library_list.push_back(boost::shared_ptr<base::Library>(new base::Library(lib_name))); }

  library_list m_library_list;
};

library_manager & operator<<(library_manager & lib_man, const char * lib_name)
{ lib_man.load_library(lib_name); return lib_man; }


} // namespace common
} // namespace samples
} // namespace yake

#endif // _YAKE_SAMPLES_COMMON_LIBRARY_MANAGER_H_
