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
#ifndef YAKE_BASE_MOVABLE_H
#define YAKE_BASE_MOVABLE_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#include "yakePrerequisites.h"
// Yake

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

	using namespace ::yake::math;
/** Interface for movable objects and basic implementation of
	methods for convenience (like translate()).

	All objects which are able to move and/or rotate should inherit
	and implement this interface.

	\todo cache transformations !?
	\todo add rotational helpers (yaw(), pitch() etc )
*/
class YAKE_BASE_API Movable
{
protected:
	Movable();
// Class
public:
	virtual ~Movable();

// Methods
public:
	virtual void setPosition( const math::Vector3& rPosition ) = 0;
	virtual void setOrientation( const math::Quaternion& rOrientation ) = 0;
	virtual math::Vector3 getPosition() const = 0;
	virtual math::Quaternion getOrientation() const = 0;
	virtual math::Vector3 getDerivedPosition() const = 0;
	virtual math::Quaternion getDerivedOrientation() const = 0;
	

	inline virtual void translate( const math::Vector3& rDelta )
	{
		setPosition( getPosition() + rDelta );
	}
};

} // yake

#endif // YAKE_BASE_MOVABLE_H

