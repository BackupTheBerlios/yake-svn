/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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

	typedef ::yake::object::ObjectManager<Object> ObjectManager;

	class YAKE_ENT_API Simulation
	{
	public:
		Simulation();
		~Simulation();

		// registration / setup

		void setTicksPerSecond(const real ticksPerSecond);
		simtime getTime() const;
		real getTimeAsSeconds() const;

		/** Register a class. */
		template<class T>
		void regClass(const String& clsName)
		{
			ClassId clsId =
				mObjMgr.registerClass(	clsName,
												boost::bind(&object_class::createInstance,T::getClass(),_1),
												boost::bind(&object_class::destroyInstance,T::getClass(),_1)).second;
		}
		/** VM binders can be used to bind features the entity's VMs. */
		void addEntityVMBinder( scripting::IBinder* pBinder, const bool bTransferOwnership = true );

		// start / stop / run

		bool start(	scripting::IScriptingSystem* pScripting = 0,
						graphics::IWorld* pGWorld = 0,
						audio::IWorld* pAWorld = 0,
						physics::IWorld* pPWorld = 0);
		void stop();

		void tick(const real timeElapsed);

		// object/entity management

		/**
			@Remarks "scriptFiles" and "components" are ignored for non-Entity derived objects.
		*/
		Object* createObject(const String& clsName,
									const StringPairVector& components = StringPairVector(),
									const StringVector& scriptFiles = StringVector());
		Object* createObject(const ClassId clsId,
									const StringPairVector& components = StringPairVector(),
									const StringVector& scriptFiles = StringVector());
		void destroyObject( Object* );
		void removeAndDestroyObjects();

		// messaging

		void postMessage(ObjectMessage* pMessage, Object* target);

		// events

		Event& getEvent_onEntityVMCreated();
		Event& getEvent_onEntityVMSpawned();

		// @fixme temporary accessors

		static Simulation& getSim();
		graphics::IWorld* getGraphicsWorld() const
		{ return mpGWorld; }
	private:
		scripting::IVM* createEntityVM();
	private:
		ObjectManager					mObjMgr;
		msg::MessageManager			mMsgMgr;
		typedef std::list<Object*> ObjectList;
		ObjectList						mObjs;

		scripting::IScriptingSystem*	mpScripting;

		// timing
		real								mSimTimeInSecs;
		simtime							mSimTime;
		real								mTickTime;
		real								mCurrTickTime;

		// scripting binders
		typedef SharedPtr<scripting::IBinder> BinderSharedPtr;
		typedef std::deque<BinderSharedPtr> BinderList;
		typedef std::deque<scripting::IBinder*> PureBinderPtrList;
		BinderList						mBinders;
		PureBinderPtrList				mNonOwnedBinders;

		// graphics / audio / physics ....
		graphics::IWorld*				mpGWorld;
		audio::IWorld*					mpAWorld;
		physics::IWorld*				mpPWorld;

		// events
		Event								mEvtEntityVMSpawned;
		Event								mEvtEntityVMCreated;
	};

} // namespace yake
} // namespace ent
#endif
