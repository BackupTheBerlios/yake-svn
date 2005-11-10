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

#ifndef YAPP_MODEL_GRAPHICAL_DATA_IMPORTER_H
#define YAPP_MODEL_GRAPHICAL_DATA_IMPORTER_H

#include <yapp/base/yappPrerequisites.h>
#include <yake/graphics/yakeGraphicsSystem.h>
#include <yapp/model/yakeModel.h>
#include <yapp/loader/yakeDotScene.h>

namespace yake {
namespace model {

    /** Class implements DotSceneParser's listener. Creates graphical objects and places
     * them in model::Graphical object.
     */
    class YAPP_BASE_API DotSceneListener
    {
	public:
	    DotSceneListener( Graphical& owner ) : mOwner(owner)
	    {}

	    virtual ~DotSceneListener() {}		

	    typedef yake::data::parser::dotscene::DotSceneParser::NodeDesc SceneNodeDesc;
	    typedef yake::data::parser::dotscene::DotSceneParser::EntityDesc EntityDesc;
	    typedef yake::data::parser::dotscene::DotSceneParser::CameraDesc CameraDesc;
	    typedef yake::data::parser::dotscene::DotSceneParser::LightDesc LightDesc;

	    virtual void processSceneNode( const SceneNodeDesc );
	    virtual void processEntity( const EntityDesc );
	    virtual void processCamera( const CameraDesc );
	    virtual void processLight( const LightDesc );

	    void reset( graphics::IWorld* graphicsWorld )
	    {
			mSceneNodes.clear();
			mEntities.clear();
			mCameras.clear();
			mLights.clear();

			mGWorld = graphicsWorld;
	    }

	    struct EntityInfo
	    {
			graphics::IEntity*	entity;
			String		parentSceneNode;
	    };

	    struct CameraInfo
	    {
			graphics::ICamera*	camera;
			String		parentSceneNode;
	    };

	    struct LightInfo
	    {
			graphics::ILight*	light;
			String		parentSceneNode;
	    };

	    typedef AssocVector< String, graphics::ISceneNode* > SceneNodeMap;
	    typedef AssocVector< String, EntityInfo > EntityMap;
	    typedef AssocVector< String, CameraInfo > CameraMap;
	    typedef AssocVector< String, LightInfo > LightMap;

	    typedef std::map< String, String > TrackerMap;
	    TrackerMap		mTrackersTargets;

	    SceneNodeMap	mSceneNodes;
	    EntityMap		mEntities;
	    CameraMap		mCameras;
	    LightMap		mLights;

	    graphics::IWorld*	mGWorld;

	    Graphical&		mOwner;
    };


} // model
} // yake

#endif 

