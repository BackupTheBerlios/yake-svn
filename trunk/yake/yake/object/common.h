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
#ifndef COMMON_H
#define COMMON_H

/**
	All include files needed by this library.
	This file ("common.h") could be used in a precompiled header, for example.
*/

#include <cassert>
#include <limits>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#define OBJECTS_NAMESPACE object

namespace yake {
namespace OBJECTS_NAMESPACE {

	/** Common types */

	enum ResultCode
	{
		RC_OK,
		RC_ALREADY_EXISTS,
		RC_ALREADY_REGISTERED = RC_ALREADY_EXISTS,
		RC_UNKNOWN_CLASS,
		RC_NOT_FOUND,
		RC_INVALID_INPUT
	};

} // namespace object

//--------------------------

namespace util {

	#include <algorithm>

	template<typename T>
		std::pair<typename T::key_type,typename T::key_type> get_minmax_keys( const T& ctr )
	{
		return std::make_pair(
			ctr.begin()->first,
			(--ctr.end())->second );
	}

	/** Erases an element from a STL container. Element is identified by value.
		@return true if element was erased, false if element couldn't be found in the container.
	*/
	template<typename T>
		bool safeErase( T& container, const typename T::value_type value)
	{
		typename T::iterator it = std::find(container.begin(), container.end(), value);
		if (it == container.end())
			return false;
		container.erase(it);
		return true;
	}
	/** Determines whether an element is in a given STL or STL-like container.
		@return true if element was found in the container, false if not.
	*/
	template<typename T>
		bool contains( const T& container, const typename T::value_type& value)
	{
		return (container.end() != std::find(container.begin(), container.end(), value));
	}
	/** Determines whether an element is in a given STL or STL-like associative container.
		@return true if element was found in the container, false if not.
	*/
	template<typename T>
		bool map_contains( const T& container, const typename T::key_type key)
	{
		return (container.end() != container.find(key));
	}

} // namespace util
} // namespace yake

#endif

