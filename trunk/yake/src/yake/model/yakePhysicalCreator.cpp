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

	//-----------------------------------------------------
	// class PhysicalFromXODECreator
	//-----------------------------------------------------
	struct PhysicalFromXODECreator : public ComponentCreator
	{
		YAKE_DECLARE_CONCRETE(PhysicalFromXODECreator,"physics/dotXODE");

		PhysicalFromXODECreator();
		virtual void create(const ComponentCreationContext& ctx, const StringMap& params);

	private:
		SharedPtr<data::parser::xode::XODEParser>	defaultParser_;
	};

	YAKE_REGISTER_CONCRETE(PhysicalFromXODECreator)

	PhysicalFromXODECreator::PhysicalFromXODECreator() :
		defaultParser_( new data::parser::xode::XODEParserV1() )
	{
	}
	void PhysicalFromXODECreator::create(const ComponentCreationContext& ctx, const StringMap& params)
	{
		// Verify validity of creation context
		physics::IWorld* pPWorld = ctx.getPhysicalWorld();
		YAKE_ASSERT( pPWorld );
		if (!pPWorld)
			return;

		// Extract parameters

		StringMap::const_iterator itParam = params.find("file");
		YAKE_ASSERT(itParam != params.end()).debug("Missing parameter 'file'.");
		if (itParam == params.end())
			return;
		const String fn = itParam->second;

		const String name = ctx.getName().empty() ? uniqueName::create("physical_") : ctx.getName();

		// Read XML file into DOM

		yake::data::dom::xml::XmlSerializer ser;
		ser.parse( fn, false );
		YAKE_ASSERT( ser.getDocumentNode() )( fn ).error("Could not parse dotScene document!");

		// Parse DOM and create graphical objects

		data::parser::xode::XODEParser* xodeparser = ctx.getXODEParser();
		if (!xodeparser)
			xodeparser = defaultParser_.get();
		YAKE_ASSERT( xodeparser );

		Physical* pPhysical = new Physical(/**ctx.model_*/);
		ctx.sigPreInit(ctx,*pPhysical);

		const String namePrefix = _T("model:") + ctx.getModel().getName() + _T("/")  // model
									+ (name.empty() ? _T("") : (name + _T("/"))); // component
		XODEListener xodeListener( *pPhysical, pPWorld, namePrefix );

		SignalConnection conn1 = xodeparser->subscribeToBodySignal( Bind1( &XODEListener::processBody, &xodeListener ) );
		SignalConnection conn2 = xodeparser->subscribeToGeomSignal( Bind1( &XODEListener::processGeom, &xodeListener ) );
		SignalConnection conn3 = xodeparser->subscribeToMassSignal( Bind1( &XODEListener::processMass, &xodeListener ) );
		SignalConnection conn4 = xodeparser->subscribeToJointSignal( Bind1( &XODEListener::processJoint, &xodeListener ) );
		SignalConnection conn5 = xodeparser->subscribeToMaterialSignal( Bind1( &XODEListener::processMaterial, &xodeListener ) );
		SignalConnection conn6 = xodeparser->subscribeToParseEndedSignal( Bind0( &XODEListener::postprocess, &xodeListener ) );

		if (!xodeparser->load( ser.getDocumentNode() ))
		{
			YAKE_SAFE_DELETE( pPhysical );
		}

		conn6.disconnect();
		conn5.disconnect();
		conn4.disconnect();
		conn3.disconnect();
		conn2.disconnect();
		conn1.disconnect();

		ctx.sigPostInit(ctx,*pPhysical);
		ctx.getModel().addComponent( pPhysical, name );
	}

	//-----------------------------------------------------
	// class PhysicalEmptyCreator
	//-----------------------------------------------------
	struct PhysicalEmptyCreator : public ComponentCreator
	{
		YAKE_DECLARE_CONCRETE(PhysicalEmptyCreator,"physics/empty");

		PhysicalEmptyCreator();
		virtual void create(const ComponentCreationContext& ctx, const StringMap& params);
	};

	YAKE_REGISTER_CONCRETE(PhysicalEmptyCreator)

	PhysicalEmptyCreator::PhysicalEmptyCreator()
	{
	}
	void PhysicalEmptyCreator::create(const ComponentCreationContext& ctx, const StringMap& params)
	{
		// Extract parameters

		const String name = ctx.getName().empty() ? uniqueName::create("physical_") : ctx.getName();

		ctx.getModel().addComponent( new Physical(), name );
	}
} // namespace model
} // namespace yake
