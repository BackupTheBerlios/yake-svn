/**------------------------------------------------------------------------------------
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
#include <yapp/ent/yakePCH.h>
#include <yapp/ent/yakeCommon.h>
#include <yapp/ent/yakeEvent.h>
#include <yapp/ent/yakeEntity.h>

namespace yake {
namespace ent {

	DEFINE_ENTITY_BASIC( entity )

	entity::entity() : mAge(0), mEvtSpawn("onSpawn"), mEvtTick("onTick")
	{
		static EntityId lastId = 1;
		mId = lastId++;

		regEvent(&mEvtSpawn);
		regEvent(&mEvtTick);
	}
	void entity::initialise(entity_creation_context& creationCtx)
	{
		//
		onInitialise(creationCtx);
	}
	bool entity::fireEvent(const String& name)
	{
		YAKE_ASSERT( !name.empty() ).warning("need a name");
		if (name.empty())
			return false;
		Event* pEvent = getEventByName(name);
		YAKE_ASSERT( pEvent )(name).warning("entity does not have this event");
		if (!pEvent)
			return false;
		pEvent->fire( getDefaultEventParams() );
		return true;
	}

	ParamList entity::getDefaultEventParams()
	{
		ParamList params;
		params["entity"] = this;
		return params;
	}
	void entity::spawn()
	{
		onSpawn();
		mEvtSpawn.fire(getDefaultEventParams());
	}

	void entity::tick()
	{
		onTick();
		mEvtTick.fire(getDefaultEventParams());
		++mAge;
	}

	simtime entity::getAge() const
	{
		return mAge;
	}

	EntityId entity::getId() const
	{
		return mId;
	}

	void entity::setVM(scripting::IVM* pVM)
	{
		mpVM = pVM;
	}

	Event* entity::getEventByName(const String& name) const
	{
		EventMap::const_iterator itFind = mEvents.find(name);
		if (mEvents.end() == itFind)
			return 0;
		return itFind->second;
	}

	void entity::addEvent(Event* pEvent)
	{
		if (pEvent->getName().empty() || !pEvent)
			return;
		mEvents[ pEvent->getName() ] = pEvent;
		mDynEvents.push_back( SharedPtr<Event>(pEvent) );
	}

	void entity::regEvent(Event* pEvent)
	{
		if (pEvent->getName().empty() || !pEvent)
			return;
		mEvents[ pEvent->getName() ] = pEvent;
	}

} // namespace yake
} // namespace ent
