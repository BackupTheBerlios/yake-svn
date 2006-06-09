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

	YAKE_IMPLEMENT_REGISTRY(ComponentCreator)

	ComponentCreatorManager::ComponentCreatorManager()
	{
	}
	ComponentCreatorManager::~ComponentCreatorManager()
	{
	}
	ModelComponent* ComponentCreatorManager::create(const String& type, const ComponentCreationContext& ctx, const StringMap& params)
	{
		YAKE_ASSERT( !type.empty() )(type)(params).debug("Invalid type!");
		if (type.empty())
			return 0;

		ComponentCreator* theCreator = 0;
		{
			TypeCreatorMap::const_iterator it = creators_.find( type );
			if (it == creators_.end())
			{
				SharedPtr<ComponentCreator> creator;
				try {
					creator = templates::create<ComponentCreator>( type );
				}
				catch (...)
				{
					YAKE_LOG_ERROR("Unregistered ComponentCreator type!");
				}
				YAKE_ASSERT( creator.get() )(type)(params).debug("Failed to create component creator! Probably it has not been registered/loaded.");
				if (!creator.get())
					return 0;
				creators_.insert( std::make_pair(type,creator) );
				theCreator = creator.get();
			}
			else
				theCreator = it->second.get();
		}
		YAKE_ASSERT( theCreator );
		ModelComponent* c = theCreator->create(ctx,params);
		YAKE_ASSERT( c )(type)(params).debug("Failed to create component!");

		return c;
	}

} // namespace model
} // namespace yake
