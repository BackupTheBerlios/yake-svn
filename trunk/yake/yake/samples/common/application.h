#ifndef _YAKE_SAMPLES_COMMON_APPLICATION_H_
#define _YAKE_SAMPLES_COMMON_APPLICATION_H_

#include <yake/base/mpl/null_type.h>

namespace yake 
{
namespace samples 
{
namespace common 
{

/* application */
template <class Initialization, class Extension = yake::base::mpl::null_type>
struct application : Extension, Initialization
{};

// if there's no extension then don't inherit from it, otherwise we would have
// two bases null_type classes, because auto_init is using null_type as root for the
// inheritance
template <class Initialization>
struct application<Initialization, yake::base::mpl::null_type> : Initialization
{};


} // namespace common
} // namespace samples
} // namespace yake

#endif // _YAKE_SAMPLES_COMMON_APPLICATION_H_
