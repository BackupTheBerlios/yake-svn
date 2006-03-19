#include <vector>

#include <yapp/plugins/terrainPhysMgr/TerrainPhysicsManager.h>

#include <yake/base/yake.h>
#include <yake/plugins/physicsODE/OdeWorld.h>
#include <yake/plugins/physicsODE/OdeBody.h>
#include <yake/plugins/physicsODE/OdeActor.h>
#include <yake/plugins/physicsODE/OdeShapes.h>

#include <yapp/plugins/terrainPhysMgr/TerrainOdeActor.h>
#include <yapp/plugins/terrainPhysMgr/TerrainOdeShape.h>

#include <yake/plugins/graphicsOgre/yakePCH.h>

#include "OgrePagingLandScapeOptions.h"
#include "OgrePagingLandScapeListener.h"
#include "OgrePagingLandScapeTile.h"
#include "OgrePagingLandScapeTileInfo.h"
#include "OgrePagingLandScapeIndexBuffer.h"
#include "OgrePagingLandScapeRenderable.h"
#include "OgrePagingLandScapeTileManager.h"
#include "OgrePagingLandScapeRaySceneQuery.h"

using namespace std;
using namespace Ogre;

namespace yake
{
	namespace physics
	{
		PLSMTerrainPhysicsManager::PLSMTerrainPhysicsManager( ) :
				sceneManager ( 0 ), pageLoadListener( 0 ), terrainCollisionDetectionActor( 0 ),  rayQuery(0),
				graphicsWorld( 0)
		{}
		//---------------------------------------------------------------------------------------------

		PLSMTerrainPhysicsManager::~PLSMTerrainPhysicsManager()
		{
			sceneManager->destroyQuery( rayQuery);
			sceneManager->setOption( "removeLoadPageListener", pageLoadListener );
			odeWorld->destroyActor( terrainCollisionDetectionActor );
		}
		//---------------------------------------------------------------------------------------------
		
		void PLSMTerrainPhysicsManager::init( Ogre::PagingLandScapeSceneManager * _sceneManager,
				IWorld *_odeWorld, graphics::IWorld* _graphicsWorld)
		{
			this->graphicsWorld = _graphicsWorld;
			
			YAKE_LOG("Inititalizing Terrain Physics Manager...");
			
			this->odeWorld = dynamic_cast<OdeWorld *>( _odeWorld );

			//add to physical world our new terrain actor
			terrainCollisionDetectionActor = new TerrainCollisionDetectionActor( odeWorld );

			odeWorld->addActor( terrainCollisionDetectionActor );

			sceneManager = _sceneManager;

			//create PageLoading event listener
			pageLoadListener = new Ogre::PagingLandscapeDelegate( &( *this ),
			                   &PLSMTerrainPhysicsManager::pageLoaded );

			tileLoadListener = new Ogre::PagingLandscapeDelegate( &( *this ),
			                   &PLSMTerrainPhysicsManager::tileLoaded );

			// sceneManager->setOption( "addLoadPageListener", pageLoadListener );
			sceneManager->setOption( "addLoadTileListener", tileLoadListener );
			
			rayQuery = sceneManager->createRayQuery( Ogre::Ray(Ogre::Vector3::ZERO, Ogre::Vector3::NEGATIVE_UNIT_Y));
			
			YAKE_ASSERT( rayQuery);
		}
		//---------------------------------------------------------------------------------------------

		void PLSMTerrainPhysicsManager::tileLoaded( Ogre::PagingLandscapeEvent* event )
		{
			YAKE_LOG("Tile loaded event handler entered...");
			//recover PLSM2's tile object
			int pageX = event->mPagex;
			int pageZ = event->mPagez;

			Ogre::PagingLandScapePage * page = sceneManager->getPageManager()->getPage( pageX, pageZ );

			PagingLandScapeTile * tile = page->getTile( event->mTilex, event->mTilez );

			createCollisionBoundingBox( tile );
		}
		//---------------------------------------------------------------------------------------------

		void PLSMTerrainPhysicsManager::pageLoaded( Ogre::PagingLandscapeEvent* event )
		{
			YAKE_LOG("Page loaded event handler entered...");
			//recover PLSM2's tile object
			int pageX = event->mPagex;
			int pageZ = event->mPagez;
			Ogre::PagingLandScapePage * page = sceneManager->getPageManager()->getPage( pageX, pageZ );

			processAllTilesInPage( page, pageX, pageZ );
		}
		//---------------------------------------------------------------------------------------------

		void PLSMTerrainPhysicsManager::processAllTilesInPage( Ogre::PagingLandScapePage * page, int pageX, int pageZ )
		{
			int tileQuantity = sceneManager->getOptions()->NumTiles;

			PagingLandScapeTile * tile = 0;

			for ( int i = 0; i < tileQuantity; ++i )
			{
				for ( int j = 0; j < tileQuantity; ++j )
				{
					tile = page->getTile( i, j );

					createCollisionBoundingBox( tile );
				}
			}
		}
		//---------------------------------------------------------------------------------------------

