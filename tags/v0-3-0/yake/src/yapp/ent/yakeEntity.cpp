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
#include <yapp/ent/yakeEntityMachine.h>
#include <yapp/ent/yakeEntityComponent.h>
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

		_initialiseComponents(creationCtx);
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
		_tickComponents();
	}
	void Entity::addComponent( const String& tag, SharedPtr<EntityComponent>& pComponent, size_t prio /*= 0*/ )
	{
		YAKE_ASSERT( prio == 0 )(prio).warning("Only 0 priorities are currently supported!");
		YAKE_ASSERT( !tag.empty() ).warning("");
		if (tag.empty())
			return;
		if (removeComponent( tag ).get())
		{
			YAKE_LOG_WARNING("Overwrote entity component!");
		}
		mComponents[ tag ] = pComponent;
	}
	SharedPtr<EntityComponent> Entity::removeComponent( EntityComponent* pComponent )
	{
		for (ComponentList::iterator itFind = mComponents.begin(); itFind != mComponents.end(); ++itFind)
		{
			if (itFind->second.get() == pComponent)
			{
				SharedPtr<EntityComponent> pC = itFind->second;
				mComponents.erase( itFind );
				return pC;
			}
		}
		return SharedPtr<EntityComponent>();
	}
	SharedPtr<EntityComponent> Entity::removeComponent( const String& tag )
	{
		ComponentList::iterator itFind = mComponents.find( tag );
		if (itFind != mComponents.end())
		{
			SharedPtr<EntityComponent> pC = itFind->second;
			mComponents.erase( itFind );
			return pC;
		}
		return SharedPtr<EntityComponent>();
	}
	EntityComponent* Entity::getComponent( const String& tag )
	{
		ComponentList::iterator itFind = mComponents.find( tag );
		if (itFind != mComponents.end())
			return itFind->second.get();
		return 0;
	}
	void Entity::_tickComponents()
	{
		ConstVectorIterator< ComponentList > itComponent( mComponents );
		while (itComponent.hasMoreElements())
		{
			EntityComponent* pComponent = itComponent.getNext().second.get();
			YAKE_ASSERT( pComponent );
			pComponent->tick();
		}
	}
	void Entity::_initialiseComponents(object_creation_context& creationCtx)
	{
		ConstVectorIterator< ComponentList > itComponent( mComponents );
		while (itComponent.hasMoreElements())
		{
			EntityComponent* pComponent = itComponent.getNext().second.get();
			YAKE_ASSERT( pComponent );
			pComponent->initialise(creationCtx);
		}
	}
	void Entity::onGetDefaultEventParams(ParamList& params)
	{
		params["entity"] = this;
	}
	simtime Entity::getAge() const
	{
		return mAge;
	}
	void Entity::addVM(scripting::IVM* pVM)
	{
		YAKE_ASSERT( pVM ).warning("Tried to add a null VM!");
		if (!pVM)
			return;
		mVMs.push_back( pVM );
	}
	scripting::IVM* Entity::getVM(const size_t index) const
	{
		YAKE_ASSERT( index <= mVMs.size() )( index )( mVMs.size() ).warning("Invalid index! VM not found!");
		if (index <= mVMs.size())
			return mVMs.at(index);
		return 0;
	}
	size_t Entity::getVMCount() const
	{
		return mVMs.size();
	}
	scripting::IVM* Entity::getDefaultVM() const
	{
		return getVM( 0 );
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
