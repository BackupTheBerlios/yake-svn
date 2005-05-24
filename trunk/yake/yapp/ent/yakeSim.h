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
#ifndef YAKE_ENT_SIM_H
#define YAKE_ENT_SIM_H

namespace yake {
namespace ent {

	class YAKE_ENT_API sim
	{
	public:
		sim(scripting::IScriptingSystem& scriptingSystem,
			const real ticksPerSecond = 10.f,
			graphics::IWorld* pGWorld = 0);
		~sim();
		simtime getTime() const;
		real getTimeAsSeconds() const;
		void tick(const real timeElapsed);
		void regEntityCreator( const EntityClassId& id, const EntityCreatorFn& creatorFn );
		void addEntityVMBinder( scripting::IBinder* pBinder, const bool bTransferOwnership = true );
		entity* createEntity( const EntityClassId& id, const String& scriptFile );

		graphics::IWorld* getGraphicsWorld() const
		{ return mpGWorld; }

		// temp
		static sim& getSim();
		
		event::MessageManager& getMessageMgr();

		Event& getEvent_onEntitySpawned();
		Event& getEvent_onEntityVMCreated();
	private:
		scripting::IVM* createEntityVM();
	private:
		typedef std::map<EntityClassId,EntityCreatorFn> EntityCreatorMap;
		typedef std::list<SharedPtr<entity> > EntityList;

		EntityCreatorMap	mEntityCreators;
		EntityList			mEntities;

		real				mSimTimeInSecs;
		simtime				mSimTime;
		real				mTickTime;
		real				mCurrTickTime;

		Event				mEvtEntitySpawned;
		Event				mEvtEntityVMCreated;
		scripting::IScriptingSystem&	mScripting;

		typedef SharedPtr<scripting::IBinder> BinderPtr;
		typedef std::deque<BinderPtr> BinderList;
		typedef std::deque<scripting::IBinder*> PureBinderPtrList;
		BinderList			mBinders;
		PureBinderPtrList	mNonOwnedBinders;

		graphics::IWorld*	mpGWorld;

		event::MessageManager		mMsgMgr;
	};

} // namespace yake
} // namespace ent
#endif