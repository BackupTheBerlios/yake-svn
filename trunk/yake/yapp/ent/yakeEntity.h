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
#ifndef YAKE_ENT_ENTITY_H
#define YAKE_ENT_ENTITY_H

namespace yake {
namespace ent {

	class Entity;
	class EntityMachine;

	typedef YAKE_ENT_API yake::app::model::Graphical GraphicalModel;

	/** An entity state used by EntityMachines.
	*/
	class EntityMachineState : public yake::app::state::State
	{
	public:
		EntityMachineState()
		{}
		virtual ~EntityMachineState()
		{
			ConstDequeIterator<CbList> itCb(mCallbacks);
			while (itCb.hasMoreElements())
				delete itCb.getNext();
		}
		struct ICallback
		{
			virtual ~ICallback() {}
			virtual void onEnter() {}
			virtual void onExit() {}
			virtual void onStep() {}
		};
		void addCallback(ICallback* pCb)
		{
			if (pCb)
				mCallbacks.push_back(pCb);
		}
		ICallback* removeCallback(ICallback* pCb)
		{
			if (pCb)
			{
				CbList::iterator it = std::find( mCallbacks.begin(), mCallbacks.end(), pCb);
				if (it != mCallbacks.end())
					mCallbacks.erase(it);
			}
			return pCb;
		}
	protected:
		typedef std::deque<ICallback*> CbList;
		CbList		mCallbacks;
		virtual void onEnter()
		{
			ConstDequeIterator<CbList> itCb(mCallbacks);
			while (itCb.hasMoreElements())
				itCb.getNext()->onEnter();
		}
		virtual void onExit()
		{
			ConstDequeIterator<CbList> itCb(mCallbacks);
			while (itCb.hasMoreElements())
				itCb.getNext()->onExit();
		}
		virtual void onStep()
		{
			ConstDequeIterator<CbList> itCb(mCallbacks);
			while (itCb.hasMoreElements())
				itCb.getNext()->onStep();
		}
	};

	/** A state machine for entities.
		@todo remove from public header?
	*/
	class YAKE_ENT_API EntityMachine : public yake::app::state::Machine<String>
	{
	public:
		typedef yake::app::state::Machine<String> BaseMachine;

		EntityMachine(Entity& owner);
		String getCurrentStateId() const;
		Entity* getOwner() const
		{ return mOwner; }
	private:
		Entity*		mOwner;
	};


	/**
	@todo use yake::reflection instead of custom object_class stuff.
	*/
	class YAKE_ENT_API Entity : public Object
	{
		friend class sim;
		DECLARE_OBJECT(Entity)
	private:
		Entity(const Entity&);
	protected:
		Entity();
		void setVM(scripting::IVM* pVM);
		virtual void onInitialise(object_creation_context& creationCtx);
		virtual void onTick();
		virtual void onGetDefaultEventParams(ParamList& params);
	public:
		simtime getAge() const;

		scripting::IVM* getVM() const
		{ return mpVM; }

		EntityMachine* ceateMachine(const String& id);
		EntityMachine* getDefaultMachine() const;
		EntityMachine* getMachine(const String& id);

		//YAKE_MEMBERSIGNAL( public, void(Entity*,EntityMachine*,EntityMachineState*), StateAdded)
		//YAKE_MEMBERSIGNAL( public, void(Entity*,EntityMachine*,const String&), ExitState)
		//YAKE_MEMBERSIGNAL( public, void(Entity*,EntityMachine*,const String&), EnterState)
	private:
		//MsgResultCode onCmdMachineEnterState(const Message& msg);
		MsgResultCode onCmdMachineChangeTo(const Message& msg);
	private:
		simtime			mAge;

		scripting::IVM*	mpVM;

		typedef std::map<String,SharedPtr<EntityMachine> > MachineMap;
		MachineMap		mMachines;
		SharedPtr<EntityMachine>	mDefaultMachine;
	};

	class YAKE_ENT_API pawn : public Entity
	{
	public:
		void setGraphical( GraphicalModel* pModel );
		GraphicalModel* getGraphical() const;

		DECLARE_OBJECT(pawn)
	protected:
		pawn();
	private:
		SharedPtr<GraphicalModel>	mGraphical;
	};

	class YAKE_ENT_API light : public Entity
	{
	public:
		void enableLight( bool yes );
		bool isLightEnabled() const;

		DECLARE_OBJECT(light)
	protected:
		light();
		virtual void onInitialise(object_creation_context& creationCtx);
	private:
		SharedPtr<graphics::ISceneNode>		mpSN;
		graphics::ILight*					mpLight;
		bool								mLightEnabled;
	};

} // namespace yake
} // namespace ent
#endif