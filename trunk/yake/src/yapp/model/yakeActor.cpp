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
#include <yapp/model/yakeActor.h>

namespace yake {
namespace model {

	Actor::Actor()
	{
	}
	Actor::~Actor()
	{
		for (ComponentList::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
		{
			if (it->second)
				delete (it->second);
		}
		mComponents.clear();
	}
	void Actor::act()
	{
		onAct();
	}
	void Actor::addComponent( Component* pComponent, const String & rTag )
	{
		YAKE_ASSERT( pComponent );
		String tag = rTag;
		if (tag.length() == 0)
			tag = uniqueName::create("co_");
		mComponents[tag] = pComponent;
	}
	Component* Actor::getComponent( const String & rTag ) const
	{
		ComponentList::const_iterator itFind = mComponents.find(rTag);
		if (itFind != mComponents.end())
			return itFind->second;
		return 0;
	}
	PhysicalComponent* Actor::getPhysicalComponent( const String & rTag ) const
	{
		return static_cast<PhysicalComponent*>(getComponent(rTag));
	}
	VisualComponent* Actor::getVisualComponent( const String & rTag ) const
	{
		return static_cast<VisualComponent*>(getComponent(rTag));
	}
	InputComponent* Actor::getInputComponent( const String & rTag ) const
	{
		return static_cast<InputComponent*>(getComponent(rTag));
	}
	ActorControl* Actor::getControlComponent( const String & rTag ) const
	{
		return static_cast<ActorControl*>(getComponent(rTag));;
	}

} // ns model
} // ns yake

