#ifndef _YAKE_SAMPLES_COMMON_EXT_LIB_MAN_H_
#define _YAKE_SAMPLES_COMMON_EXT_LIB_MAN_H_

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

/* library manager extension */
struct ext_lib_man
{
  library_manager & get_lib_man()
	{ return m_lib_man; }

private:
	library_manager m_lib_man;
};

} // namespace common
} // namespace samples
} // namespace yake

#endif // _YAKE_SAMPLES_COMMON_EXT_LIB_MAN_H_