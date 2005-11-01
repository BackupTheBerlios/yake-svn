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
#ifndef YAKE_ENT_OBJECT_H
#define YAKE_ENT_OBJECT_H

namespace yake {
	namespace audio {
		class IWorld;
	}
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
		physics::IWorld*		mpPWorld;
		audio::IWorld*			mpAWorld;
		String					mScript;
	};

	class Property;
	struct YAKE_ENT_API PropertyDef
	{
		PropertyDef() {}
		PropertyDef(	const char* n,
						const char* t,
						const boost::any& defProp  ) :
			name(n),
			typeName(t),
			defaultValue(defProp)
		{}
		PropertyDef(	String& n,
						String& t,
						const boost::any& defProp ) :
			name(n),
			typeName(t),
			defaultValue(defProp)
		{}
		String			name;
		String			typeName;
		boost::any		defaultValue;
		Property* instantiate() const;
	};
	typedef std::vector<PropertyDef> PropertyDefList;
	typedef std::map<String,PropertyDef> PropertyDefMap;
	class Property
	{
	public:
		template<typename T>
			void setValue(const T& newVal)
		{
			mValue = newVal;
			mDirty = true;
			mSigOnChanged( *this );
		}

		template<typename T>
			T getValueAs() const
		{
			try {
				return boost::any_cast<T>( mValue );
			} catch (boost::bad_any_cast&)
			{
				YAKE_EXCEPT("Property value of incorrect type!");
			}
		}
		bool hasChanged() const
		{
			return mDirty;
		}
		void setHasChanged(const bool hasChanged)
		{
			mDirty = hasChanged;
		}
		YAKE_MEMBERSIGNAL( public, void(Property&), OnChanged );
	private:
		boost::any	mValue;
		bool		mDirty;
	};
	typedef std::vector<Property*> PropertyPtrList;
	typedef std::map<String,Property*> PropertyPtrMap;
	class PropertyContainer
	{
	public:

		Property* getByName(const String& name);
		const Property* getByName(const String& name) const;
		void add(Property* pProperty);
	private:
		PropertyPtrList	mProps;
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
		const PropertyDefMap& getPropertyDef() const
		{ return mPropDefs; }
		PropertyDefMap& getPropertyDef()
		{ return mPropDefs; }
		void addParent(object_class* parent);
		typedef std::vector<object_class*> ClassPtrList;
		const ClassPtrList& getParents() const;
		const ClassPtrList& getChildren() const;
	private:
		String				mName;
		PropertyDefMap		mPropDefs;
		ClassPtrList		mParents;
		ClassPtrList		mChilds;
	};

	#define DECLARE_OBJECT_BASIC(CLASSNAME) \
	public: \
		static ::yake::ent::object_class* getClass(); \
		virtual ::yake::ent::object_class* isA() const;

	#define DECLARE_OBJECT_ROOT(CLASSNAME) \
	public: \
		DECLARE_OBJECT_BASIC(CLASSNAME) \
		static void initEntity(CLASSNAME* pObj); \
		static void regPropDefs_##CLASSNAME(::yake::ent::PropertyDefMap& propDefs) {} \
	private: \
		::yake::ent::PropertyPtrMap	mProps;

	/** Base class providing minimum functionality and the necessary hooks
		for easy extension.
		It supports messaging and dynamic events.
	*/
	class YAKE_ENT_API Object
	{
		friend class sim;
		DECLARE_OBJECT_ROOT(Object)
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

		void addProperty(const String& name, Property* pProp);
		Property* getProperty(const String& name) const;
		bool setProperty(const String& propName, const boost::any& value);

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

#define OBJECT_PROPS_NONE(CLASSNAME) \
		static void regPropDefs##CLASSNAME(::yake::ent::PropertyDefMap& propDefs) {}

#define OBJECT_PROPS_BEGIN(CLASSNAME) \
		static void regPropDefs##CLASSNAME(::yake::ent::PropertyDefMap& propDefs) \
		{

#define OBJECT_PROP(NAME,TYPE,DEFAULTVALUE) \
			propDefs[ NAME ] = ::yake::ent::PropertyDef( NAME, #TYPE, boost::any(TYPE(DEFAULTVALUE)) );

#define OBJECT_PROPS_END() \
		}

	#define DECLARE_OBJECT(CLASSNAME) \
		DECLARE_OBJECT_BASIC(CLASSNAME) \
	private: \
		static Object* create() \
		{ \
			CLASSNAME* pObj = new CLASSNAME(); \
			initEntity(pObj); \
			return pObj; \
		} \
	public: \
		static void reg( ::yake::ent::sim& theSim ); \
		static CLASSNAME* cast( Object* other ) \
		{  return dynamic_cast<CLASSNAME*>(other); } \
		static const CLASSNAME* cast( const Object* other ) \
		{  return dynamic_cast<const CLASSNAME*>(other); }

	#define DEFINE_OBJECT_BASIC(CLASSNAME) \
		::yake::ent::object_class* CLASSNAME::getClass() \
		{ \
			static ::yake::ent::object_class g_class(#CLASSNAME); \
			return &g_class; \
		} \
		::yake::ent::object_class* CLASSNAME::isA() const \
		{ return CLASSNAME::getClass(); }

	#define DEFINE_OBJECT_ROOT(CLASSNAME) \
		DEFINE_OBJECT_BASIC(CLASSNAME) \
		void CLASSNAME::initEntity(CLASSNAME* pObj) \
		{ \
			YAKE_ASSERT( pObj ); \
			if (!pObj) return; \
			applyClassProperties<CLASSNAME>( pObj, *pObj->isA() ); \
		}

	#define DEFINE_OBJECT(CLASSNAME) \
		DEFINE_OBJECT_BASIC(CLASSNAME) \
		void CLASSNAME::reg( ::yake::ent::sim& theSim ) \
		{ \
			CLASSNAME::regPropDefs##CLASSNAME(getClass()->getPropertyDef()); \
			theSim.regEntityCreator(#CLASSNAME, &CLASSNAME::create); \
		}

	#define DEFINE_OBJECT_1(CLASSNAME,PARENT0) \
		DEFINE_OBJECT_BASIC(CLASSNAME) \
		void CLASSNAME::reg( ::yake::ent::sim& theSim ) \
		{ \
			getClass()->addParent(PARENT0::getClass()); \
			CLASSNAME::regPropDefs##CLASSNAME(getClass()->getPropertyDef()); \
			theSim.regEntityCreator(#CLASSNAME, &CLASSNAME::create); \
		}

	template<class T>
		bool setObjectProperty(T* pObj, const String& propName, const boost::any& value)
	{
		YAKE_ASSERT(pObj).warning("Valid object expected.");
		if (!pObj)
			return false;
		ent::Property* pProp = pObj->getProperty(propName);
		YAKE_ASSERT(pProp).warning("Could not retrieve property.");
		if (!pProp)
			return false;
		pProp->setValue( value );
		return true;
	}
} // namespace yake
} // namespace ent
#endif
