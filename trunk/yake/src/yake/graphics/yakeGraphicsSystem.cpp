/**------------------------------------------------------------------------------------
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

	Ray ICamera::createCameraToViewportRay(const real screenX, const real screenY) const
	{
		real centeredScreenX = (screenX - 0.5f);
		real centeredScreenY = (0.5f - screenY);

		const real nearDist = getNearClipDistance();

		real normalizedSlope = Math::Tan(getFOV() / 2); // getFOV() returns FOVy
		real viewportYToWorldY = normalizedSlope * nearDist * 2;
		real viewportXToWorldX = viewportYToWorldY * getAspectRatio();

		Vector3 rayDirection(centeredScreenX * viewportXToWorldX,
			centeredScreenY * viewportYToWorldY,
			-nearDist);
		rayDirection = getOrientation() * rayDirection; //getDerivedOrientation() * rayDirection;
		rayDirection.normalise();

		//return Ray(getDerivedPosition(), rayDirection);
		return Ray( getPosition(), rayDirection );
	}

} // graphics
} // yake
