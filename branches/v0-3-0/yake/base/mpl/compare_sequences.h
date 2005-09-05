/*
   ------------------------------------------------------------------------------------
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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#ifndef _YAKE_BASE_MPL_COMPARE_SEQUENCES_H_
#define _YAKE_BASE_MPL_COMPARE_SEQUENCES_H_

#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/count_if.hpp>
#include <boost/mpl/lambda.hpp>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace mpl
{

// meta function: compares given type lists at compile time where the order of elements does not matter
// usage: typedef compare_sequences<list1, list2>::type check;
// note: a simple static assert would not work, because the typedef does not instantiate the type, 
//       so we cause a compile time error since 'static_assert_helper<false>' does not define 'type'
template <typename Sequence1, typename Sequence2>
struct compare_sequences
	: boost::mpl::equal_to
		<
			typename boost::mpl::count_if
			<
				Sequence1, 
				boost::mpl::contains<Sequence2, boost::mpl::_>
			>::type,
			typename boost::mpl::size<Sequence1>::type
		>
{};

} // mpl
} // base
} // yake

#endif // _YAKE_BASE_MPL_COMPARE_SEQUENCES_H_