		void PLSMTerrainPhysicsManager::createCollisionBoundingBox( Ogre::PagingLandScapeTile* tile )
		{
 			if ( find( alreadyLoadedCollisionDetectionTiles.begin(), alreadyLoadedCollisionDetectionTiles.end(), tile ) !=
						 alreadyLoadedCollisionDetectionTiles.end() )
 				return;
 			else
 				alreadyLoadedCollisionDetectionTiles.push_back( tile);

			Ogre::AxisAlignedBox tileBbox = tile->getWorldBbox();

			Ogre::Vector3 tmpVector = tileBbox.getMaximum() - tileBbox.getMinimum();
			math::Vector3 bboxDims = VEC_OGRE2YAKE( tmpVector );
			
			tmpVector = tile->getCenter();
 			math::Vector3 bboxCenter = VEC_OGRE2YAKE( tmpVector );
  			
 			terrainCollisionDetectionActor ->createTerrainCollisionDetectionShape( bboxDims, bboxCenter, tile->getInfo() );
			
			 ///uncoment to create a tile visual BBox
			if( graphicsWorld != 0)
			{
				graphics::ISceneNode* pTerrainSN = graphicsWorld->createSceneNode();
	
				graphics::IEntity* pTerrainE = graphicsWorld->createEntity( "box_1x1x1.mesh" );
	
				pTerrainE->setMaterial("box");
	
				pTerrainSN->attachEntity( pTerrainE );
	
				pTerrainSN->setScale( bboxDims );
	
				pTerrainSN->setPosition( bboxCenter );
	
				model::Graphical* pG = new model::Graphical();
				
				pG->addSceneNode( pTerrainSN );
			}
		}
		//---------------------------------------------------------------------------------------------

