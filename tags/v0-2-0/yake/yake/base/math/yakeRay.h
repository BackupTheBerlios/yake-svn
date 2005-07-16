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
#ifndef YAKE_BASE_MATH_RAY_H
#define YAKE_BASE_MATH_RAY_H

// Yake
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
#include "yakeMath.h"
#include "yakeVector3.h"
#include "yakePlane.h"

namespace yake {
namespace math {

	class YAKE_BASE_API Ray
	{
	public:
		Ray( const Vector3 & rOrigin, const Vector3 & rDirection ) : mOrigin( rOrigin ), mDirection( rDirection )
		{
		}
		Vector3 getOrigin() const
		{
			return mOrigin;
		}
		Vector3 getDirection() const
		{
			return mDirection;
		}
		void setOrigin( const Vector3 & rOrigin )
		{
			mOrigin = rOrigin;
		}
		void setDirection( const Vector3 & rDirection )
		{
			mDirection = rDirection;
		}
		Vector3 getPoint( const real s ) const
		{
			return (mOrigin + (mDirection * s));
		}
		std::pair<bool, real> intersects(const Plane& plane) const;

	private:
		Vector3		mOrigin;
		Vector3		mDirection;
	};

}
}

#endif
