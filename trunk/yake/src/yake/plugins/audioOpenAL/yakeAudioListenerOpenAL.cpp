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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#include <yake/plugins/audioOpenAL/yakePCH.h>
#include <yake/plugins/audioOpenAL/yakeAudioSystemOpenAL.h>

namespace yake {
namespace audio {

	ListenerOpenAL::ListenerOpenAL()
	{
		mListener = new openalpp::Listener();
	}

	ListenerOpenAL::~ListenerOpenAL()
	{
	}

	void ListenerOpenAL::setOrientation( const Quaternion & orientation )
	{
		YAKE_ASSERT( mListener.valid() );
		Vector3 lookAt = orientation * Vector3(0,0,1);	//FIXME: Set these vectors globally in Yake !?
		Vector3 up = orientation * Vector3(0,1,0);
		mListener->setOrientation( lookAt.x, lookAt.y, lookAt.z, up.x, up.y, up.z );

		mCachedOrientation = orientation;
	}

	Quaternion ListenerOpenAL::getOrientation() const
	{
		// avoid the costy calculations below for many calls to this function by caching the orientation.
		return mCachedOrientation;

		/*
		// get y and z axes
		float lookAtX, lookAtY, lookAtZ, upX, upY, upZ;
		mListener->getOrientation( lookAtX, lookAtY, lookAtZ, upX, upY, upZ );
		Vector3 yAxis = Vector3( upX, upY, upZ );
		Vector3 zAxis = Vector3( lookAtX, lookAtY, lookAtZ );

		// get orientation from axes
		Quaternion orientation;
		orientation.FromAxes( zAxis.dotProduct( yAxis ), yAxis, zAxis );
		return orientation;*/
	}

	void ListenerOpenAL::setPosition( const Vector3 & position )
	{
		YAKE_ASSERT( mListener.valid() );
		mListener->setPosition( position.x, position.y, position.z );
	}

	Vector3 ListenerOpenAL::getPosition() const
	{
		YAKE_ASSERT( mListener.valid() );
		float x,y,z;
		mListener->getPosition( x, y, z );
		return Vector3( x, y, z );
	}

	void ListenerOpenAL::setVelocity( const Vector3 & velocity )
	{
		YAKE_ASSERT( mListener.valid() );
		mListener->setVelocity( velocity.x, velocity.y, velocity.z );
	}

	Vector3 ListenerOpenAL::getVelocity() const
	{
		YAKE_ASSERT( mListener.valid() );
		float x,y,z;
		mListener->getVelocity( x, y, z );
		return Vector3( x, y, z );
	}

}
}