		void PLSMTerrainPhysicsManager::convertIndexDataForOde( Ogre::IndexData* indexData, TriangleMeshDesc::IndexVector &
			destBuffer) 
		{
			// code snipet from OgreOde project
			Ogre::HardwareIndexBufferSharedPtr ibuf = indexData->indexBuffer;
			
			bool use32bitindexes = ( ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
			
			unsigned short* pShort = 0;
			unsigned int* pInt = 0;

			if (use32bitindexes) 
				pInt = static_cast<unsigned int*>(ibuf->lock( Ogre::HardwareBuffer::HBL_READ_ONLY));
			else 
				pShort = static_cast<unsigned short*>(ibuf->lock( Ogre::HardwareBuffer::HBL_READ_ONLY));

			size_t numTris = indexData->indexCount / 3;
			size_t index_offset = 0;
			
			destBuffer.resize( indexData->indexCount);
						
			for( size_t k = 0; k < numTris; ++k)
			{
				unsigned int vindex = use32bitindexes? *pInt++ : *pShort++;
				destBuffer[index_offset + 0] = vindex;
				
				vindex = use32bitindexes? *pInt++ : *pShort++;
				destBuffer[index_offset + 1] = vindex;
				
				vindex = use32bitindexes? *pInt++ : *pShort++;
				destBuffer[index_offset + 2] = vindex;

				index_offset += 3;
			}
			
			ibuf->unlock();
		}
		//------------------------------------------------------------------------------------

		class OdeVertexConverter
		{
			public:
				OdeVertexConverter( Ogre::Vector3 & parentSNPosition) : pSN( parentSNPosition) {};
				
				math::Vector3 operator()( const Ogre::Vector3 & val)
				{
					Ogre::Vector3 result = val - pSN;
					
					return VEC_OGRE2YAKE( result);
				}
				//------------------------------------------------------------------------------------

			protected:
				Ogre::Vector3 pSN;
		};
		//------------------------------------------------------------------------------------

		void PLSMTerrainPhysicsManager::processTile( Ogre::PagingLandScapeTileInfo * tileInfo )
		{
			Ogre::PagingLandScapePage * page = 
					sceneManager->getPageManager()->getPage( tileInfo->pageX, tileInfo->pageZ );

			YAKE_ASSERT( page ).error( "Page pointer is invalid!" );

			Ogre::PagingLandScapeTile * tile = page->getTile( tileInfo->tileX, tileInfo->tileZ ) ;

			YAKE_ASSERT( tile ).error( "Tile pointer is invalid!" );

 			if ( find( alreadyLoadedCollisionTiles.begin(), alreadyLoadedCollisionTiles.end(), tile ) !=
				alreadyLoadedCollisionTiles.end() )
 				return; // nothing to be done because tile already processed 
 			else
			{
// 				YAKE_LOG( ( "--->>>>Got new tile for creating ode trimesh<<<<---" ) );
// 				std::cerr << "Page ( " << tileInfo->pageX << ", " << tileInfo->pageZ << ")" << std::endl;
// 				std::cerr << "Tile ( " << tileInfo->tileX << ", " << tileInfo->tileZ << ")" << std::endl;

				alreadyLoadedCollisionTiles.push_back( tile );
			}
			
			Ogre::PagingLandScapeRenderable* rend = tile->getRenderable();

			YAKE_ASSERT( rend ).error( "Renderable pointer is invalid!" );

			vector< void*> params;
			int renderLevel = 0;

			params.push_back( &tileInfo->pageX );
			params.push_back( &tileInfo->pageZ );
			params.push_back( &tileInfo->tileX );
			params.push_back( &tileInfo->tileZ );
			params.push_back( &renderLevel );

			sceneManager->getOption( "PageGetTileVertexData_2", &params );
			
			//get tile parenent SN position
			Ogre::SceneNode * sceneNode = tile->getSceneNode();
			Ogre::Vector3 parentSNPosition = sceneNode->getWorldPosition();
		
			// get vertices
			int * numVtx = ( ( int* ) params[ 5 ] );
			Ogre::Vector3 * ogreVertices = ( ( Ogre::Vector3* ) params[ 6 ] );
			
			TriangleMeshDesc::VertexVector vertices( *numVtx, math::Vector3( 0, 0, 0 ) );
			
			std::transform( ogreVertices, ogreVertices + *numVtx, vertices.begin(), OdeVertexConverter( parentSNPosition) );

			// get  indexes
			Ogre::IndexData* indexData = ( ( Ogre::IndexData* ) params[ 7 ] );
			YAKE_ASSERT( indexData->indexCount).error( "There is no indexes!");
			
			TriangleMeshDesc::IndexVector indexes;
			convertIndexDataForOde( indexData, indexes);
					
			terrainCollisionDetectionActor->createTerrainCollisionShape( VEC_OGRE2YAKE( parentSNPosition), vertices, indexes);
			
			delete[] ogreVertices;
		}
		//------------------------------------------------------------------------------------
		
		real PLSMTerrainPhysicsManager::getHeightAt( const math::Vector3& position)
		{
			return getHeightAt( VEC_YAKE2OGRE( position));
		}
		//------------------------------------------------------------------------------------
		
		real PLSMTerrainPhysicsManager::getHeightAt( const Ogre::Vector3& position)
		{
			// register ray query for getHeightAt function
			ray.setOrigin( position);
			ray.setDirection( Ogre::Vector3::UNIT_Y);
	
			rayQuery->setRay( ray);
			rayQuery->setQueryMask( Ogre::RSQ_Height);
			
			Ogre::RaySceneQueryResult & result = rayQuery->execute(); 
			Ogre::RaySceneQueryResult::iterator i = result.begin(); 
 			
			Ogre::Real terrainHeight;
			
			if (i != result.end() && i->worldFragment) 
				terrainHeight = i->worldFragment->singleIntersection.y;
			
			std::cerr<<"Terrain Height  =  "<< terrainHeight;
			
			return position.y - terrainHeight;//helperQueryListener.heightAt;
		}
		//------------------------------------------------------------------------------------

		Ogre::AxisAlignedBox PLSMTerrainPhysicsManager::calcActorAABB(  IActor * _actor)
		{
			OdeActor * actor = dynamic_cast<OdeActor * >( _actor);
			YAKE_ASSERT( actor).error( "Wrong actor type - expected OdeActor...");
			
			IShapePtrList shapesList = actor->getShapes();
			
			ConstDequeIterator< IShapePtrList> itShape( shapesList );

			Ogre::AxisAlignedBox resultAABB;
			
			// iterate over all shapes and found their containing AABB
			while (itShape.hasMoreElements())
			{
				const physics::IShapePtr pShape = itShape.getNext();
			
				OdeGeom * odeGeom = dynamic_cast<OdeGeom*>( pShape);
				YAKE_ASSERT( odeGeom).error( "Wrong shape type - expected OdeGeom...");
				
				dGeomID odeGeomID = odeGeom->_getOdeGeomID();
				
				dReal odeAABB[6];
				dGeomGetAABB( odeGeomID, odeAABB); // catch ODE AABB
				
				Ogre::AxisAlignedBox  ogreAABB( odeAABB[0], odeAABB[2], odeAABB[4], //mx, my, mz 
						odeAABB[1], odeAABB[3], odeAABB[5]); //Mx, My, Mz
				
				resultAABB.merge( ogreAABB);
			}
			return resultAABB;
		}
		//------------------------------------------------------------------------------------

		void PLSMTerrainPhysicsManager::placeActorOnTerrain( const math::Vector3 & position, IActor * _actor)
		{
			//Actualy, on the ground we will place actor AABB 
			Ogre::AxisAlignedBox actorAABB = calcActorAABB( _actor) ; 
			
			Ogre::Vector3 actorPosition = actorAABB.getCenter();
			
			Ogre::Vector3 actorDims = actorAABB.getMaximum() - actorAABB.getMinimum();
			
			real terrainHeightAtActorPos = getHeightAt( actorPosition);
			
			actorPosition.y -= terrainHeightAtActorPos;
			actorPosition.y += actorDims.y / 2.0;
			
			_actor->setPosition( VEC_OGRE2YAKE( actorPosition));
		}
		//------------------------------------------------------------------------------------

	}
}
