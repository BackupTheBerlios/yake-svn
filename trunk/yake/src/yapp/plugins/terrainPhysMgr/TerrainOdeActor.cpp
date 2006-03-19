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

#include <algorithm>

#include <yake/base/yake.h>
#include <yake/physics/yakePhysicsPrerequisites.h>

#include <yapp/plugins/terrainPhysMgr/TerrainOdeActor.h>
#include <yapp/plugins/terrainPhysMgr/TerrainOdeShape.h>

namespace yake
{
	namespace physics
	{
		TerrainCollisionDetectionActor::TerrainCollisionDetectionActor ( OdeWorld* pOdeWorld ) : OdeActor( pOdeWorld, false )
		{
			this->mEnterCollisionSignal.connect( Bind1( &TerrainCollisionDetectionActor::onCollision, this ) );
		}
		//------------------------------------------------------------------------------------

		TerrainCollisionDetectionActor::~TerrainCollisionDetectionActor ()
		{}
		//------------------------------------------------------------------------------------

		void TerrainCollisionDetectionActor::createTerrainCollisionDetectionShape( const math::Vector3& rDimensions,
		        const math::Vector3& rPosition, Ogre::PagingLandScapeTileInfo* tileInfo )
		{
			OdeTerrainCollisionBox * pBox = new OdeTerrainCollisionBox( mOdeWorld->_getOdeSpace(),
			                                this,
			                                rDimensions.x,
			                                rDimensions.y,
			                                rDimensions.z,
			                                tileInfo );

			pBox->setPosition( rPosition );
			pBox->setOrientation( math::Quaternion::kIdentity );

			//set material NULL for awoiding collisions with this helper shape
			pBox->setMaterial( NULL );

			mShapes.push_back( SharedPtr<OdeTerrainCollisionBox>( pBox ) );
			
  			std::cerr << "Tile Ode BOX derived pos = " << pBox->getDerivedPosition() << std::endl;
			std::cerr << "Tile Ode BOX pos = " << pBox->getPosition()<< std::endl;
			std::cerr << "Tile Ode BOX dims = " << rDimensions << std::endl;
			std::cerr << "--------------------------------\n";
		}
		//------------------------------------------------------------------------------------

		void TerrainCollisionDetectionActor::createTerrainCollisionShape( const math::Vector3& rPosition, TriangleMeshDesc::VertexVector & vertices,
			TriangleMeshDesc::IndexVector & indexes)
		{
			std::cerr<<"Parent SN pos = "<<rPosition<<std::endl;
			std::cerr<<"vertex number = "<<vertices.size()<<std::endl;
			std::cerr<<"index number = "<<indexes.size()<<std::endl;
			
			TriangleMeshDesc meshDescriptor( vertices, indexes );

			IShape::TriMeshDesc * pTriMeshDesc = new IShape::TriMeshDesc( meshDescriptor );

			TriangleMeshId id = pTriMeshDesc->trimeshId_;

			// build Triangle mesh by TriMeshDesc
			if ( id == kTriangleMeshIdNone )
			{
				YAKE_LOG("Creating triangle mesh id");
				id = mOdeWorld->createTriangleMesh( pTriMeshDesc->trimesh_ );
				YAKE_ASSERT( id != kTriangleMeshIdNone );
			}
			
 			OdeTriMesh::MeshData data = mOdeWorld->getMeshDataById( id );
				
			//finaly create ODE trimesh object
 			OdeTriMesh * pMesh = new OdeTriMesh( mOdeWorld->_getOdeSpace(), this, data.id );
			YAKE_ASSERT( pMesh ).error( "Mesh with such id wasn't found!" );
			
			//setting material
			IMaterial* pMaterial =  this->getCreator()->getMaterial( "default" );

 			OdeMaterial * tmpMaterial = dynamic_cast< OdeMaterial*>( pMaterial);
			
			YAKE_ASSERT( tmpMaterial).error( "null material!" );

			pMesh->setMaterial( tmpMaterial);

			pMesh->setPosition( rPosition);
			pMesh->setOrientation( math::Quaternion::kIdentity );

			mShapes.push_back( SharedPtr<OdeGeom>( pMesh) );
		}
		//------------------------------------------------------------------------------------

		void TerrainCollisionDetectionActor::onCollision( const ActorCollisionInfo & collisionInfo )
		{	
// 			OdeActor * pA = dynamic_cast<OdeActor*>( collisionInfo.pThis );
//  			OdeActor * pB = dynamic_cast<OdeActor*>( collisionInfo.pOther );
			
// 			std::cerr<<"Actor "<<this->actorName<<" collided with actor "<<pB->actorName<<std::endl;
			
// 			TerrainCollisionDetectionActor * pA = dynamic_cast<TerrainCollisionDetectionActor*>( collisionInfo.pOther );
// 			TerrainCollisionDetectionActor * pB = dynamic_cast<TerrainCollisionDetectionActor*>( collisionInfo.pOther );
			
// 			if( pA || pB)
//  			{
//    				YAKE_LOG("AutoCollision detected!");
//  				return;
//  			}
  			
			OdeTerrainCollisionBox * terrainCollisionBbox = dynamic_cast< OdeTerrainCollisionBox *>( collisionInfo.pThisShape_ );

			if ( terrainCollisionBbox == NULL )
				terrainCollisionBbox = dynamic_cast< OdeTerrainCollisionBox *>( collisionInfo.pOtherShape_ );
			if( terrainCollisionBbox == NULL)
			{
//    				YAKE_LOG("Can't get proper collision bbox!" );
				return;
			}
			
			Ogre::PagingLandScapeTileInfo * tileInfo = terrainCollisionBbox->getTileInfo();

  			TerrainPhysicsManager::instance().processTile( tileInfo );
		}
		//------------------------------------------------------------------------------------
		
} // physics
} // yake
