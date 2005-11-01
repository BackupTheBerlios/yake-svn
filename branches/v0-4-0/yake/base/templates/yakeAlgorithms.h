/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#ifndef YAKE_BASE_TEMPLATES_ALGORITHMS_H
#define YAKE_BASE_TEMPLATES_ALGORITHMS_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif
// Yake

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

template<class ContainerT>
	typename ContainerT::const_iterator find(const ContainerT& ctr, const typename ContainerT::value_type& v)
{
	return std::find( ctr.begin(), ctr.end(), v );
}
template<class ContainerT>
	typename ContainerT::iterator find(ContainerT& ctr, const typename ContainerT::value_type& v)
{
	return std::find( ctr.begin(), ctr.end(), v );
}

/** Performs an operation operation for each element in a container [ begin, end - 1 ]. */
//template< typename Iterator, typename Operation >
//YAKE_BASE_API void ForEach( Iterator begin, Iterator end, Operation operation );

} // yake

/** Include inline implementation. */
#include "yakeAlgorithms.inl"

#endif // YAKE_BASE_TEMPLATES_ALGORITHMS_H
