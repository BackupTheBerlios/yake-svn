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
#ifndef INC_YAKE_GRAPHICOGREPARTICLESYSTEM_H
#define INC_YAKE_GRAPHICOGREPARTICLESYSTEM_H

namespace yake {
namespace graphics {

	class OgreParticleSystem : public graphics::IParticleSystem
	{
	public:
// for Ogre 1.1.0 "Dagon" compatibility
#if OGRE_VERSION_MINOR >= 1 
		OgreParticleSystem( Ogre::SceneManager& rSMgr, const String& rPSTemplateName);
#else
		OgreParticleSystem( Ogre::ParticleSystemManager& rPSMgr, const String& rPSTemplateName);
#endif
		virtual ~OgreParticleSystem();

		virtual void setVisible( bool visible );
		virtual size_t getNumEmitters() const;
		virtual void setEmissionRate( const size_t emitter, const real particlesPerSec );
		virtual real getEmissionRate( const size_t emitter ) const;
		virtual void setMinVelocity( const size_t emitter, const real vel );
		virtual real getMinVelocity( const size_t emitter ) const;
		virtual void setMaxVelocity( const size_t emitter, const real vel );
		virtual real getMaxVelocity( const size_t emitter ) const;

		Ogre::ParticleSystem* getParticleSystem_() const
		{
			return mParticleSys;
		}

	protected:
// for Ogre 1.1.0 "Dagon" compatibility
#if OGRE_VERSION_MINOR >= 1 
		Ogre::SceneManager&	mSceneMgr;
#else
		Ogre::ParticleSystemManager&	mParticleSysMgr;
#endif
		Ogre::ParticleSystem*			mParticleSys;
	};

} // graphics
} // yake

#endif

