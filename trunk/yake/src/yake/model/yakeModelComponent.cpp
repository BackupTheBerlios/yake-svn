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

	ComponentCreationContext::ComponentCreationContext(ModelCreationContext& modelCtx, const String& name) : 
		modelCtx_(modelCtx),
		name_(name)
	{}
	const String& ComponentCreationContext::getName() const
	{
		return name_;
	}
	Model& ComponentCreationContext::getModel() const
	{
		YAKE_ASSERT( modelCtx_.model_ )(name_).fatal("Cannot dereference 0 model.");
		return *modelCtx_.model_;
	}
	graphics::IWorld* ComponentCreationContext::getGraphicalWorld() const
	{
		return modelCtx_.gworld_;
	}
	physics::IWorld* ComponentCreationContext::getPhysicalWorld() const
	{
		return modelCtx_.pworld_;
	}
	CentralControllerBase* ComponentCreationContext::getCentralController() const
	{
		return modelCtx_.centralController_;
	}
	data::parser::dotscene::DotSceneParser* ComponentCreationContext::getDotSceneParser() const
	{
		return modelCtx_.dotSceneParser_;
	}
	data::parser::xode::XODEParser* ComponentCreationContext::getXODEParser() const
	{
		return modelCtx_.xodeParser_;
	}
	bool ComponentCreationContext::getParamValue(const String& key, boost::any& out) const
	{
		ModelCreationContext::ParamMap::const_iterator it = modelCtx_.params_.find(key);
		if (it == modelCtx_.params_.end())
			return false;
		out = it->second;
		return true;
	}
	void ComponentCreationContext::sigPreInit(const ComponentCreationContext& compCtx,ModelComponent& comp) const
	{
		modelCtx_.sigPreInitComponent_(compCtx,comp);
	}
	void ComponentCreationContext::sigPostInit(const ComponentCreationContext& compCtx,ModelComponent& comp) const
	{
		modelCtx_.sigPostInitComponent_(compCtx,comp);
	}
} // namespace model
} // namespace yake
