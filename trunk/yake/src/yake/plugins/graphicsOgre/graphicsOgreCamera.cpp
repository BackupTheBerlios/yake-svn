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
#include <yake/plugins/graphicsOgre/yakePCH.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCamera.h>

namespace yake {
namespace graphics {
namespace ogre3d {

	//------------------------------------------------------
	OgreCamera::OgreCamera( Ogre::SceneManager * sceneMgr ) : mSceneMgr( sceneMgr ), mCam( 0 )
	{
		YAKE_ASSERT( sceneMgr ).debug("need a scene manager!");
		mCam = mSceneMgr->createCamera( yake::base::uniqueName::create("sn_") );
		YAKE_ASSERT( mCam ).warning("Couldn't create a camera!");
		mCam->setPosition( 0, 0, 0 );

		// used for shadows
		// incase infinite far distance is not supported
		//mCam->setFarClipDistance(100000);
	}

	//------------------------------------------------------
	OgreCamera::~OgreCamera()
	{
		if ( mCam )
			mSceneMgr->removeCamera( mCam->getName() );
	}

	//------------------------------------------------------
	void OgreCamera::setPosition( const Vector3 & position )
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		mCam->setPosition( VEC_YAKE2OGRE( position ) );
	}

	//------------------------------------------------------
	Vector3 OgreCamera::getPosition() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return VEC_OGRE2YAKE( mCam->getPosition() );
	}

	//------------------------------------------------------
	void OgreCamera::setOrientation( const Quaternion & orientation )
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		mCam->setOrientation( QUAT_YAKE2OGRE( orientation ) );
	}

	//------------------------------------------------------
	Quaternion OgreCamera::getOrientation() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return QUAT_OGRE2YAKE( mCam->getOrientation() );
	}

	//------------------------------------------------------
	void OgreCamera::setDirection(const Vector3 & vec)
	{
		Movable::setDirection( -vec );
	}

	//------------------------------------------------------
	Ogre::ProjectionType yake2Ogre( const ICamera::ProjectionType type )
	{
		switch (type)
		{
		case ICamera::PT_ORTHOGRAPHIC: 
			return Ogre::PT_ORTHOGRAPHIC;

		case ICamera::PT_PERSPECTIVE:
			return Ogre::PT_PERSPECTIVE;
		
		default:
			return Ogre::PT_PERSPECTIVE;
		}
	}

	//------------------------------------------------------
	ICamera::ProjectionType ogre2Yake( const Ogre::ProjectionType type )
	{
		switch (type)
		{
		case Ogre::PT_ORTHOGRAPHIC: 
			return ICamera::PT_ORTHOGRAPHIC;

		case Ogre::PT_PERSPECTIVE:
			return ICamera::PT_PERSPECTIVE;
		
		default:
			return ICamera::PT_PERSPECTIVE;
		}
	}

	//------------------------------------------------------
	void OgreCamera::setProjectionType( const ProjectionType type )
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		mCam->setProjectionType( yake2Ogre( type ) );			
	}

	//------------------------------------------------------
	ICamera::ProjectionType OgreCamera::getProjectionType() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return ogre2Yake( mCam->getProjectionType() );
	}

	//------------------------------------------------------
	void OgreCamera::setNearClipDistance( real clipDistance )
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		mCam->setNearClipDistance( clipDistance );
	}
	
	//------------------------------------------------------
	real OgreCamera::getNearClipDistance() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return mCam->getNearClipDistance();
	}
	
	//------------------------------------------------------
	void OgreCamera::setFarClipDistance( real clipDistance )
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		mCam->setFarClipDistance( clipDistance );
	}

	//------------------------------------------------------
	real OgreCamera::getFarClipDistance() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return mCam->getFarClipDistance();
	}

	//------------------------------------------------------
	void OgreCamera::setFOV( real cameraFOV )
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		mCam->setFOVy( Ogre::Radian(cameraFOV) );
	}

	//------------------------------------------------------
	real OgreCamera::getFOV() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return mCam->getFOVy().valueRadians();
	}

	//------------------------------------------------------
	void OgreCamera::setAspectRatio( real aspectRatio )
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		mCam->setAspectRatio( aspectRatio );
	}

	//------------------------------------------------------
	real OgreCamera::getAspectRatio() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return mCam->getAspectRatio();
	}

}
}
}
