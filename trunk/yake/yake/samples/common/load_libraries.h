#ifndef _YAKE_SAMPLES_COMMON_LOAD_LIBRARIES_H_
#define _YAKE_SAMPLES_COMMON_LOAD_LIBRARIES_H_

// stl
#include <vector>
#include <string>
// local
#include "library_manager.h"

namespace yake
{
namespace samples
{
namespace common
{

/* library manager */
template <class Config>
struct load_libraries
{
  load_libraries()
  {
		std::vector<std::string> libs = config_libraries<Config>::get_libraries();
		for(std::vector<std::string>::const_iterator iter = libs.begin();
			iter != libs.end(); iter++)
		{ m_lib_man.load_library(iter->c_str()); }
	}

private:
	library_manager m_lib_man;
};

} // namespace common
} // namespace samples
} // namespace yake

#endif // _YAKE_SAMPLES_COMMON_LOAD_LIBRARIES_H_
