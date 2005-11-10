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
#ifndef YAPP_MODEL_GRAPHICAL_H
#define YAPP_MODEL_GRAPHICAL_H

#include <yapp/base/yappPrerequisites.h>
#include <yake/graphics/yakeGraphicsSystem.h>
#include <yapp/model/yakeModel.h>
#include <yapp/loader/yakeDotScene.h>

namespace yake {
	using namespace base::templates;
namespace model {

	/** A graphical container model, also a submodel in a complex model.
		@Remarks Read more about the concepts in the manual on "yapp::model".
	*/
	class YAPP_BASE_API Graphical : public Submodel
	{
	public:
		Graphical()
		{}

		virtual ~Graphical();

		typedef graphics::SceneNodePtrList SceneNodeList;

		/** Adds a scene node to this model::Graphical object.
			By default ownership of the scene node is transferred to this object.
			@param pSceneNode pointer to the scene node to add
			@param bTransferOwnership self-explanatory
		*/
		void addSceneNode( graphics::ISceneNode* pSceneNode, bool bTransferOwnership = true );

		/** Returns a list of all root scene nodes added to this object. Ooptionally also returns
			all child nodes recursively.
			Consider using getAllSceneNodes() as an alternative (for performance reasons!).
		*/
		SceneNodeList getSceneNodes( bool bRecursive = false ) const;

		/** Returns a scene node given its name, either restricted to root nodes or the complete
			node hierarchy.
			@param rName name tag of the scene node
		*/
		graphics::ISceneNode* getSceneNode( const String& rName, bool bRecursive = false );

		/** Loads a complete scene from a dotScene (.scene) file and adds
			all appropriate objects to this model::Graphical.
		*/
		void fromDotScene( const String& fn, graphics::IWorld* pGWorld );

		/** Sets the position of all root nodes.
		*/
		void setPosition( const Vector3& pos );
	
	private:
		struct SceneNodeEntry
		{
			graphics::ISceneNode*	pSN;
			bool					bOwned;
		};

		typedef Vector< SceneNodeEntry > NodeList;
		NodeList		mNodes;
		
	};

} // model
} // yake

#endif
