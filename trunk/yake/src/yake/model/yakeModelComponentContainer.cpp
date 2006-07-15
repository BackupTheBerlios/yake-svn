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
	bool operator==(const SharedPtr<ModelComponent>& lhs, const ModelComponent* rhs)
	{
		return (lhs.get() == rhs);
	}
	ModelComponentContainer::ModelComponentContainer()
	{
	}
	ModelComponentContainer::~ModelComponentContainer()
	{
	}
	/*
	void ModelComponentContainer::_cloneComponents(Model& cloned) const
	{
		// clone components
		typedef std::map<const ModelComponent*,SharedPtr<ModelComponent> > ClonedMap;
		ClonedMap clonedMap;

		ConstVectorIterator<ModelComponentList> itC(components_);
		while (itC.hasMoreElements())
		{
			const ModelComponent* comp = itC.getNext().get();
			SharedPtr<ModelComponent> clonedComp( comp->clone() );
			clonedMap[ comp ] = clonedComp;
			cloned.components_.push_back( clonedComp );
		}

		// clone tags
		ConstVectorIterator<TagModelComponentMap> itCM(tag2components_);
		while (itCM.hasMoreElements())
		{
			const TagModelComponentMap::value_type& cm = itCM.getNext();
			ClonedMap::const_iterator it = clonedMap.find( cm.second.get() );
			if (it != clonedMap.end())
				cloned.tag2components_[ cm.first ] = it->second;
		}
	}
	*/
	void ModelComponentContainer::addComponent(ModelComponent* c)
	{
		YAKE_ASSERT( c );
#ifdef YAKE_DEBUG_BUILD
		YAKE_ASSERT( components_.end() == std::find(components_.begin(),components_.end(),c) );
#endif
		components_.push_back( SharedPtr<ModelComponent>(c) );
	}
	void ModelComponentContainer::addComponent(ModelComponent* c, const ComponentTag& tag)
	{
		YAKE_ASSERT( c );
		YAKE_ASSERT( !tag.empty() )(tag);

		components_.push_back( SharedPtr<ModelComponent>(c) );
#ifdef YAKE_DEBUG_BUILD
		YAKE_ASSERT( tag2components_.end() == tag2components_.find(tag) );
#endif
		tag2components_[ tag ] = c;
	}
	ModelComponent* ModelComponentContainer::getComponentByTag(const ComponentTag& tag) const
	{
		TagModelComponentMap::const_iterator it = tag2components_.find(tag);
		return ((it == tag2components_.end()) ? 0 : it->second);
	}
	ModelComponent* ModelComponentContainer::getComponentByIndex(const size_t idx) const
	{
		YAKE_ASSERT( !this->empty() );
		YAKE_ASSERT( idx < components_.size() );
		return components_[ idx ].get();
	}
	size_t ModelComponentContainer::numComponents() const
	{
		return components_.size();
	}
	bool ModelComponentContainer::empty() const
	{
		return components_.empty();
	}

} // namespace model
} // namespace yake
