/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
*/
#ifndef _YAKE_BASE_MPL_DISPATCH_ARBITRARY_TYPES_H_
#define _YAKE_BASE_MPL_DISPATCH_ARBITRARY_TYPES_H_

#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/remove_if.hpp>
#include <boost/mpl/size.hpp>

#include "null_type.h"

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace mpl
{

// meta functions: accepts any number of arguments and returns the according type selected by the holder
// todo: holder <int num, typename types> so the holder does not need to have n typedefs but just one typelist
template 
< 
  template<int, typename T1, typename T2, typename T3> class holder, 
  typename T1 = null_type, 
  typename T2 = null_type, 
  typename T3 = null_type
>
struct dispatch_arbitrary_types
{
	// type dispatching according to the number of arguments
	/*
	   template <int num, typename types> struct holder { typedef foo type; ... };
	   template <...> struct holder<0, ...> { typedef foo0<...> type; ... };
	   template <...> struct holder<1, ...> { typedef foo1<...> type; ... };
	   template <...> struct holder<2, ...> { typedef foo2<...> type; ... };
	*/
	typedef boost::mpl::list<T1, T2, T3> given_types;
	typedef typename boost::mpl::remove_if< given_types, boost::is_same<boost::mpl::_, null_type> >::type types;
	typedef typename holder<boost::mpl::size<types>::type::value, T1, T2, T3>::type type;
};

} // mpl
} // base
} // yake

#endif // _YAKE_BASE_MPL_DISPATCH_ARBITRARY_TYPES_H_