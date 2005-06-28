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
#include <yake/plugins/graphicsOgre/graphicsOgreSkeleton.h>
#include <yake/plugins/graphicsOgre/graphicsOgreEntity.h>

namespace yake {
namespace graphics {
namespace ogre3d {

	//------------------------------------------------------
	OgreSkeleton::OgreSkeleton( Ogre::Entity& rEntity ) : mEntity( rEntity )
	{
	}

	//------------------------------------------------------
	OgreSkeleton::~OgreSkeleton()
	{
	}

	//------------------------------------------------------
	void OgreSkeleton::enableAnimation( const String& rAnimName, bool enable )
	{
		Ogre::AnimationState* pAnimState = mEntity.getAnimationState( rAnimName );
		
		pAnimState->setEnabled( enable );
	}

	//------------------------------------------------------
	void OgreSkeleton::advanceAnimation( const String& rAnimName, real timeOffset )
	{
		Ogre::AnimationState* pAnimState = mEntity.getAnimationState( rAnimName );

		pAnimState->addTime( timeOffset );
	}

	//------------------------------------------------------
	void OgreSkeleton::advanceAllAnimations( real timeOffset )
	{
/*		Ogre::AnimationStateSet* pAnimStates = mEntity.getAllAnimationStates();
		Ogre::AnimationStateIterator i = pAnimStates->getAnimationStateIterator();
		
		while( i.hasMoreElements() )
		{
			Ogre::AnimationState* pAnimState = i.getNext();
			
			if ( pAnimState->getEnabled() )
				pAnimState->addTime( timeOffset );
		}*/
	}


	//------------------------------------------------------
	void OgreSkeleton::setAnimationWeight( const String& rAnimName, real weight )
	{
		YAKE_ASSERT( false ).debug( "feature NYI" );
	}

	//------------------------------------------------------
	void OgreSkeleton::attachEntityToBone( const String& rBoneName, IEntity* pEntity )
	{
		mEntity.attachObjectToBone( rBoneName, static_cast<OgreEntity*>( pEntity )->getEntity_() );
	}

} // ogre3d
} // graphics
} // yake
