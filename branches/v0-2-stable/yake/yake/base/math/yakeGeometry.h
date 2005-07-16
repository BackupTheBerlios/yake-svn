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
#ifndef YAKE_BASE_MATH_GEOMETRY_H
#define YAKE_BASE_MATH_GEOMETRY_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif
// Yake
#include <yake/base/math/yakeMath.h>

namespace yake {
namespace math {

	typedef YAKE_BASE_API Vector3 Point3;
	inline bool operator > (const Point3 & lhs, const Point3 & rhs)
	{
		return ( lhs.x > rhs.x && lhs.y > rhs.y /*&& lhs.z > rhs.z*/ );
	}
	inline bool operator >= (const Point3 & lhs, const Point3 & rhs)
	{
		return ( lhs.x >= rhs.x && lhs.y >= rhs.y /*&& lhs.z >= rhs.z*/ );
	}
	inline bool operator < (const Point3 & lhs, const Point3 & rhs)
	{
		return ( lhs.x < rhs.x && lhs.y < rhs.y /*&& lhs.z < rhs.z*/ );
	}
	inline bool operator <= (const Point3 & lhs, const Point3 & rhs)
	{
		return ( lhs.x <= rhs.x && lhs.y <= rhs.y /*&& lhs.z <= rhs.z*/ );
	}

	/** rectangle (2D)
	*/
	struct YAKE_BASE_API Rectangle {
		Rectangle(const Rectangle & rect) : min(rect.min), max(rect.max)
		{}
		Rectangle(const Point3 & minimum, const Point3 & maximum) : min(minimum), max(maximum)
		{}
		Rectangle() : min(Point3::kZero), max(Point3::kZero)
		{}

		Point3	min,max;

		bool Rectangle::intersects(const Rectangle & rect) const
		{
			//fixme
			YAKE_ASSERT( 1==0 );
			return false;
		}

		bool Rectangle::contains(const Point3 & point) const
		{
			return (point >= min && point <= max);
		}

		void Rectangle::clipLimit(const Rectangle & frame)
		{
			if (frame.min.x > max.x) return;
			if (frame.max.x < min.x) return;
			if (frame.min.y > max.y) return;
			if (frame.max.y < min.y) return;

			if (frame.min.x > min.x) // x
				min.x = frame.min.x;
			if (frame.max.x < max.x)
				max.x = frame.max.x;
			if (frame.min.y > min.y) // y
				min.y = frame.min.y;
			if (frame.max.y < max.y)
				max.y = frame.max.y;
		}

		Rectangle& operator = (const Rectangle & rhs)
		{
			min = rhs.min;
			max = rhs.max;
			return *this;
		}
		real getWidth()
		{ return (max.x - min.x); }
		real getHeight()
		{ return (max.y - min.y); }
	};

}
}
#endif
