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
#include <yapp/model/yakeModelLink.h>
#include <yapp/loader/yakeModelLinkCreator.h>
#include <yapp/loader/yakeDotLinkLoader.h>

namespace yake {
namespace app {
namespace model {

	DotLinkLoader::DotLinkLoader() : mpModel(0)
	{
	}

	DotLinkLoader::~DotLinkLoader()
	{
	}

	bool DotLinkLoader::load( const String & rDotLinkFilename, complex::Model & rModel )
	{
		using namespace data::dom;

		xml::XmlSerializer ser;
		ser.parse( rDotLinkFilename, false );

		const SharedPtr<INode> pLinksNode = ser.getDocumentNode();
		YAKE_ASSERT( pLinksNode )( rDotLinkFilename );
		if (!pLinksNode)
			return false;

		return load( *pLinksNode, rModel );
	}

	bool DotLinkLoader::load( const data::dom::INode & rLinksNode, complex::Model & rModel )
	{
		using namespace data::dom;
		using namespace base::templates;

		mpModel = &rModel;

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

	void DotLinkLoader::parseLink( const data::dom::INode & rLinkNode )
	{
		YAKE_ASSERT( mpModel );
		if (!mpModel)
			return;

		using namespace data::dom;
		using namespace base::templates;

		const String linkType = StringUtil::toLowerCase( rLinkNode.getAttributeValueAs<String>("type") );
		//const String linkName = varGet<String>( rLinkNode.getId("id") );
		const String linkName = "[not_set]";

		std::cout << "Creating link of type '"<< linkType << "'" << std::endl;
		
		SharedPtr<ModelLinkCreator> pCreator = create< ModelLinkCreator >( linkType );
		YAKE_ASSERT( pCreator )( linkName )( linkType );
		if (!pCreator)
			return;
		
		std::cout << "Creator acquired for link type '"<< linkType << "'" << std::endl;

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

			const String nodeName = StringUtil::toLowerCase( pTargetNode->getValueAs<String>( "name" ) );
			if (nodeName != "link_target")
				continue;
	
			std::cout << "Found source and target. Adding controller to model..." << std::endl;

			SharedPtr<IObjectController>  pController( pCreator->createLink( *mpModel, *pSourceNode, *pTargetNode ) );
			YAKE_ASSERT( pController ).error( "Failed to create controller! " );
			
			mpModel->addController( pController );
		}
	}
}
}
}
