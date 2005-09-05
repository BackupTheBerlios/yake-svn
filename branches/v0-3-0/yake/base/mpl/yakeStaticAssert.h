/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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
#ifndef YAKE_BASE_MPL_STATICASSERT_H
#define YAKE_BASE_MPL_STATICASSERT_H

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace mpl
{

/** Helper class for compile time checking. */
template <bool, typename Error>
struct StaticAssert
{};

/** Specialized class that makes compile time checks fail if the condition is false. */
template<typename Error>
struct StaticAssert <false, Error>;

/** Helper class that simply accepts the return value of sizeof. */
template <int>
struct AbsorbValue
{};

/** Checks the compile-time condition using sizeof as executor. If the condition is false, a compiler error is generated. */
#ifdef YAKE_DEBUG_BUILD
#	define YAKE_STATIC_ASSERT(condition, message) \
		class ERROR_##message {}; \
		typedef AbsorbValue<sizeof(yake::base::mpl::StaticAssert<(condition) != 0, ERROR_##message>)> AssertType##message
#else
#	define YAKE_STATIC_ASSERT(condition, message) typedef bool message
#endif

} // mpl
} // base
} // yake

#endif // YAKE_BASE_MPL_STATICASSERT_H
