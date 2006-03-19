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
#ifndef YAKE_TERRAIN_ODESHAPES_H
#define YAKE_TERRAIN_ODESHAPES_H

#include <yapp/plugins/terrainPhysMgr/TerrainOdePrerequisites.h>
#include <yake/plugins/physicsODE/OdeShapes.h>

#include "OgreRenderQueue.h"
#include "OgreRenderable.h"
#include "OgreMovableObject.h"

#include "OgrePagingLandScapePrerequisites.h"
#include "OgrePagingLandScapeRenderable.h"

#include "OgrePagingLandScapeTile.h"
#include "OgrePagingLandScapeTileInfo.h"

namespace yake {
namespace physics {

	class YAKE_PHYSICSPLSM_API OdeTerrainCollisionBox : public OdeBox
	{
		public:
			OdeTerrainCollisionBox( dSpace* pSpace, OdeActor* pOwner, real sizex, real sizey, real sizez, Ogre::PagingLandScapeTileInfo * _tileInfo) : 
				OdeBox( pSpace, pOwner, sizex, sizey, sizez)
			{
				tileInfo.pageX = _tileInfo->pageX;
				tileInfo.pageZ = _tileInfo->pageZ;
				tileInfo.tileX = _tileInfo->tileX;
				tileInfo.tileZ = _tileInfo->tileZ;
			};
							
			Ogre::PagingLandScapeTileInfo * getTileInfo() { return &tileInfo;};
			
		protected:
					
			Ogre::PagingLandScapeTileInfo 	tileInfo;						
	};
	
}
}
#endif //YAKE_TERRAIN_ODESHAPES_H
