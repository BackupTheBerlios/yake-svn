#ifndef _GET_TYPE_OR_NULL_H_
#define _GET_TYPE_OR_NULL_H_

#include <boost/mpl/if.hpp>

// meta functions: returns given arg if there is one, otherwise it will return null
template<typename T = null>
struct get_type_or_null
{
  typedef T type;   
};

#endif // _GET_TYPE_OR_NULL_H_