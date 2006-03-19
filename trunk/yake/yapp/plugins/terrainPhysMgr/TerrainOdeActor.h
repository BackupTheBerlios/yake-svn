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
#ifndef TERRAIN_ACTOR_H
#define TERRAIN_ACTOR_H

#include <map>
#include <vector>

#include <yake/plugins/physicsODE/yakePCH.h>

#include <yake/plugins/physicsODE/OdeWorld.h>
#include <yake/plugins/physicsODE/OdeBody.h>
#include <yake/plugins/physicsODE/OdeActor.h>

#include "TerrainPhysicsManager.h"

#include "OgrePagingLandScapeTileInfo.h"

using namespace std;

namespace yake
{
	namespace physics
	{
		class OdeWorld;
		class OdeGeom;
		class Ogre::PagingLandScapeTileInfo;
		
		class YAKE_PHYSICSODE_API TerrainCollisionDetectionActor : public OdeActor
		{
			public:
				TerrainCollisionDetectionActor( OdeWorld* pOdeWorld );
				virtual ~TerrainCollisionDetectionActor();

				void createTerrainCollisionDetectionShape( const math::Vector3& rDimensions,
					        const math::Vector3& rPosition,
				       		Ogre::PagingLandScapeTileInfo* tileInfo );

				void createTerrainCollisionShape(  const math::Vector3& rPosition, TriangleMeshDesc::VertexVector & vertices,
						TriangleMeshDesc::IndexVector & indexes);
				
				void onCollision( const ActorCollisionInfo & collisionInfo );
		};
		//------------------------------------------------------------------------------------
	} // physics
} // yake

#endif //	TERRAIN_ACTOR_H
