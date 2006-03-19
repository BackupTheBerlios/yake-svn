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
#include <yake/plugins/graphicsOgre/yakePCH.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCamera.h>

namespace yake {
namespace graphics {
namespace ogre3d {

	//------------------------------------------------------
	OgreCamera::OgreCamera( Ogre::SceneManager* sceneMgr ) : mSceneMgr( sceneMgr ), mCam( 0 )
	{
		YAKE_ASSERT( sceneMgr ).debug("need a scene manager!");
		mCam = mSceneMgr->createCamera( yake::uniqueName::create("sn_") );
		
		YAKE_ASSERT( mCam ).warning("Couldn't create a camera!");
		
		// we want free look as default
		mCam->setFixedYawAxis( false );
		
		mCam->setPosition( 0, 0, 0 );

		// used for shadows
		// incase infinite far distance is not supported
		//mCam->setFarClipDistance(100000);
	}

	//------------------------------------------------------
	OgreCamera::OgreCamera( Ogre::Camera* cam, Ogre::SceneManager * sceneMgr ) : mSceneMgr( sceneMgr ), mCam( cam )
	{
		YAKE_ASSERT( sceneMgr ).debug("need a scene manager!");
		YAKE_ASSERT( mCam ).warning("Couldn't create a camera!");
	}

	//------------------------------------------------------
	OgreCamera::~OgreCamera()
	{
	    if ( mCam )
// for Ogre 1.1.0 "Dagon" compatibility
#if OGRE_VERSION_MINOR >= 1 
		mSceneMgr->destroyCamera( mCam->getName() );
#else 
		mSceneMgr->removeCamera( mCam->getName() );
#endif
	}

	//------------------------------------------------------
	void OgreCamera::setPosition( const math::Vector3& position )
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		mCam->setPosition( VEC_YAKE2OGRE( position ) );
	}

	//------------------------------------------------------
	math::Vector3 OgreCamera::getPosition() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return VEC_OGRE2YAKE( mCam->getPosition() );
	}

	//------------------------------------------------------
	math::Vector3 OgreCamera::getDerivedPosition() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return VEC_OGRE2YAKE( mCam->getDerivedPosition() );
	}

	//------------------------------------------------------
	void OgreCamera::setOrientation( const math::Quaternion & orientation )
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		mCam->setOrientation( QUAT_YAKE2OGRE( orientation ) );
	}

	//------------------------------------------------------
	math::Quaternion OgreCamera::getOrientation() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return QUAT_OGRE2YAKE( mCam->getOrientation() );
	}

	//------------------------------------------------------
	math::Quaternion OgreCamera::getDerivedOrientation() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return QUAT_OGRE2YAKE( mCam->getDerivedOrientation() );
	}

	//------------------------------------------------------
	void OgreCamera::setDetailLevel( SceneDetailLevel sd )
	{
		mCam->setPolygonMode( Ogre::PolygonMode( sd ) );
	}
	
	//------------------------------------------------------
	SceneDetailLevel OgreCamera::getDetailLevel() const
	{
		return SceneDetailLevel( mCam->getPolygonMode() );
	}
	
	//------------------------------------------------------
	void OgreCamera::setDirection( const math::Vector3& rVec )
	{
		//@todo cleaner implementation
		mCam->setDirection( VEC_YAKE2OGRE( rVec ) );
	}
	
	//------------------------------------------------------
	math::Vector3 OgreCamera::getDirection() const
	{
		return VEC_OGRE2YAKE( mCam->getDirection() );
	}
	
	//------------------------------------------------------
	math::Vector3 OgreCamera::getUp() const
	{
		return VEC_OGRE2YAKE( mCam->getUp() );
	}
	
	//------------------------------------------------------
	math::Vector3 OgreCamera::getRight() const
	{
		return VEC_OGRE2YAKE( mCam->getRight() );
	}
	
	//------------------------------------------------------
	void OgreCamera::setFixedYawEnabled( bool enabled )
	{
		mCam->setFixedYawAxis( enabled );
	}
	
	//------------------------------------------------------
	void OgreCamera::setFixedYawAxis( const math::Vector3& yawAxis )
	{
		mCam->setFixedYawAxis( true, VEC_YAKE2OGRE( yawAxis ) );
	}
	
	//------------------------------------------------------
	void OgreCamera::moveRelative( const math::Vector3& rVec )
	{
		mCam->moveRelative( VEC_YAKE2OGRE( rVec ) );
	}
	
	//------------------------------------------------------
	void OgreCamera::lookAt( const math::Vector3& target )
	{
		mCam->lookAt( VEC_YAKE2OGRE( target ) );
	}

	//------------------------------------------------------
	void OgreCamera::yaw( const real degrees )
	{
		mCam->yaw( Ogre::Degree( degrees ) );
	}
	
	//------------------------------------------------------
	void OgreCamera::roll( const  real degrees )
	{
		mCam->roll( Ogre::Degree( degrees ) );
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
	math::Matrix4 OgreCamera::getProjectionMatrix() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
// for Ogre 1.1.0 "Dagon" compatibility
#if OGRE_VERSION_MINOR >= 1 
		return MATRIX4_OGRE2YAKE( mCam->getProjectionMatrix() );
#else
		return MATRIX4_OGRE2YAKE( mCam->getStandardProjectionMatrix() );
#endif
	}

	//------------------------------------------------------
	math::Matrix4 OgreCamera::getViewMatrix() const
	{
		YAKE_ASSERT( mCam ).debug("need a camera!");
		return MATRIX4_OGRE2YAKE( mCam->getViewMatrix() );
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
