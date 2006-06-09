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

	Model::Model() : name_(_T("")), internalName_(uniqueName::create(_T("yake.model_")))
	{}
	void Model::setName(const String& name)
	{
		name_ = name;
	}
	const String& Model::getName() const
	{
		return name_; 
	}
	void Model::addComponent(ModelComponent* c, const ComponentTag& tag)
	{
		components_.addComponent(c,tag);
	}
	ModelComponent* Model::getComponentByTag(const ComponentTag& tag) const
	{
		return components_.getComponentByTag(tag);
	}
	/*
	Model* Model::clone() const
	{
		Model* cloned = new Model();
		YAKE_ASSERT( cloned );

		// clone name
		cloned->name_ = name_ + "_copy";

		// clone components
		ModelComponentContainer::_cloneComponents(*cloned);

		return cloned;
	}
	*/

} // namespace model
} // namespace yake
