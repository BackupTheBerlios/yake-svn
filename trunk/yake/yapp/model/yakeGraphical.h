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

namespace yake {
	using namespace base::templates;
namespace app {
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
		typedef Vector<graphics::ISceneNode*> SceneNodeList;

		/** Adds a scene node to this model::Graphical object and tags it with a name.
			By default ownership of the scene node is transferred to this object.
			@param pSceneNode pointer to the scene node to add
			@param rName name tag for later retrieval of the added object
			@param bTransferOwnership self-explanatory
		*/
		void addSceneNode( graphics::ISceneNode* pSceneNode, const String & rName, bool bTransferOwnership = true );

		/** Returns a list of all registerd scene nodes.
		*/
		SceneNodeList getRootSceneNodes() const;

		/** Returns a scene node given the name tag it has been registered with earlier.
			@param rName name tag of the scene node
		*/
		graphics::ISceneNode* getSceneNodeByName( const String & rName );

		/** Loads a complete scene from a dotScene (.scene) file and adds
			all appropriate objects to this model::Graphical.
		*/
		void fromDotScene(const String & fn, graphics::IWorld* pGWorld);
	private:
		struct SceneNodeEntry
		{
			graphics::ISceneNode*	pSN;
			bool					bOwned;
		};
		typedef AssocVector< String, SceneNodeEntry > NodeList;
		NodeList		mNodes;
	};

} // model
} // app
} // yake

#endif
