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
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/plugins/graphicsOgre/yakePCH.h>
#include <yake/graphics/yakeGeometryAccess.h>
#include <OgreMeshManager.h>
#include <OgreNoMemoryMacros.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsSystem.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsWorld.h>
#include <yake/plugins/graphicsOgre/graphicsOgreNode.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCamera.h>
#include <yake/plugins/graphicsOgre/graphicsOgreEntity.h>
#include <yake/plugins/graphicsOgre/graphicsOgreViewport.h>
#include <yake/plugins/graphicsOgre/graphicsOgreLight.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCore.h>
#include <yake/plugins/graphicsOgre/graphicsOgreParticleSystem.h>
#include <yake/plugins/graphicsOgre/graphicsOgreGeometryAccess.h>

#ifdef YAKE_USE_OSM
#include "OgreOSMScene.h"
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace graphics {
namespace ogre3d {

	OgreCore* GraphicalWorld::msCore = 0;

	//-----------------------------------------------------
	void GraphicalWorld::reg( OgreNode* pO )
	{
		YAKE_ASSERT( pO );
		if (!pO)
			return;
		//mWrapped.push_back( pO );
		mWrapped[ pO->getName() ] = pO;

		//YAKE_LOG(String("REG WRAPPED=") << pO->getName());
	}

	//-----------------------------------------------------
	void GraphicalWorld::unreg( OgreNode* pO )
	{
		YAKE_ASSERT( pO );
		if (!pO)
			return;
		//WrappedList::iterator itFind = std::find( mWrapped.begin(), mWrapped.end(), pO );
		WrappedList::iterator itFind = mWrapped.find( pO->getName() );
		YAKE_ASSERT( itFind != mWrapped.end() );
		if (itFind == mWrapped.end())
			return;
		mWrapped.erase( itFind );

		//YAKE_LOG(String("UNREG WRAPPED=") << pO->getName());
	}

	//-----------------------------------------------------
	void GraphicalWorld::setCore( OgreCore* core )
	{
		msCore = core;
		YAKE_ASSERT( msCore );
	}

	//-----------------------------------------------------
	GraphicalWorld::GraphicalWorld() : mpRaySceneQuery(0), mRootNode(0)
	{
		YAKE_ASSERT( msCore );

		StringMap params;
		params["tex_size"] = "1024";
		params["tex_count"] = "3";
		params["colour.r"] = "0.7";
		params["colour.g"] = "0.7";
		params["colour.b"] = "0.7";
		params["far_distance"] = "75";
		params["directional_light_extrusion_distance"] = "500";
		selectShadowTechnique("texture_modulative", params);

		if (!mRootNode)
		{
			mRootNode = new OgreNode( *this, msCore->getSceneMgr(), uniqueName::create("rootSn") );
			YAKE_ASSERT( mRootNode );
		}
	}

	GraphicalWorld::~GraphicalWorld()
	{
		YAKE_SAFE_DELETE( mRootNode );
		while (mWrapped.size() > 0)
		{
			YAKE_LOG_INFORMATION(String("graphicsOgre: trying to destroy wrapped object..."));
			//delete *mWrapped.begin();
			delete mWrapped.begin()->second;
			mWrapped.erase( mWrapped.begin() );
		}

		if (mpRaySceneQuery && msCore->getSceneMgr())
			msCore->getSceneMgr()->destroyQuery( mpRaySceneQuery );
		mpRaySceneQuery = 0;
	}
	
	//-----------------------------------------------------
	Ogre::SceneManager* GraphicalWorld::getSceneMgr() const
	{
		if (!msCore)
			return 0;
		return msCore->getSceneMgr();
	}

	//-----------------------------------------------------
	ISceneNode* GraphicalWorld::createSceneNode( const String& name )
	{
		YAKE_ASSERT( mRootNode );
		YAKE_ASSERT( msCore ).debug("need a core!");
		return mRootNode->createChildNode(name);
	}

	//-----------------------------------------------------
	void GraphicalWorld::setSkyBox( const String& name )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		msCore->getSceneMgr()->setSkyBox( true, name );
	}

	//-----------------------------------------------------
	IEntity* GraphicalWorld::createEntity( const String& mesh )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		OgreEntity* pEntity = new OgreEntity( msCore->getSceneMgr(), mesh );
		mEntityMap[ pEntity->getEntity_() ] = pEntity;
		return pEntity;
	}

	//-----------------------------------------------------
	ILight* GraphicalWorld::createLight()
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		return new OgreLight( msCore->getSceneMgr() );
 	}

	//-----------------------------------------------------
	IParticleSystem* GraphicalWorld::createParticleSystem( const String& rPSTemplateName )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
