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
#include "yake/model/model_graphical_dotscene_loader.h"

namespace yake {
namespace model {

	YAKE_REGISTER_CONCRETE(GraphicalFromDotSceneCreator)

	ModelComponent* GraphicalFromDotSceneCreator::create(const ComponentCreationContext& ctx, const StringMap& params)
	{
		// Verify validity of creation context
		graphics::IWorld* pGWorld = ctx.gworld_;
		YAKE_ASSERT( pGWorld );
		if (!pGWorld)
			return 0;

		// Extract parameters

		StringMap::const_iterator itParam = params.find("file");
		YAKE_ASSERT(itParam != params.end()).debug("Missing parameter 'file'.");
		if (itParam == params.end())
			return 0;
		const String fn = itParam->second;

		itParam = params.find("name");
		YAKE_ASSERT(itParam != params.end()).debug("Missing parameter 'name'.");
		if (itParam == params.end())
			return 0;
		const String name = itParam->second;

		// Read dotscene file into DOM

		yake::data::dom::xml::XmlSerializer ser;
		ser.parse( fn, false );
		YAKE_ASSERT( ser.getDocumentNode() )( fn ).error("Could not parse dotScene document!");

		// Parse DOM and create graphical objects

		yake::data::parser::dotscene::DotSceneParserV1 dsp;

		Graphical* pGraphical = new Graphical();

		DotSceneListener dotSceneListener( *pGraphical, name.empty() ? _T("") : (name + _T("/")) );
		dotSceneListener.reset( pGWorld );

		dsp.subscribeToNodeSignal( Bind1( &DotSceneListener::processSceneNode, &dotSceneListener ) );
		dsp.subscribeToEntitySignal( Bind1( &DotSceneListener::processEntity, &dotSceneListener ) );
		dsp.subscribeToCameraSignal( Bind1( &DotSceneListener::processCamera, &dotSceneListener ) );
		dsp.subscribeToLightSignal( Bind1( &DotSceneListener::processLight, &dotSceneListener ) );

		if (!dsp.load( ser.getDocumentNode() ))
		{
			YAKE_SAFE_DELETE( pGraphical );
		}

		return pGraphical;
	}

} // namespace model
} // namespace yake
