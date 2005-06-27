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
#ifndef YAKE_BASE_TEMPLATES_NIFTYCONTAINER_H
#define YAKE_BASE_TEMPLATES_NIFTYCONTAINER_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace templates
{

// Use pointers for polymorhpic types
template< typename T, bool isPolymorhpic >
struct NiftyContainerHelper
{
	typedef T* ValueType;
};

// Nonpolymorphic types by value
template< typename T >
struct NiftyContainerHelper< T, false >
{
	typedef T ValueType;
};

// Todo: ispoly compile time check
// Nifty Container
template< typename T >
struct NiftyContainer
{
	typedef NiftyContainerHelper< T, isPolymorphic  > Traits;
	typedef typename Traits::ValueType ValueType;
};

} // templates
} // base
} // yake

#endif // YAKE_BASE_TEMPLATES_NIFTYCONTAINER_H
