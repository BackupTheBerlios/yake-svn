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
#ifndef __TERRAIN_PHYS_MANAGER
#define  __TERRAIN_PHYS_MANAGER

#include <yake/base/yakePCH.h>

#include <Ogre.h>

#include <OgrePagingLandScapeSceneManager.h>
#include <OgrePagingLandScapeCallBackEvent.h>
#include <OgrePagingLandScapeListener.h>
#include <OgreDefaultHardwareBufferManager.h>
#include <OgrePagingLandScapePageManager.h>

#include <yapp/plugins/terrainPhysMgr/TerrainOdeActor.h>

#include <yake/plugins/graphicsOgre/yakeGraphicsSystem.h>
#include <yapp/model/yakeGraphical.h>

namespace yake
{
namespace physics
{	
	class IWorld;
	class IActor;
	class OdeWorld;
	class TerrainCollisionDetectionActor;
	class TerrainCollisionActor;
		
	class PLSMTerrainPhysicsManager
	{
		protected:
			void pageLoaded( Ogre::PagingLandscapeEvent* event);
			void tileLoaded( Ogre::PagingLandscapeEvent* event);
			
			void processAllTilesInPage( Ogre::PagingLandScapePage * page, int pageX, int pageZ);
			
			void createCollisionBoundingBox( Ogre::PagingLandScapeTile* tile);
			
			void convertIndexDataForOde( Ogre::IndexData* indexData, TriangleMeshDesc::IndexVector & destBuffer) ;
			
		public: 
			PLSMTerrainPhysicsManager( );
			~PLSMTerrainPhysicsManager();
	
			void init( Ogre::PagingLandScapeSceneManager * _sceneManager, IWorld *  _odeWorld, 
				   graphics::IWorld* graphicsWorld = 0);
			
			void processTile( Ogre::PagingLandScapeTileInfo * tileInfo);
	
			real getHeightAt( const math::Vector3& position);
			real getHeightAt( const Ogre::Vector3& position);
			
			Ogre::AxisAlignedBox calcActorAABB( IActor * _actor);
		
			void placeActorOnTerrain( const math::Vector3 & position,  IActor * _actor);
			
		protected:
			Ogre::PagingLandScapeSceneManager * sceneManager;
			
			Ogre::PagingLandscapeDelegate * 	pageLoadListener;		
			Ogre::PagingLandscapeDelegate * 	tileLoadListener;		
			
			Ogre::Ray 				ray;
			Ogre::RaySceneQuery * 			rayQuery;
			
			OdeWorld *	odeWorld;
			
			TerrainCollisionDetectionActor * 	terrainCollisionDetectionActor;
			
			graphics::IWorld* graphicsWorld;
			
			typedef std::list<Ogre::PagingLandScapeTile*> TileList;
			typedef TileList::iterator TileListIterator;
			
			TileList 				alreadyLoadedCollisionTiles;
			TileList				alreadyLoadedCollisionDetectionTiles;
	};
	
 	typedef yake::base::templates::SingletonHolder< PLSMTerrainPhysicsManager, 
	 		yake::base::templates::CreateUsingNew> TerrainPhysicsManager;
}
}
#endif // __TERRAIN_PHYS_MANAGER
