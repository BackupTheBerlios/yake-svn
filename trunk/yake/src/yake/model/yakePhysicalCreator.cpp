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
#include "yake/model/model_physical_xode_loader.h"

namespace yake {
namespace model {

	YAKE_REGISTER_CONCRETE(PhysicalFromXODECreator)

	void PhysicalFromXODECreator::create(const ComponentCreationContext& ctx, const StringMap& params)
	{
		// Verify validity of creation context
		physics::IWorld* pPWorld = ctx.pworld_;
		YAKE_ASSERT( pPWorld );
		if (!pPWorld)
			return;

		// Extract parameters

		StringMap::const_iterator itParam = params.find("file");
		YAKE_ASSERT(itParam != params.end()).debug("Missing parameter 'file'.");
		if (itParam == params.end())
			return;
		const String fn = itParam->second;

		itParam = params.find("name");
		YAKE_ASSERT(itParam != params.end()).debug("Missing parameter 'name'.");
		if (itParam == params.end())
			return;
		const String name = itParam->second;

		// Read XML file into DOM

		yake::data::dom::xml::XmlSerializer ser;
		ser.parse( fn, false );
		YAKE_ASSERT( ser.getDocumentNode() )( fn ).error("Could not parse dotScene document!");

		// Parse DOM and create graphical objects

		yake::data::parser::xode::XODEParserV1 xodeparser;

		Physical* pPhysical = new Physical(/**ctx.model_*/);

		const String namePrefix = _T("model:") + ctx.model_->getName() + _T("/")  // model
									+ (name.empty() ? _T("") : (name + _T("/"))); // component
		XODEListener xodeListener( *pPhysical, pPWorld, namePrefix );

		xodeparser.subscribeToBodySignal( Bind1( &XODEListener::processBody, &xodeListener ) );
		xodeparser.subscribeToGeomSignal( Bind1( &XODEListener::processGeom, &xodeListener ) );

		if (!xodeparser.load( ser.getDocumentNode() ))
		{
			YAKE_SAFE_DELETE( pPhysical );
		}

		ctx.model_->addComponent( pPhysical, name );
	}

} // namespace model
} // namespace yake
