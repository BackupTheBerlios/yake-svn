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
#include <yapp/base/yappPCH.h>
#include <yapp/base/yapp.h>
#include <yake/samples/data/demo/yakeDotScene.h>

namespace yake {
namespace app {
namespace model {

	//-----------------------------------------------------
	Graphical::~Graphical()
	{
		for (NodeMap::iterator it = mNodes.begin(); it != mNodes.end(); ++it)
		{
			if (it->second)
			{
				YAKE_ASSERT( it->first );
				delete (it->first);
			}
		}
		mNodes.clear();
	}
	//-----------------------------------------------------
	void Graphical::addSceneNode( graphics::ISceneNode* pSceneNode, bool bTransferOwnership )
	{
		YAKE_ASSERT( pSceneNode );
		if (!pSceneNode)
			return;
		mNodes.insert( std::make_pair(pSceneNode,bTransferOwnership) );
	}
	//-----------------------------------------------------
	Graphical::SceneNodeList Graphical::getRootSceneNodes() const
	{
		SceneNodeList nodes;
		ConstVectorIterator< AssocVector<graphics::ISceneNode*, bool> > it(mNodes.begin(), mNodes.end());
		while (it.hasMoreElements())
			nodes.push_back( it.getNext().first );
		return nodes;
	}
	//-----------------------------------------------------
	void Graphical::fromDotScene(const String & fn, graphics::IGraphicalWorld* pGWorld)
	{
		YAKE_ASSERT( pGWorld );
	
		// 1. read dotscene file into DOM

		SharedPtr<yake::data::dom::ISerializer> ser( new yake::data::dom::xml::XmlSerializer() );
		ser->parse(fn, false);
		YAKE_ASSERT( ser->getDocumentNode() );

		// 2. parse DOM and create graphical objects

		yake::data::serializer::dotscene::DotSceneSerializerV1 dss;
		dss.load( ser->getDocumentNode(), pGWorld );

		// 3. create graphical model & add it to complex model

		// 4. fill graphical
		yake::data::serializer::dotscene::SceneNodeList nodes =
			dss.getRootLevelSceneNodes();
		ConstVectorIterator<yake::data::serializer::dotscene::SceneNodeList>
			it( nodes.begin(), nodes.end() );
		while (it.hasMoreElements())
		{
			this->addSceneNode( it.getNext(), true );
		}
	}

}
}
}