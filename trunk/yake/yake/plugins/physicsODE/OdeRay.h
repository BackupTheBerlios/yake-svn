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
#ifndef ODE_RAY_H
#define ODE_RAY_H

#include <yake/plugins/physicsODE/yakePrerequisites.h>

namespace yake
{	
namespace physics
{
	class OdeWorld;
	
	//-----------------------------------------------------
	class YAKE_PHYSICSODE_API OdeRay : public Movable
	{
	public:
		OdeRay(OdeWorld* world, const real length);
		virtual ~OdeRay();
		void collideWith( dGeomID geomOther );

		real length() const;
		bool intersects() const;
		real intersectionDepth() const;
		Vector3 intersectionPoint() const;
		Vector3 intersectionNormal() const;

		virtual void setPosition( Vector3 const& rPosition);
		virtual Vector3 getPosition() const;

		virtual void setOrientation( Quaternion const& rOrientation );
		virtual Quaternion getOrientation() const;

		void setDirection(const Vector3&);
		Vector3 getDirection() const;

		void ignore(dGeomID);

		void _clearIntersection()
		{
			mIntersectionDepth = mLength;
			mIntersects = false;
		}
	private:
		void onPreStep()
		{
			_clearIntersection();
		}
		yake::SignalConnection	mPreStepConn;
		dRay*		mRay;
		OdeWorld*	mWorld;
		real		mLength;
		mutable Vector3	mPosition;
		mutable Vector3	mDirection;

		bool		mIntersects;
		real		mIntersectionDepth;
		Vector3		mIntersectionPoint;
		Vector3		mIntersectionNormal;

		typedef std::deque<dGeomID> GeomIdList;
		GeomIdList	mIgnoreGeoms;
	};

} // physics
} // yake

#endif
