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
#include <yapp/loader/yakeDotScene.h>

namespace yake {
namespace app {
namespace model {

	//-----------------------------------------------------
	Graphical::~Graphical()
	{
		VectorIterator<NodeList> it(mNodes);
		while (it.hasMoreElements())
		{
			delete it.getNext().pSN;
		}
		mNodes.clear();
	}
	//-----------------------------------------------------
	void Graphical::addSceneNode( graphics::ISceneNode* pSceneNode, bool bTransferOwnership )
	{
		YAKE_ASSERT( pSceneNode );
		if (!pSceneNode)
			return;
		SceneNodeEntry e;
		e.bOwned = bTransferOwnership;
		e.pSN = pSceneNode;
		mNodes.push_back( e );
	}
	//-----------------------------------------------------
	Graphical::SceneNodeList Graphical::getSceneNodes() const
	{
		SceneNodeList nodes;
		ConstVectorIterator< NodeList > it(mNodes);
		while (it.hasMoreElements())
			nodes.push_back( it.getNext().pSN );
		return nodes;
	}
	//-----------------------------------------------------
	graphics::ISceneNode* Graphical::getSceneNodeByName(const String & rName)
	{
		VectorIterator< NodeList > it(mNodes);
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pNode = it.getNext().pSN;
			if (pNode->getName() == rName)
				return pNode;
		}
		return 0;
	}
	//-----------------------------------------------------
	graphics::ISceneNode* Graphical::getAnySceneNodeByName(const String & rName)
	{
		VectorIterator< NodeList > it(mNodes);
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pNode = it.getNext().pSN;
			if (pNode->getName() == rName)
				return pNode;
			pNode = pNode->getChildByName(rName);
			if (pNode)
				return pNode;
		}
		return 0;
	}
	//-----------------------------------------------------
	void Graphical::fromDotScene(const String & fn, graphics::IWorld* pGWorld)
	{
		YAKE_ASSERT( pGWorld );
	
		// 1. read dotscene file into DOM

		yake::data::dom::xml::XmlSerializer ser;
		ser.parse(fn, false);
		YAKE_ASSERT( ser.getDocumentNode() )( fn ).error("Could not parse dotScene document!");

		// 2. parse DOM and create graphical objects

		yake::data::serializer::dotscene::DotSceneSerializerV1 dss;
		dss.load( ser.getDocumentNode(), pGWorld );

		// 3. create graphical model & add it to complex model

		// 4. fill graphical
		yake::data::serializer::dotscene::SceneNodeList nodes =
			dss.getRootLevelSceneNodes();
		ConstVectorIterator<yake::data::serializer::dotscene::SceneNodeList>
			it( nodes.begin(), nodes.end() );
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pSN = it.getNext();
			String name = pSN->getName();
			if ( name.empty() ) 
				name = uniqueName::create("dotScene_sn_");
			this->addSceneNode( pSN, true );
			std::cout << "Graphical: node added - '" << name << "'" << std::endl; 
		}
	}

}
}
}
