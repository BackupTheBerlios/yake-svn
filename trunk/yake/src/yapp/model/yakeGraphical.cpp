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
		for (NodeList::iterator it = mNodes.begin(); it != mNodes.end(); ++it)
		{
			if (it->second.pSN)
			{
				YAKE_ASSERT( it->second.pSN );
				delete (it->second.pSN);
			}
		}
		mNodes.clear();
	}
	//-----------------------------------------------------
	void Graphical::addSceneNode( graphics::ISceneNode* pSceneNode, const String & rName, bool bTransferOwnership )
	{
		YAKE_ASSERT( pSceneNode );
		if (!pSceneNode)
			return;
		SceneNodeEntry e;
		e.bOwned = bTransferOwnership;
		e.pSN = pSceneNode;
		String name = (rName.length() > 0) ? rName : (uniqueName::create("graphical_sn_"));
		mNodes.insert( std::make_pair(name,e) );
	}
	//-----------------------------------------------------
	Graphical::SceneNodeList Graphical::getRootSceneNodes() const
	{
		SceneNodeList nodes;
		ConstVectorIterator< AssocVector<String,SceneNodeEntry> > it(mNodes.begin(), mNodes.end());
		while (it.hasMoreElements())
			nodes.push_back( it.getNext().second.pSN );
		return nodes;
	}
	//-----------------------------------------------------
	graphics::ISceneNode* Graphical::getSceneNodeByName(const String & rName)
	{
		AssocVector<String,SceneNodeEntry>::const_iterator itFind = mNodes.find( rName );
		if (itFind == mNodes.end())
			return 0;
		return itFind->second.pSN;
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
			String name = dss.getNameForSceneNode( pSN );
			if ( name.empty() ) 
				name = uniqueName::create("dotScene_sn_");
			this->addSceneNode( pSN, name, true );
			std::cout << "Graphical: node added - '" << name << "'" << std::endl; 
		}
	}

}
}
}
