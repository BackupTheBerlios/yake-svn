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
#ifndef YAKE_ENT_ENTITY_H
#define YAKE_ENT_ENTITY_H

namespace yake {
namespace ent {

	class Event;
	class entity;
	class sim;

	typedef YAKE_ENT_API yake::app::model::Graphical GraphicalModel;

	typedef YAKE_ENT_API yake::String EntityClassId;
	typedef YAKE_ENT_API uint32 EntityId;
	typedef YAKE_ENT_API boost::function<entity*(void)> EntityCreatorFn;

	struct YAKE_ENT_API entity_creation_context
	{
		graphics::IWorld*		mpGWorld;
		String					mScript;
	};

	struct YAKE_ENT_API entity_class
	{
	public:
		entity_class(const String& name) : mName(name) {}
		bool operator == (const entity_class& rhs)
		{ return mName == rhs.getName(); }
		String getName() const
		{ return mName; }
	private:
		String	mName;
	};

	#define DECLARE_ENTITY_BASIC(CLASSNAME) \
	public: \
		static entity_class* getClass(); \
		virtual entity_class* isA() const;

	/**
	@todo use yake::reflection instead of custom entity_class stuff.
	*/
	class YAKE_ENT_API entity
	{
		friend class sim;
		DECLARE_ENTITY_BASIC(entity)
	private:
		entity(const entity&);
	protected:
		entity();
		void setVM(scripting::IVM* pVM);
		void initialise(entity_creation_context& creationCtx);
		virtual void onInitialise(entity_creation_context& creationCtx) {}
	public:
		simtime getAge() const;
		EntityId getId() const;
		bool isServer() const
		{ return true; }

		void spawn();
		void tick();

		Event* getEventByName(const String& name) const;
		void addEvent(Event* pEvent);
		void regEvent(Event* pEvent);
		bool fireEvent(const String& name);

		scripting::IVM* getVM() const
		{ return mpVM; }
	protected:
		virtual void onSpawn() {}
		virtual void onTick() {}
		ParamList getDefaultEventParams();
	private:
		EntityId		mId;
		simtime			mAge;

		Event			mEvtSpawn;
		Event			mEvtTick;

		scripting::IVM*	mpVM;

		typedef std::map<String,Event*> EventMap;
		EventMap		mEvents;
		typedef std::list<SharedPtr<Event> > EventHolderList;
		EventHolderList	mDynEvents;
	};

	#define DECLARE_ENTITY(CLASSNAME) \
		DECLARE_ENTITY_BASIC(CLASSNAME) \
	private: \
		static entity* create() \
		{ return new CLASSNAME(); } \
	public: \
		static void reg( sim& theSim ); \
		static CLASSNAME* cast( entity* other ) \
		{  return dynamic_cast<CLASSNAME*>(other); } \
		static const CLASSNAME* cast( const entity* other ) \
		{  return dynamic_cast<const CLASSNAME*>(other); }

	#define DEFINE_ENTITY_BASIC(CLASSNAME) \
		entity_class* CLASSNAME::getClass() \
		{ \
			static entity_class g_class(#CLASSNAME); \
			return &g_class; \
		} \
		entity_class* CLASSNAME::isA() const \
		{ return CLASSNAME::getClass(); }

	#define DEFINE_ENTITY(CLASSNAME) \
		DEFINE_ENTITY_BASIC(CLASSNAME) \
		void CLASSNAME::reg( sim& theSim ) \
		{ theSim.regEntityCreator(#CLASSNAME, &CLASSNAME::create); }

	class YAKE_ENT_API pawn : public entity
	{
	public:
		void setGraphical( GraphicalModel* pModel );
		GraphicalModel* getGraphical() const;

		DECLARE_ENTITY(pawn)
	protected:
		pawn();
	private:
		SharedPtr<GraphicalModel>	mGraphical;
	};

	class YAKE_ENT_API light : public entity
	{
	public:
		void enableLight( bool yes );
		bool isLightEnabled() const;

		DECLARE_ENTITY(light)
	protected:
		light();
		virtual void onInitialise(entity_creation_context& creationCtx);
	private:
		SharedPtr<graphics::ISceneNode>		mpSN;
		graphics::ILight*					mpLight;
		bool								mLightEnabled;
	};

} // namespace yake
} // namespace ent
#endif