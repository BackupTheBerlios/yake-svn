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
#include <yapp/ent/yakePCH.h>
#include <yapp/ent/yakeCommon.h>
#include <yapp/ent/yakeEvent.h>
#include <yapp/ent/yakeMessaging.h>
#include <yapp/ent/yakeObject.h>
#include <yapp/ent/yakeEntity.h>
#include <yapp/ent/yakeSim.h>

namespace yake {
namespace ent {

	DEFINE_OBJECT( Entity )

	Entity::Entity() : mAge(0)
	{
	}
	void Entity::onInitialise(object_creation_context& creationCtx)
	{
		//getMsgMgr().addMessageHandler( CMDID_MachineEnterState, boost::bind(&Entity::onCmdMachineEnterState,this,_1) );
		//getMsgMgr().addMessageHandler( CMDID_MachineExitState, boost::bind(&Entity::onCmdMachineExitState,this,_1) );
		getMsgMgr().addMessageHandler( CMDID_MachineChangeTo, boost::bind(&Entity::onCmdMachineChangeTo,this,_1) );
		mDefaultMachine.reset( new EntityMachine(*this) );
	}
	MsgResultCode Entity::onCmdMachineChangeTo(const Message& msg)
	{
		const ObjectMessage& omsg = static_cast<const ObjectMessage&>(msg);
		try {
			const String machine = boost::any_cast<String>(omsg.get("machine"));
			const String targetState = boost::any_cast<String>(omsg.get("to"));

			EntityMachine* pMachine = (machine == "default") ? getDefaultMachine() : getMachine(machine);
			YAKE_ASSERT( pMachine )(machine)(targetState).warning("machine not found");
			if (pMachine)
			{
				ErrorCode err = pMachine->changeTo( targetState );
				YAKE_ASSERT( err == eOk )(err).warning("failed to change state");
				if (err != eOk)
				{
					std::cout << "ERROR: Failed to change state. Error: " << err << "\n";
				}
			}
		} catch(...)
		{
		}
		return msg::kHandled;
	}
	void Entity::onTick()
	{
		++mAge;
	}
	void Entity::onGetDefaultEventParams(ParamList& params)
	{
		params["entity"] = this;
	}
	simtime Entity::getAge() const
	{
		return mAge;
	}
	void Entity::setVM(scripting::IVM* pVM)
	{
		mpVM = pVM;
	}
	EntityMachine* Entity::ceateMachine(const String& id)
	{
		YAKE_ASSERT( !id.empty() );
		if (id.empty())
			return 0;
		const bool bMachineExists = (mMachines.find(id) == mMachines.end());
		YAKE_ASSERT( !bMachineExists );
		if (bMachineExists)
			return 0;
		EntityMachine* pMachine = new EntityMachine(*this);
		YAKE_ASSERT( pMachine );
		mMachines[ id ] = SharedPtr<EntityMachine>(pMachine);

		return pMachine;
	}
	EntityMachine* Entity::getDefaultMachine() const
	{
		return mDefaultMachine.get();
	}
	EntityMachine* Entity::getMachine(const String& id)
	{
		YAKE_ASSERT( !id.empty() );
		if (id.empty())
			return 0;
		MachineMap::const_iterator itFind = mMachines.find(id);
		const bool bMachineExists = (itFind != mMachines.end());
		YAKE_ASSERT( bMachineExists );
		if (!bMachineExists)
			return 0;
		return itFind->second.get();
	}

} // namespace yake
} // namespace ent
