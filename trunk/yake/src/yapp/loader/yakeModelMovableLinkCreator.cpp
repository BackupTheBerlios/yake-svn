/*
   ------------------------------------------------------------------------------------
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
#include <yapp/loader/yakeModelMovableLinkCreator.h>
#include <yapp/model/yakeModelMovableLink.h>

namespace yake {
namespace app {
namespace model {

	YAKE_REGISTER_CONCRETE( ModelMovableLinkCreator );

	Movable* getMovableFromModel(	complex::Model & rModel, const data::dom::INode& rNode )
	{
		// parse
		String sourceType = StringUtil::toLowerCase(rNode.getAttributeValueAs<String>("type"));
		String sourceModelName = (rNode.getAttributeValueAs<String>("submodel"));
		String sourceMovableType = StringUtil::toLowerCase(rNode.getAttributeValueAs<String>("elementtype"));
		String sourceMovableName = (rNode.getAttributeValueAs<String>("element"));
		
		std::cout << "Searching movable: " << std::endl; 
		std::cout << "    type: " << sourceType << std::endl; 
		std::cout << "    submodel: " << sourceModelName << std::endl; 
		std::cout << "    movable type: " << sourceMovableType << std::endl; 
		std::cout << "    movable name: " << sourceMovableName << std::endl; 
		
		// retrieve source movable
		Movable* pMovable = 0;
		if (sourceType == "graphical")
		{
			YAKE_ASSERT( sourceMovableType == "graphics.scenenode" );
			std::cout << "Searching for graphical named '" << sourceModelName << "'" << std::endl;
			Graphical* pG = rModel.getGraphicalByName( sourceModelName );
			YAKE_ASSERT( pG );
			pMovable = pG->getSceneNode( sourceMovableName, true );
		}
		else if (sourceType == "physical")
		{
			std::cout << "Searching for physical named '" << sourceModelName << "'" << std::endl;
			Physical* pP = rModel.getPhysicalByName( sourceModelName );
			YAKE_ASSERT( pP );
			if (sourceMovableType == "physics.actor" || sourceMovableType == "physics.complex")
				pMovable = pP->getActorByName( sourceMovableName );
			//else if (sourceMovableType == "physics.complex")
			//	pMovable = pP->getActorByName( sourceMovableName ).get();
		}
		YAKE_ASSERT( pMovable );
		return pMovable;
	}

	ModelLink* ModelMovableLinkCreator::createLink(	complex::Model & rModel, 
							const data::dom::INode& rSourceNode,
							const data::dom::INode& rTargetNode )
	{
		using namespace ::yake::data::dom;

		// retrieve source movable
		Movable* pSourceMovable = getMovableFromModel( rModel, rSourceNode );
		YAKE_ASSERT( pSourceMovable );
		if (!pSourceMovable)
			return 0;

		// retrieve target movable
		Movable* pTargetMovable = getMovableFromModel( rModel, rTargetNode );
		YAKE_ASSERT( pTargetMovable );
		if (!pTargetMovable)
			return 0;

		// link them up
		ModelMovableLink* pLink = new ModelMovableLink();
		YAKE_ASSERT( pLink );
		pLink->setSource( pSourceMovable );
		pLink->subscribeToPositionChanged( pTargetMovable );
		pLink->subscribeToOrientationChanged( pTargetMovable );
		return pLink;
	}
}
}
}