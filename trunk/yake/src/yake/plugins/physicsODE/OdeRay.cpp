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
#include <yake/plugins/physicsODE/yakePCH.h>
#include <yake/plugins/physicsODE/OdeWorld.h>
#include <yake/plugins/physicsODE/OdeRay.h>

namespace yake {
namespace physics {
	using namespace yake::math;

	//-----------------------------------------------------
	OdeRay::OdeRay(OdeWorld* world, const real length) :
		mWorld(world),
		mLength(length)
	{
		YAKE_ASSERT( mWorld );
		YAKE_ASSERT( length > 0 );
		mRay = new dRay( world->_getOdeSpace()->id(), length );
		mRay->setData( this );
		this->setDirection( -Vector3::kUnitY );
		mPreStepConn = mWorld->subscribeToPreStep( boost::bind(&OdeRay::onPreStep,this) );
	}
	OdeRay::~OdeRay()
	{
		mPreStepConn.disconnect();
		YAKE_SAFE_DELETE( mRay );
	}
	void OdeRay::ignore(dGeomID id)
	{
		mIgnoreGeoms.push_back( id );
	}
	void OdeRay::collideWith( dGeomID geomOther )
	{
		GeomIdList::const_iterator itFind = std::find( mIgnoreGeoms.begin(), mIgnoreGeoms.end(), geomOther );
		if (itFind != mIgnoreGeoms.end())
			return;
		dContact contact;
		int numCollided = dCollide( mRay->id(), geomOther, 1, &contact.geom, sizeof( dContact ) );
		YAKE_ASSERT( numCollided >= 0 && numCollided <= 1 );
		if (numCollided < 1)
			return;
		if (contact.geom.depth < mIntersectionDepth)
		{
			mIntersects = true;
			mIntersectionDepth = real(contact.geom.depth);
			mIntersectionPoint = Vector3( real(contact.geom.pos[0]), real(contact.geom.pos[1]), real(contact.geom.pos[2]) );
			// As we have passed dCollide the ray as the first parameter, the normal
			// is oriented correctly for ray deflection from the surface.
			mIntersectionNormal = Vector3( real(contact.geom.normal[0]), real(contact.geom.normal[1]), real(contact.geom.normal[2]) );
		}
	}
	real OdeRay::length() const
	{
		return mLength;
	}
	bool OdeRay::intersects() const
	{
		return mIntersects;
	}
	real OdeRay::intersectionDepth() const
	{
		return mIntersectionDepth;
	}
	Vector3 OdeRay::intersectionPoint() const
	{
		return mIntersectionPoint;
	}
	Vector3 OdeRay::intersectionNormal() const
	{
		return mIntersectionNormal;
	}
    void OdeRay::setPosition( const Vector3& rPosition )
    {
		mPosition = rPosition;
		if (mRay)
			mRay->setPosition( mPosition.x, mPosition.y, mPosition.z );
    }
	void OdeRay::setDirection(const Vector3& dir)
	{
		mDirection = dir;
		if (mRay)
			mRay->set( mPosition.x, mPosition.y, mPosition.z, mDirection.x, mDirection.y, mDirection.z );
	}
	Vector3 OdeRay::getDirection() const
	{
		return mDirection;
	}
    void OdeRay::setOrientation( const Quaternion& rOrientation )
    {
		this->setDirection( rOrientation * Vector3::kUnitZ );
    }
    Vector3 OdeRay::getPosition() const
    {
		if (mRay)
		{
			const dReal* pos = mRay->getPosition();
			mPosition = Vector3(real(pos[0]),real(pos[1]),real(pos[2]));
		}
		return mPosition;
    }
    Quaternion OdeRay::getOrientation() const
    {
		if (mRay)
		{
			dQuaternion q;
			mRay->getQuaternion(q);
			return Quaternion( real(q[0]), real(q[1]), real(q[2]), real(q[3]) );
		}
		return Quaternion::kIdentity;
    }

} // physics
} // yake

