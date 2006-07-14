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

namespace yake {
namespace model {

	ModelManager::ModelManager()
	{
	}
	ModelManager::~ModelManager()
	{
	}
	SignalConnection ModelManager::subscribeToModelCreatedSignal(const ModelCreatedSignal::slot_type& slot)
	{
		return sigModelCreatedSignal_.connect(slot);
	}
	SignalConnection ModelManager::subscribeToPreCreateModelComponent(const PreCreateModelComponent::slot_type& slot)
	{
		return sigPreCreateModelComponent_.connect(slot);
	}
	SignalConnection ModelManager::subscribeToPostCreateModelComponent(const PostCreateModelComponent::slot_type& slot)
	{
		return sigPostCreateModelComponent_.connect(slot);
	}
	SignalConnection ModelManager::subscribeToModelInitializedSignal(const ModelInitializedSignal::slot_type& slot)
	{
		return sigModelInitializedSignal_.connect(slot);
	}
	SignalConnection ModelManager::subscribeToComponentPreInitializeSignal(const ComponentPreInitializeSignal::slot_type& slot)
	{
		return ctx_.sigPreInit_.connect( slot );
	}
	SignalConnection ModelManager::subscribeToComponentPostInitializeSignal(const ComponentPostInitializeSignal::slot_type& slot)
	{
		return ctx_.sigPostInit_.connect( slot );
	}
	namespace detail {
		void extractParams(const String& params, StringMap& out)
		{
			StringVector singles = split<String>(params,";");
			ConstVectorIterator<StringVector> it( singles );
			while (it.hasMoreElements())
			{
				const String s = it.getNext();
				StringVector keyValuePair = split<String>( s, "=" );
				out[ keyValuePair.front() ] = (keyValuePair.size() == 1) ? "" : keyValuePair.back();
			}
		}
	} // namespace detail
	void ModelManager::clear()
	{
		models_.clear();
	}
	/**
	list of components:
		component #1
			type=graphics/dotScene
			name=gfx
			file=...
			...
		component #2
			type=physics/dotXODE
			name=...
			...
	*/
	Model* ModelManager::createModel(const String& modelName, const ModelComponentDescList& compDescList)
	{
		// Validate name
		YAKE_ASSERT( !modelName.empty() );
		YAKE_ASSERT( models_.find(modelName) == models_.end() )(modelName).debug("Model with that name already exists.");
		if (models_.find(modelName) != models_.end())
			return 0;

		// Create model object
		Model* m = new Model();
		m->setName( modelName );
		models_.insert( std::make_pair(modelName,SharedPtr<Model>(m)) );
		ctx_.model_ = m;

		sigModelCreatedSignal_(*m,ctx_);

		// Create components (if any)
		ConstVectorIterator<ModelComponentDescList> itCompDesc(compDescList);
		while (itCompDesc.hasMoreElements())
		{
			const ModelComponent::Desc& desc = itCompDesc.getNextRef();

			const String& type = desc.type; // e.g. "graphics/dotScene" or "physics/XODE" etc
			const StringMap& params = desc.params;

			// Extract parameter 'name'.
			StringMap::const_iterator itParam = params.find("name");
			const String name = (itParam == params.end()) ? _T("") : itParam->second;

			// Create component.
#ifdef YAKE_DEBUG
			if (!name.empty())
			{
				YAKE_ASSERT( m->getComponentByTag(name) == 0 )(name)(type).debug("Components with duplicate tags are not allowed within the same model!");
			}
#endif

			sigPreCreateModelComponent_(*m,ctx_,type);
			creatorMgr_.create( type, ctx_, params );
			sigPostCreateModelComponent_(*m,ctx_,type);
		}
		ctx_.model_ = 0;
		sigModelInitializedSignal_(*m,ctx_);
		return m;
	}
	/// graphics/dotScene:name=gfx;file=bla.scene|physics/dotXODE...
	Model* ModelManager::createModel(const String& modelName, const String& def)
	{
		// Validate name
		YAKE_ASSERT( !modelName.empty() );
		YAKE_ASSERT( models_.find(modelName) == models_.end() )(modelName).debug("Model with that name already exists.");
		if (models_.find(modelName) != models_.end())
			return 0;

		// Split components string
		Vector<String> defComponents = split<String>(def, "|");

		// Create ModelComponentDescList.
		ModelComponentDescList compDescList;
		ConstVectorIterator<Vector<String> > itDef( defComponents );
		while (itDef.hasMoreElements())
		{
			const String& defC = itDef.getNextRef();

			// Split e.g. "gfx=graphics/dotScene:file=blah.scene" into "gfx" and "file=graphics/dotScene:blah.scene"
			Vector<String> defTypeRest = split<String>( defC, ":" );
			YAKE_ASSERT( defTypeRest.size() == 2 )(modelName)(def);

			const String type = defTypeRest.front(); // e.g. "graphics/dotScene" or "physics/XODE" etc

			// Extract parameters.
			StringMap params;
			detail::extractParams( defTypeRest.back(), params );

			// Extract parameter 'name'.
			StringMap::const_iterator itParam = params.find("name");
			const String name = (itParam == params.end()) ? _T("") : itParam->second;

			// Create component description.
			compDescList.push_back( ModelComponent::Desc(name,type,params) );
		}
		return this->createModel(modelName,compDescList);
	}
	void ModelManager::setCreationContext_GraphicalWorld(graphics::IWorld* w)
	{
		ctx_.gworld_ = w;
	}
	void ModelManager::setCreationContext_PhysicalWorld(physics::IWorld* w)
	{
		ctx_.pworld_ = w;
	}
	void ModelManager::setCreationContext_CentralController(CentralControllerBase* c)
	{
		ctx_.centralController_ = c;
	}
	void ModelManager::setCreationContext_DotSceneParser(data::parser::dotscene::DotSceneParser* parser)
	{
		ctx_.dotSceneParser_ = parser;
	}
	void ModelManager::setCreationContext_XODEParser(data::parser::xode::XODEParser* parser)
	{
		ctx_.xodeParser_ = parser;
	}

} // namespace model
} // namespace yake
