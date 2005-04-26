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
#include <yake/plugins/graphicsOgre/graphicsOgreSkeleton.h>
#include <yake/plugins/graphicsOgre/graphicsOgreEntity.h>

namespace yake {
namespace graphics {
namespace ogre3d {

	//------------------------------------------------------
	OgreEntity::OgreEntity( ::Ogre::SceneManager * sceneMgr, const String & mesh ) : mSceneMgr( sceneMgr ), mEntity( 0 )
	{
		YAKE_ASSERT( mSceneMgr ).debug("need a scene manager!");
		YAKE_TRY
			mEntity = mSceneMgr->createEntity( Ogre::String(yake::uniqueName::create("en_").c_str()), Ogre::String(mesh.c_str()) );
		YAKE_CATCH_OGRE_RETHROW
	}

	//------------------------------------------------------
	OgreEntity::~OgreEntity()
	{
		YAKE_TRY
		if (mEntity && mSceneMgr)
		{
			mSceneMgr->removeEntity( mEntity );
			mEntity = 0;
		}
		YAKE_CATCH_OGRE_RETHROW
	}

	//------------------------------------------------------
	void OgreEntity::setVisible( bool visible )
	{
		if (mEntity)
			mEntity->setVisible( visible );
	}

	//------------------------------------------------------
	void OgreEntity::setMaterial( const String & materialName )
	{
		if (mEntity)
			mEntity->setMaterialName( materialName );
	}

	//------------------------------------------------------
	void OgreEntity::setSubEntityMaterial( const String & subEntity, const String & materialName )
	{
		if (mEntity)
		{
			Ogre::SubEntity* pSubEntity = mEntity->getSubEntity( subEntity );
			if (pSubEntity)
                pSubEntity->setMaterialName( materialName );
		}
	}

	//------------------------------------------------------
	ISkeleton* OgreEntity::getSkeleton() const
	{
		YAKE_ASSERT( mEntity ).debug( "need an entity" );
		if ( mEntity->hasSkeleton() )
			return new OgreSkeleton( *mEntity );
		else
			return NULL;
	}

	//------------------------------------------------------
	void OgreEntity::setCastsShadow( bool castsShadow )
	{
		YAKE_ASSERT( mEntity ).debug( "need an entity" );
		if (mEntity)
			mEntity->setCastShadows( castsShadow );
	}


	//------------------------------------------------------
	String OgreEntity::getTag() const
	{
		YAKE_ASSERT( mEntity ).debug( "need an entity" );
		return String( mEntity->getName().c_str() );
	}

	//------------------------------------------------------
	void OgreEntity::getTag(String& tag)
	{
		YAKE_ASSERT( mEntity ).debug( "need an entity" );
		tag = mEntity->getName().c_str();
	}
}
}
}
