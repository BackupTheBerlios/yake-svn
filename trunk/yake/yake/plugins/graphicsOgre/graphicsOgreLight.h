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
#ifndef INC_YAKE_GRAPHICOGRELIGHT_H
#define INC_YAKE_GRAPHICOGRELIGHT_H

namespace yake {
namespace graphics {
namespace ogre3d {

	class OgreLight : public graphics::ILight
	{
	public:
		OgreLight( Ogre::SceneManager * sceneMgr );
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

		virtual void setDiffuseColour( const Color& colour );
		virtual void setSpecularColour( const Color& colour );
		
		virtual void setEnabled( bool enabled );
		virtual void setCastsShadow( bool enabled );

		virtual void setPosition( const Vector3 & position );
		virtual Vector3 getPosition() const;
		virtual void setOrientation( const Quaternion & orientation );
		virtual Quaternion getOrientation() const;
		virtual void setDirection( const Vector3& direction );
		virtual Vector3 getDirection() const;

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
