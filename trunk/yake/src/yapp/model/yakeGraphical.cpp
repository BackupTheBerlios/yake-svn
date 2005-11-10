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
#include <yapp/base/yappPCH.h>
#include <yapp/base/yapp.h>
#include <yapp/model/yakeGraphicalDataImporter.h>


namespace yake {
namespace model {

	//-----------------------------------------------------
	Graphical::~Graphical()
	{
		VectorIterator<NodeList> it(mNodes);
		while (it.hasMoreElements())
		{
			SceneNodeEntry e = it.getNext();
			if (e.bOwned)
				delete e.pSN;
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
	Graphical::SceneNodeList Graphical::getSceneNodes(bool bRecursive /*= false*/) const
	{
		SceneNodeList nodes;
		ConstVectorIterator< NodeList > it(mNodes);
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pSN = it.getNext().pSN;
			nodes.push_back( pSN );
			if (bRecursive)
				pSN->getChildren( nodes, true );
		}
		return nodes;
	}

	//-----------------------------------------------------
	graphics::ISceneNode* Graphical::getSceneNode(const String & rName, bool bRecursive /*= false*/)
	{
		VectorIterator< NodeList > it(mNodes);
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pNode = it.getNext().pSN;
			if (pNode->getName() == rName)
				return pNode;
			if (bRecursive)
			{
				pNode = pNode->getChildByName(rName,true);
				if (pNode)
					return pNode;
			}
		}
		return 0;
	}

	//-----------------------------------------------------
	void Graphical::setPosition( const Vector3& pos )
	{
		VectorIterator< NodeList > it(mNodes);
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pNode = it.getNext().pSN;
			YAKE_ASSERT( pNode );
			if (pNode)
				pNode->setPosition( pos );
		}
	}

	//-----------------------------------------------------
	void Graphical::fromDotScene(const String& fn, graphics::IWorld* pGWorld)
	{
		//TODO make this method somewhat deprecated...
		YAKE_ASSERT( pGWorld );
	
		// 1. read dotscene file into DOM

		yake::data::dom::xml::XmlSerializer ser;
		ser.parse( fn, false );
		YAKE_ASSERT( ser.getDocumentNode() )( fn ).error("Could not parse dotScene document!");

		// 2. parse DOM and create graphical objects

		yake::data::parser::dotscene::DotSceneParserV1 dsp;
		
		DotSceneListener dotSceneListener( *this );
		dotSceneListener.reset( pGWorld );
		
		dsp.subscribeToNodeSignal( Bind1( &DotSceneListener::processSceneNode, &dotSceneListener ) );
		dsp.subscribeToEntitySignal( Bind1( &DotSceneListener::processEntity, &dotSceneListener ) );
		dsp.subscribeToCameraSignal( Bind1( &DotSceneListener::processCamera, &dotSceneListener ) );
		dsp.subscribeToLightSignal( Bind1( &DotSceneListener::processLight, &dotSceneListener ) );
		
		dsp.load( ser.getDocumentNode() );
	}
}
}
