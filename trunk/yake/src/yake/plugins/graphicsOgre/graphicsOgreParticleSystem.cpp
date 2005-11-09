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
#include <yake/plugins/graphicsOgre/yakeGraphicsWorld.h>
#include <yake/plugins/graphicsOgre/graphicsOgreParticleSystem.h>

namespace yake {
namespace graphics {

	//------------------------------------------------------
	OgreParticleSystem::OgreParticleSystem( ogre3d::GraphicalWorld& owningWorld, Ogre::ParticleSystemManager& rPSMgr, const String& rPSTempl ) : 
																ogre3d::OgreWrappedObject(owningWorld),
																mParticleSysMgr( rPSMgr ), mParticleSys( NULL )
	{
		mParticleSys = mParticleSysMgr.createSystem( uniqueName::create( "ps_" ), rPSTempl );

		YAKE_ASSERT( mParticleSys ).debug( "Failed to create particle system!" );
	}

	//------------------------------------------------------
	OgreParticleSystem::~OgreParticleSystem()
	{
		YAKE_ASSERT( mParticleSys );
		mParticleSysMgr.destroySystem( mParticleSys->getName() );
	}

	//------------------------------------------------------
	void OgreParticleSystem::setVisible( bool visible )
	{
		YAKE_ASSERT( mParticleSys );
		mParticleSys->setVisible( visible );
	}

	//------------------------------------------------------
	size_t OgreParticleSystem::getNumEmitters() const
	{
		YAKE_ASSERT( mParticleSys );
		return mParticleSys->getNumEmitters();
	}

	//------------------------------------------------------
	void OgreParticleSystem::setEmissionRate( const size_t emitter, const real particlesPerSec )
	{
		YAKE_ASSERT( mParticleSys );
		YAKE_ASSERT( mParticleSys->getNumEmitters() > emitter );
		Ogre::ParticleEmitter* pEmitter = mParticleSys->getEmitter(emitter);
		YAKE_ASSERT( pEmitter );
		pEmitter->setEmissionRate( particlesPerSec );
	}

	//------------------------------------------------------
	real OgreParticleSystem::getEmissionRate( const size_t emitter ) const
	{
		YAKE_ASSERT( mParticleSys );
		YAKE_ASSERT( mParticleSys->getNumEmitters() > emitter );
		Ogre::ParticleEmitter* pEmitter = mParticleSys->getEmitter(emitter);
		YAKE_ASSERT( pEmitter );
		return pEmitter->getEmissionRate();
	}

	//------------------------------------------------------
	void OgreParticleSystem::setMinVelocity( const size_t emitter, const real vel )
	{
		YAKE_ASSERT( mParticleSys );
		YAKE_ASSERT( mParticleSys->getNumEmitters() > emitter );
		Ogre::ParticleEmitter* pEmitter = mParticleSys->getEmitter(emitter);
		YAKE_ASSERT( pEmitter );
		pEmitter->setMinParticleVelocity( vel );
	}

	//------------------------------------------------------
	real OgreParticleSystem::getMinVelocity( const size_t emitter ) const
	{
		YAKE_ASSERT( mParticleSys );
		YAKE_ASSERT( mParticleSys->getNumEmitters() > emitter );
		Ogre::ParticleEmitter* pEmitter = mParticleSys->getEmitter(emitter);
		YAKE_ASSERT( pEmitter );
		return pEmitter->getMinParticleVelocity();
	}

	//------------------------------------------------------
	void OgreParticleSystem::setMaxVelocity( const size_t emitter, const real vel )
	{
		YAKE_ASSERT( mParticleSys );
		YAKE_ASSERT( mParticleSys->getNumEmitters() > emitter );
		Ogre::ParticleEmitter* pEmitter = mParticleSys->getEmitter(emitter);
		YAKE_ASSERT( pEmitter );
		return pEmitter->setMaxParticleVelocity(vel);
	}

	//------------------------------------------------------
	real OgreParticleSystem::getMaxVelocity( const size_t emitter ) const
	{
		YAKE_ASSERT( mParticleSys );
		YAKE_ASSERT( mParticleSys->getNumEmitters() > emitter );
		Ogre::ParticleEmitter* pEmitter = mParticleSys->getEmitter(emitter);
		YAKE_ASSERT( pEmitter );
		return pEmitter->getMaxParticleVelocity();
	}

} // graphics
} // yake