// for Ogre 1.1.0 "Dagon" compatibility
#if OGRE_VERSION_MINOR >= 1 
		return new OgreParticleSystem( *msCore->getSceneMgr(), rPSTemplateName );
#else
		return new OgreParticleSystem( *msCore->getParticleSysMgr(), rPSTemplateName );
#endif
	}

	//-----------------------------------------------------
	ICamera* GraphicalWorld::createCamera()
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		return new OgreCamera( msCore->getSceneMgr() );
	}

	//------------------------------------------------------
	IViewport* GraphicalWorld::createViewport( ICamera * pCamera )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		YAKE_ASSERT( pCamera ).debug("need a camera!");
		YAKE_TRY
		return new OgreViewport( msCore, static_cast<OgreCamera*>( pCamera ) );
		YAKE_CATCH_OGRE_RETHROW
	}

	//-----------------------------------------------------
	StringVector GraphicalWorld::getShadowTechniques() const
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		static StringVector s_availableTechniques;
		if (s_availableTechniques.empty())
		{
			s_availableTechniques.push_back("stencil_additive");
			s_availableTechniques.push_back("stencil_modulative");
			s_availableTechniques.push_back("texture_modulative");
		}
		return s_availableTechniques;
	}
	//-----------------------------------------------------
	bool GraphicalWorld::selectShadowTechnique(const String& name, const StringMap& params)
	{
		mCurrentShadowTechniqueParams.clear();
		mCurrentShadowTechnique = "";
		StringVector techniques = getShadowTechniques();
		if (techniques.end() == std::find(techniques.begin(), techniques.end(), name))
		{
			return false;
		}
		mCurrentShadowTechnique = name;
		mCurrentShadowTechniqueParams = params;
		if (msCore->getSceneMgr())
		{
			if (msCore->getSceneMgr()->getShadowTechnique() != Ogre::SHADOWTYPE_NONE)
				setShadowsEnabled( true );
		}
		return true;
	}

	template<class T>
		bool contains(const T& mapContainer, const typename T::key_type& key)
	{ return (mapContainer.end() != mapContainer.find(key)); }

	//-----------------------------------------------------
	void GraphicalWorld::setShadowsEnabled( bool enabled )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		Ogre::SceneManager* pSceneMgr = msCore->getSceneMgr();;
		YAKE_ASSERT( pSceneMgr ).debug("need a scene manager!");
		if (enabled)
		{
			if (mCurrentShadowTechnique.empty())
			{
				pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_NONE );
				YAKE_GRAPHICS_EXCEPT("No shadowing technique selected!");
			}

			StringMap& params = mCurrentShadowTechniqueParams;
			int texSize = 1024;
			if (contains(params,"tex_size"))
				texSize = StringUtil::parseInt(params["tex_size"]);

			int texCount = 3;
			if (contains(params,"tex_count"))
				texCount = StringUtil::parseInt(params["tex_count"]);

			math::Color colour(0.6f,0.6f,0.6f,1.0f);
			if (contains(params,"colour.r"))
				colour.r = StringUtil::parseReal(params["colour.r"]);
			if (contains(params,"colour.g"))
				colour.g = StringUtil::parseReal(params["colour.g"]);
			if (contains(params,"colour.b"))
				colour.b = StringUtil::parseReal(params["colour.b"]);

			real farDistance = 100;
			if (contains(params,"far_distance"))
				farDistance = StringUtil::parseReal(params["far_distance"]);

			real directionalLightExtrusionDistance = 100;
			if (contains(params,"directional_light_extrusion_distance"))
				directionalLightExtrusionDistance = StringUtil::parseReal(params["directional_light_extrusion_distance"]);

			if (mCurrentShadowTechnique == "texture_modulative")
			{
				pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_TEXTURE_MODULATIVE );
				pSceneMgr->setShadowTextureSize( texSize );
				pSceneMgr->setShadowTextureCount( texCount );
			}
			else if (mCurrentShadowTechnique == "stencil_additive")
			{
				pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_ADDITIVE );
			}
			else if (mCurrentShadowTechnique == "stencil_modulative")
			{
				pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_MODULATIVE );
			}
			else
				pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_NONE );

			pSceneMgr->setShadowDirectionalLightExtrusionDistance( directionalLightExtrusionDistance );
			pSceneMgr->setShadowFarDistance( farDistance );
			pSceneMgr->setShadowColour( Ogre::ColourValue(colour.r,colour.g,colour.b,colour.a) );
			pSceneMgr->setShowDebugShadows( false );
		}
		else
			pSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_NONE );
	}

	//-----------------------------------------------------
	void GraphicalWorld::render( real timeElapsed )
	{
		YAKE_ASSERT( msCore ).debug("need a core!");
		firePreRender( timeElapsed );
		msCore->update( timeElapsed );
		firePostRender( timeElapsed );
	}

	//-----------------------------------------------------
	IMeshGeometryAccess* GraphicalWorld::createProceduralMesh(const String & name)
	{
		IMeshGeometryAccess* pMGA = getProceduralMesh( name );
		if (pMGA)
			return 0;

		Ogre::MeshPtr pMesh = Ogre::Root::getSingleton().getMeshManager()->createManual(
										name.c_str(), "yakeGroup" /*@todo user another group*/ );
		YAKE_ASSERT( !pMesh.isNull() );
		//@todo deprecated? pMesh->setManuallyDefined( true );
		pMesh->setIndexBufferPolicy( Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );
		pMesh->setVertexBufferPolicy( Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, false );

		pMGA = new OgreMeshGeometryAccess( pMesh );
		YAKE_ASSERT( pMGA );
		return pMGA;
	}

	//-----------------------------------------------------
	void GraphicalWorld::destroyProceduralMesh(const String & name)
	{
	}

	//-----------------------------------------------------
	IMeshGeometryAccess* GraphicalWorld::getProceduralMesh(const String & name)
	{
		ProcMeshMap::iterator itFind = mProcMeshes.find( name );
		if (itFind == mProcMeshes.end())
			return 0;
		return new OgreMeshGeometryAccess(itFind->second);
	}

	//-----------------------------------------------------
	real GraphicalWorld::getRenderWindowWidth() const
	{
		YAKE_ASSERT( msCore );
		Ogre::RenderWindow* pWin = msCore->getRenderWindow();
		YAKE_ASSERT( pWin );
		return pWin->getWidth();
	}

	//-----------------------------------------------------
	real GraphicalWorld::getRenderWindowHeight() const
	{
		YAKE_ASSERT( msCore );
		Ogre::RenderWindow* pWin = msCore->getRenderWindow();
		YAKE_ASSERT( pWin );
		return pWin->getHeight();
	}

	//-----------------------------------------------------
	IEntity* GraphicalWorld::pickEntity(const math::Ray& ray)
	{
		YAKE_ASSERT( msCore );

		const unsigned long mask = 0xFFFFFFFF;
		Ogre::Ray ogreRay( VEC_YAKE2OGRE(ray.getOrigin()), VEC_YAKE2OGRE(ray.getDirection()) );
		if (!mpRaySceneQuery)
		{
			mpRaySceneQuery = msCore->getSceneMgr()->createRayQuery( ogreRay, mask );
			mpRaySceneQuery->setSortByDistance( true, 1 ); // sort and return max 1 object
		}
		else
			mpRaySceneQuery->setRay( ogreRay );

		Ogre::RaySceneQueryResult &result = mpRaySceneQuery->execute();
		Ogre::RaySceneQueryResult::iterator it = result.begin();
		if (it != result.end())
		{
			//it->distance
			//if (it->worldFragment)
			if (it->movable)
			{
				//Ogre::UserDefinedObject* udo = Ogre::MovableObject::getUserObject();
				if (it->movable->getMovableType() == "Entity")
				{
					EntityMap::const_iterator itFind = mEntityMap.find( static_cast<Ogre::Entity*>( it->movable ) );
					if (itFind != mEntityMap.end())
						return itFind->second;
				}
			}
		}
		return 0;
	}
	//-----------------------------------------------------
