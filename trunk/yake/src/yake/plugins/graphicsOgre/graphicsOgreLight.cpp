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
#include <yake/plugins/graphicsOgre/yakeGraphicsSystem.h>
#include <yake/plugins/graphicsOgre/graphicsOgreLight.h>

namespace yake {
namespace graphics {
namespace ogre3d {

	//------------------------------------------------------
	OgreLight::OgreLight( Ogre::SceneManager* pSceneMgr ) : mSceneMgr( pSceneMgr ), mLight( NULL )
	{
		YAKE_ASSERT( mSceneMgr ).debug("need a scene manager!");
		mLight = mSceneMgr->createLight( yake::base::uniqueName::create("sn_") );
		YAKE_ASSERT( mLight ).warning("Couldn't create a light!");
	}

	//------------------------------------------------------
	OgreLight::~OgreLight()
	{
		if ( mLight != NULL && mSceneMgr != NULL)
			mSceneMgr->removeLight( mLight->getName() );
	}

	//------------------------------------------------------
	Ogre::Light::LightTypes yake2Ogre( const ILight::LightType type )
	{
		switch (type)
		{
		case ILight::LT_POINT: 
			return Ogre::Light::LT_POINT;

		case ILight::LT_SPOT: 
			return Ogre::Light::LT_SPOTLIGHT;

		case ILight::LT_DIRECTIONAL: 
			return Ogre::Light::LT_DIRECTIONAL;

		default:
			return Ogre::Light::LT_POINT;
		}
	}

	//------------------------------------------------------
	ILight::LightType ogre2Yake( const Ogre::Light::LightTypes type )
	{
		switch (type)
		{
		case Ogre::Light::LT_POINT: 
			return ILight::LT_POINT;

		case Ogre::Light::LT_SPOTLIGHT: 
			return ILight::LT_SPOT;

		case Ogre::Light::LT_DIRECTIONAL: 
			return ILight::LT_DIRECTIONAL;
		
		default:
			return ILight::LT_POINT;
		}
	}

	//------------------------------------------------------
	void OgreLight::setType( const LightType type )
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		mLight->setType( yake2Ogre( type ) );
	}

	//------------------------------------------------------
	OgreLight::LightType OgreLight::getType() const
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		return ogre2Yake( mLight->getType() );
	}

	//------------------------------------------------------
	void OgreLight::setDiffuseColour( const Color& colour )
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		mLight->setDiffuseColour( COL_YAKE2OGRE( colour ) );
	}

	//------------------------------------------------------
	void OgreLight::setSpecularColour( const Color& colour )
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		mLight->setSpecularColour( COL_YAKE2OGRE( colour ) );
	}
	
	//------------------------------------------------------
	void OgreLight::setCastsShadow( bool enabled )
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		mLight->setCastShadows( true );
	}

	//------------------------------------------------------
	void OgreLight::setAttenuation( real range, real constant, real linear, real quadratic )
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		mLight->setAttenuation( range, constant, linear, quadratic );
	}

	//------------------------------------------------------
	real OgreLight::getAttenuationRange() const
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		return mLight->getAttenuationRange();
	}

	//------------------------------------------------------
	real OgreLight::getAttenuationConstFactor() const
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		return mLight->getAttenuationConstant();
	}

	//------------------------------------------------------
	real OgreLight::getAttenuationLinearFactor() const
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		return mLight->getAttenuationLinear();
	}

	//------------------------------------------------------
	real OgreLight::getAttenuationQuadraticFactor() const
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		return mLight->getAttenuationQuadric();
	}

	//------------------------------------------------------
	void OgreLight::setSpotlightRange( real innerAngle, real outerAngle, real falloff )
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		return mLight->setSpotlightRange( innerAngle, outerAngle, falloff );
	}

	//------------------------------------------------------
	real OgreLight::getSpotlightInnerAngle() const
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		return mLight->getSpotlightInnerAngle();
	}

	//------------------------------------------------------
	real OgreLight::getSpotlightOuterAngle() const
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		return mLight->getSpotlightOuterAngle();
	}
	
	//------------------------------------------------------
	real OgreLight::getSpotlightFalloff() const
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		return mLight->getSpotlightFalloff();
	}

	//------------------------------------------------------
	void OgreLight::setPosition( const Vector3& rPosition )
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		mLight->setPosition( VEC_YAKE2OGRE( rPosition ) );
	}

	//------------------------------------------------------
	Vector3 OgreLight::getPosition() const
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		return VEC_OGRE2YAKE( mLight->getPosition() );
	}

	//------------------------------------------------------
	void OgreLight::setOrientation( const Quaternion & orientation )
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		setDirection( orientation * Vector3(0,0,1) );
	}

	//------------------------------------------------------
	Quaternion OgreLight::getOrientation() const
	{
		YAKE_ASSERT( mLight ).debug( "not implemented..." );
		return Quaternion::kIdentity;
	}

	//------------------------------------------------------
	void OgreLight::setDirection( const Vector3& direction )
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		mLight->setDirection( VEC_YAKE2OGRE( direction ) );
	}

	//------------------------------------------------------
	Vector3 OgreLight::getDirection() const
	{
		YAKE_ASSERT( mLight ).debug( "need actual light!" );
		return VEC_OGRE2YAKE( mLight->getDirection() );
	}

} // ogre3d
} // graphics
} // yake