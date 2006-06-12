/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright (c) The YAKE Team
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
#include "yake/model/pch.h"
#include "yake/model/prerequisites.h"
#include "yake/model/model.h"
#include "yake/model/model_link_dotlink_loader.h"

namespace yake {
namespace model {

	DotLinkLoader::DotLinkLoader() : mpModel(0)
	{
	}

	DotLinkLoader::~DotLinkLoader()
	{
	}

	bool DotLinkLoader::load( const String & rDotLinkFilename, Model & rModel, CentralControllerBase& centralController )
	{
		using namespace data::dom;

		xml::XmlSerializer ser;
		ser.parse( rDotLinkFilename, false );

		const SharedPtr<INode> pLinksNode = ser.getDocumentNode();
		YAKE_ASSERT( pLinksNode )( rDotLinkFilename );
		if (!pLinksNode)
			return false;

		return load( *pLinksNode, rModel, centralController );
	}

	bool DotLinkLoader::load( const data::dom::INode & rLinksNode, Model & rModel, CentralControllerBase& centralController )
	{
		using namespace data::dom;
		using namespace base::templates;

		mpModel = &rModel;
		mpCentralController = &centralController;

		// parse links
		const NodeList nodes = rLinksNode.getNodes();
		ConstVectorIterator< NodeList > itNode( nodes.begin(), nodes.end() );
		while (itNode.hasMoreElements())
		{
			const SharedPtr<INode> & pNode = itNode.getNext();
			if (!pNode.get())
				continue;
			parseLink( *pNode.get() );
		}

		mpModel = 0;
		return true;
	}

	Movable* getMovableFromModel( Model& rModel, const data::dom::INode& rNode )
	{
		// parse
		String sourceType = StringUtil::toLowerCase(rNode.getAttributeValueAs<String>("type"));
		String sourceModelName = (rNode.getAttributeValueAs<String>("submodel"));
		String sourceMovableType = StringUtil::toLowerCase(rNode.getAttributeValueAs<String>("elementtype"));
		String sourceMovableName = (rNode.getAttributeValueAs<String>("element"));
		
		YAKE_LOG( "Searching movable: " ); 
		YAKE_LOG( "    type: " + sourceType );
		YAKE_LOG( "    submodel: " + sourceModelName );
		YAKE_LOG( "    movable type: " + sourceMovableType );
		YAKE_LOG( "    movable name: " + sourceMovableName );
		
		// retrieve source movable
		Movable* pMovable = 0;
		if ( sourceType == "graphical" )
		{
			YAKE_ASSERT( sourceMovableType == "graphics.scenenode" );
			YAKE_LOG( "Searching for graphical named '" + sourceModelName + "'" );

			Graphical* pG = dynamic_cast<Graphical*>(rModel.getComponentByTag( sourceModelName ));
			YAKE_ASSERT( pG )( sourceModelName );

			pMovable = pG->getSceneNode( sourceMovableName );
		}
		else if ( sourceType == "physical" )
		{
			YAKE_LOG( "Searching for physical named '" + sourceModelName + "'" );

			Physical* pP = dynamic_cast<Physical*>(rModel.getComponentByTag( sourceModelName ));
			YAKE_ASSERT( pP )( sourceModelName );

			if (sourceMovableType == "physics.actor" || sourceMovableType == "physics.complex")
				pMovable = pP->getActor( sourceMovableName );
		}

		YAKE_ASSERT( pMovable != NULL ).error( "Couldn't find movable '" + sourceMovableName + "'. Bailing out." );
		return pMovable;
	}

	void DotLinkLoader::parseLink( const data::dom::INode & rLinkNode )
	{
		YAKE_ASSERT( mpModel );
		if (!mpModel)
			return;

		YAKE_ASSERT( mpCentralController );
		if (!mpCentralController)
			return;

		using namespace data::dom;
		using namespace yake::templates;

		const String linkType = StringUtil::toLowerCase( rLinkNode.getAttributeValueAs<String>("type") );
		//const String linkName = varGet<String>( rLinkNode.getId("id") );
		const String linkName = "[not_set]";

		std::cout << "Creating link of type '"<< linkType << "'" << std::endl;

		// source
		const SharedPtr<INode> pSourceNode = rLinkNode.getNodeByName("link_source");
		YAKE_ASSERT( pSourceNode )( linkName );
		if (!pSourceNode)
			return;
	
		std::cout << "Found source for link... Searching for targets " << std::endl;

		// targets
		const NodeList targets = rLinkNode.getNodes();
		ConstVectorIterator< NodeList > itTargetNode( targets.begin(), targets.end() );
		while (itTargetNode.hasMoreElements())
		{
			const SharedPtr<INode> & pTargetNode = itTargetNode.getNext();
			YAKE_ASSERT( pTargetNode.get() )( linkName );
			if (!pTargetNode.get())
				continue;

			const String nodeName = StringUtil::toLowerCase( pTargetNode->getName() );
			if (nodeName != "link_target")
				continue;
	
			Movable* from = getMovableFromModel(*mpModel,*pSourceNode);
			Movable* to = getMovableFromModel(*mpModel,*pTargetNode);

			std::cout << "Found source and target. Adding controller to model..." << std::endl;

			ModelMovableLink* link = mpModel->createLink( from, to, linkType );
			YAKE_ASSERT( link );
			
			mpCentralController->subscribeToGraphicsUpdate(boost::bind(&ModelMovableLink::update,link,_1,_2));
		}
	}

} // namespace model
} // namespace yake
