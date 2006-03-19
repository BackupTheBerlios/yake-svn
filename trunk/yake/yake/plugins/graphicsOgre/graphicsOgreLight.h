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
#ifndef INC_YAKE_GRAPHICOGRELIGHT_H
#define INC_YAKE_GRAPHICOGRELIGHT_H

namespace yake {
namespace graphics {
namespace ogre3d {

	class OgreLight : public graphics::ILight
	{
	public:
		OgreLight( Ogre::SceneManager* sceneMgr );
		OgreLight( Ogre::Light*, Ogre::SceneManager* sceneMgr );
		virtual ~OgreLight();

		virtual void setType( const LightType type );
		virtual LightType getType() const;

		virtual void setAttenuation( real range, real constant, real linear, real quadratic );
		virtual real getAttenuationRange() const;
		virtual real getAttenuationConstFactor() const;
		virtual real getAttenuationLinearFactor() const;
		virtual real getAttenuationQuadraticFactor() const;

		virtual void setSpotlightRange( real innerAngle, real outerAngle, real falloff );
		virtual real getSpotlightInnerAngle() const;
		virtual real getSpotlightOuterAngle() const;
		virtual real getSpotlightFalloff() const;

		virtual void setDiffuseColour( const math::Color& colour );
		virtual void setSpecularColour( const math::Color& colour );

		virtual void setEnabled( bool enabled );
		virtual void setCastsShadows( bool enabled );

		virtual void setPosition( const math::Vector3 & position );
		virtual math::Vector3 getPosition() const;
		virtual void setOrientation( const math::Quaternion & orientation );
		virtual math::Quaternion getOrientation() const;
		virtual void setDirection( const math::Vector3& direction );
		virtual math::Vector3 getDirection() const;

		virtual const String& getName() { return mLight->getName(); }

		Ogre::Light* getLight_() const
		{ return mLight; }
	protected:
		Ogre::SceneManager* mSceneMgr;
		Ogre::Light*		mLight;
	};

}
}
}

#endif

