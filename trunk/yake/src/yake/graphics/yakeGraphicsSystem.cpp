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
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/graphics/yakePCH.h>
#include <yake/graphics/yakeGraphicsSystem.h>
#include <yake/graphics/yakeGraphicalWorld.h>
#include <yake/graphics/yakeEntity.h>
#include <yake/graphics/yakeGeometryAccess.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace graphics {

	YAKE_IMPLEMENT_REGISTRY( IGraphicsSystem )

	IGraphicsSystem::~IGraphicsSystem()
	{
	}

	/** @Remarks Code has been taken more or less directly from Ogre's OgreCamera.cpp. */
	Ray ICamera::createCameraToViewportRay(const real screenX, const real screenY) const
	{
		real centeredScreenX = (screenX - 0.5f);
		real centeredScreenY = (0.5f - screenY);

		const real nearDist = getNearClipDistance();

		real normalizedSlope = Math::Tan(getFOV() / 2); // getFOV() returns FOVy
		real viewportYToWorldY = normalizedSlope * nearDist * 2;
		real viewportXToWorldX = viewportYToWorldY * getAspectRatio();

		Vector3 rayOrigin, rayDirection;
		if (getProjectionType() == PT_PERSPECTIVE)
		{
			rayOrigin = getPosition(); //@todo fixme should be: getDerivedPosition()
			rayDirection.x = centeredScreenX * viewportXToWorldX;
			rayDirection.y = centeredScreenY * viewportYToWorldY;
			rayDirection.z = -nearDist;
			rayDirection = getOrientation() * rayDirection; //@todo fixme should be: getDerivedOrientation() * rayDirection;
			rayDirection.normalise();
		}
		else
		{
			// Ortho always parallel to point on screen
			rayOrigin.x = centeredScreenX * viewportXToWorldX;
			rayOrigin.y = centeredScreenY * viewportYToWorldY;
			rayOrigin.z = 0.0f;
			rayOrigin = getOrientation() * rayOrigin; //@todo fixme should be: getDerivedOrientation() * rayOrigin;
			rayOrigin = getPosition() + rayOrigin; //@todo fixme should be: getDerivedPosition() + rayOrigin;
			rayDirection = getOrientation() * Vector3::kUnitZ; //@todo fixme should be: getDerivedDirection();
		}
		return Ray( rayOrigin, rayDirection );
	}

} // graphics
} // yake
