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
#ifndef YAKE_ENT_OBJECT_H
#define YAKE_ENT_OBJECT_H

namespace yake {
namespace ent {

	class sim;

	typedef YAKE_ENT_API yake::String ObjectClassId;
	typedef YAKE_ENT_API uint32 ObjectId;
	typedef YAKE_ENT_API boost::function<Object*(void)> ObjectCreatorFn;

	/** Data is passed to a newly created object.
	*/
	struct YAKE_ENT_API object_creation_context
	{
		graphics::IWorld*		mpGWorld;
		String					mScript;
	};

	/** A meta class which represents the class of an Object.
		It can be used for comparisons, casts etc.
	*/
	struct YAKE_ENT_API object_class
	{
	public:
		object_class(const String& name) : mName(name) {}
		bool operator == (const object_class& rhs)
		{ return mName == rhs.getName(); }
		String getName() const
		{ return mName; }
	private:
		String	mName;
	};

	#define DECLARE_OBJECT_BASIC(CLASSNAME) \
	public: \
		static object_class* getClass(); \
		virtual object_class* isA() const;

	/** Base class providing minimum functionality and the necessary hooks
		for easy extension.
		It supports messaging and dynamic events.
	*/
	class YAKE_ENT_API Object
	{
		friend class sim;
		DECLARE_OBJECT_BASIC(Object)
	private:
		Object(const Object&);
	protected:
		Object();

		void initialise(object_creation_context& creationCtx);
		virtual void onInitialise(object_creation_context& creationCtx) {}

		void spawn();
		virtual void onSpawn() {}

		void tick();
		virtual void onTick() {}

		ParamList getDefaultEventParams();
		virtual void onGetDefaultEventParams(ParamList& params) {};
	public:
		ObjectId getId() const;
		bool isServer() const;

		Event* getEventByName(const String& name) const;
		void addEvent(Event* pEvent);
		void regEvent(Event* pEvent);
		bool fireEvent(const String& name);

		ObjectMessage* createMessage(const MessageId& id);
		void addMessageHandler(const MessageId& id, const MessageHandlerFn& fn, Object* source = 0);
	protected:
		// supposed to be called only by sim!
		void postMessage(ObjectMessage* pMessage)
		{
			mMessageManager.postMessage( pMessage );
		}
		msg::MessageManager& getMsgMgr()
		{ return mMessageManager; }
	private:
		msg::MessageManager	mMessageManager;
		ObjectId		mId;

		Event			mEvtSpawn;
		Event			mEvtTick;

		typedef std::map<String,Event*> EventMap;
		EventMap		mEvents;
		typedef std::list<SharedPtr<Event> > EventHolderList;
		EventHolderList	mDynEvents;
	};

	#define DECLARE_OBJECT(CLASSNAME) \
		DECLARE_OBJECT_BASIC(CLASSNAME) \
	private: \
		static Object* create() \
		{ return new CLASSNAME(); } \
	public: \
		static void reg( sim& theSim ); \
		static CLASSNAME* cast( Object* other ) \
		{  return dynamic_cast<CLASSNAME*>(other); } \
		static const CLASSNAME* cast( const Object* other ) \
		{  return dynamic_cast<const CLASSNAME*>(other); }

	#define DEFINE_OBJECT_BASIC(CLASSNAME) \
		object_class* CLASSNAME::getClass() \
		{ \
			static object_class g_class(#CLASSNAME); \
			return &g_class; \
		} \
		object_class* CLASSNAME::isA() const \
		{ return CLASSNAME::getClass(); }

	#define DEFINE_OBJECT(CLASSNAME) \
		DEFINE_OBJECT_BASIC(CLASSNAME) \
		void CLASSNAME::reg( sim& theSim ) \
		{ theSim.regEntityCreator(#CLASSNAME, &CLASSNAME::create); }

} // namespace yake
} // namespace ent
#endif