#ifdef YAKE_USE_OSM
	// Callback handler to post-process created objects loaded by the OSM loader.
	// We create our graphicsOGRE wrapper objects here.
	class oSceneCallback : public OSMSceneCallbacks {
	public:
		GraphicalWorld&		world_;
		Ogre::SceneManager*	sceneMgr_;
		OgreNode*				currNode_;
		typedef std::map<Ogre::Node*,OgreNode*> NodeMap;
		NodeMap					nodeMap_;
		oSceneCallback(GraphicalWorld& world, Ogre::SceneManager* sceneMgr, OgreNode* root) : 
			world_(world), sceneMgr_(sceneMgr), currNode_(0)
		{
			nodeMap_[ root->getSceneNode_() ] = root;
		}
		// Called when a node has been created
		virtual void OnNodeCreate(Ogre::SceneNode *pNode, TiXmlElement* pNodeDesc)
		{
			std::cout << "osm node '" << pNode->getName().c_str() << "'\n";
			currNode_= new OgreNode( pNode, world_, sceneMgr_ );
			YAKE_ASSERT( 0 == pNode->numAttachedObjects() );

			nodeMap_[ pNode ] = currNode_;

			nodeMap_[ pNode->getParent() ]->addChildNode(currNode_);
		}

		// Called when an entity has been created
		virtual void OnEntityCreate(Ogre::Entity *pEntity, TiXmlElement* pEntityDesc)
		{
			std::cout << "osm entity '" << pEntity->getName().c_str() << "'\n";
			YAKE_ASSERT( currNode_ );
			if (!currNode_)
				return;
			OgreEntity* ent = new OgreEntity( pEntity, sceneMgr_ );
			currNode_->attachEntity( ent ); //<= this is safe even though pEntity is already
												// attached to the underlying Ogre::SceneNode of
												// currNode_.
		}

		// Called when a camera has been created
		virtual void OnCameraCreate(Ogre::Camera *pCamera, TiXmlElement* pCameraDesc)
		{
			std::cout << "osm camera '" << pCamera->getName().c_str() << "'\n";
			YAKE_ASSERT( pCamera );
			if (!pCamera)
				return;
			OgreCamera* cam = new OgreCamera( pCamera, sceneMgr_ );
			if (currNode_)
			{
				currNode_->attachCamera( cam ); // Is safe even if cam is already attached.
			}
			else
			{
			}
		}

		// Called when a light has been created
		virtual void OnLightCreate(Ogre::Light *pLight, TiXmlElement* pLightDesc)
		{
			std::cout << "osm light '" << pLight->getName().c_str() << "'\n";
			YAKE_ASSERT( pLight );
			if (!pLight)
				return;
			OgreLight* light = new OgreLight( pLight, sceneMgr_ );
			if (currNode_)
			{
				currNode_->attachLight( light ); // Is safe even if light is already attached.
			}
			else
			{
			}
		}
	};
#endif
	//-----------------------------------------------------
	bool GraphicalWorld::load(const String& type, const String& file)
	{
		if (type.empty())
			return false;
		if (file.empty())
			return false;
#ifdef YAKE_USE_OSM
		if (type == "osm")
		{
			YAKE_ASSERT( msCore );
			if (!msCore)
				return false;
			// create oscene parser
			OgreNode* root = mRootNode;
			OSMScene oScene(msCore->getSceneMgr(), msCore->getRenderWindow());
			oSceneCallback oe_Callback(*this, msCore->getSceneMgr(), root); // creates wrappers
			if (!oScene.initialise(file.c_str(), &oe_Callback))
				return false;
			YAKE_ASSERT( mRootNode );
			if (!oScene.createScene(root->getSceneNode_()))
				return false;
			return true;
		}
#endif
		return false;
	}

} // ogre3d
} // graphics
} // yake
