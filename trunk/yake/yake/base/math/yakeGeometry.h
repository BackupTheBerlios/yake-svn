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
namespace base {
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
		\todo move this into math?
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
}
#endif
