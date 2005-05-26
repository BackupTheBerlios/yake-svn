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
#include <yake/base/yakePCH.h>
#include <yake/base/math/yakeRay.h>
#include <yake/base/math/yakePlane.h>

namespace yake {
namespace math {

	std::pair<bool, real> Ray::intersects(const Plane& plane) const
	{
		real denom = plane.normal.dotProduct(mDirection);
		if (Math::Abs(denom) < std::numeric_limits<real>::epsilon())
		{
			// Parallel
			return std::pair<bool, real>(false, 0);
		}
		else
		{
			real nom = plane.normal.dotProduct(mOrigin) + plane.d;
			real t = -(nom/denom);
			return std::pair<bool, real>(t >= 0, t);
		}
	}

}
}
