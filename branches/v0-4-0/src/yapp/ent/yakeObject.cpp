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
//#include <yapp/ent/yakeSim.h>

namespace yake {
namespace ent {

	void object_class::addParent(object_class* parent)
	{
		YAKE_ASSERT( parent );
		if (!parent)
			return;
		mParents.push_back( parent );
		parent->mChilds.push_back( this );
	}
	const object_class::ClassPtrList& object_class::getParents() const
	{
		return mParents;
	}
	const object_class::ClassPtrList& object_class::getChildren() const
	{
		return mChilds;
	}

	template<class ObjectType>
		void applyClassProperties(ObjectType* pObj, const object_class& objectClass)
	{
		YAKE_ASSERT( pObj );
		if (!pObj) return;
		object_class::ClassPtrList classes = objectClass.getParents();
		ConstVectorIterator< object_class::ClassPtrList > itParent( classes );
		while (itParent.hasMoreElements())
		{
			applyClassProperties(pObj, *(itParent.getNext()));
		}
		PropertyDefMap::const_iterator itDefEnd = objectClass.getPropertyDef().end();
		for (PropertyDefMap::const_iterator itDef = objectClass.getPropertyDef().begin(); itDef != itDefEnd; ++itDef)
		{
			pObj->addProperty( itDef->second.name, itDef->second.instantiate() );
		}
	}

	DEFINE_OBJECT_ROOT( Object )

	Object::Object() : mEvtSpawn("onSpawn"), mEvtTick("onTick")
	{
		static ObjectId lastId = 1;
		mId = lastId++;

		regEvent(&mEvtSpawn);
		regEvent(&mEvtTick);
	}
	void Object::initialise(object_creation_context& creationCtx)
	{
		onInitialise(creationCtx);
	}
	bool Object::isServer() const
	{
		return true;
	}
	bool Object::fireEvent(const String& name)
	{
		YAKE_ASSERT( !name.empty() ).warning("need a name");
		if (name.empty())
			return false;
		Event* pEvent = getEventByName(name);
		YAKE_ASSERT( pEvent )(name).warning("Object does not have this event");
		if (!pEvent)
			return false;
		pEvent->fire( getDefaultEventParams() );
		return true;
	}
	ParamList Object::getDefaultEventParams()
	{
		ParamList params;
		params["object"] = this;
		onGetDefaultEventParams(params);
		return params;
	}
	void Object::spawn()
	{
		onSpawn();
		mEvtSpawn.fire(getDefaultEventParams());
	}
	void Object::tick()
	{
		mMessageManager.execute();

		onTick();
		mEvtTick.fire(getDefaultEventParams());
	}
	ObjectId Object::getId() const
	{
		return mId;
	}
	Event* Object::getEventByName(const String& name) const
	{
		EventMap::const_iterator itFind = mEvents.find(name);
		if (mEvents.end() == itFind)
			return 0;
		return itFind->second;
	}
	void Object::addEvent(Event* pEvent)
	{
		if (pEvent->getName().empty() || !pEvent)
			return;
		mEvents[ pEvent->getName() ] = pEvent;
		mDynEvents.push_back( SharedPtr<Event>(pEvent) );
	}
	void Object::regEvent(Event* pEvent)
	{
		if (pEvent->getName().empty() || !pEvent)
			return;
		mEvents[ pEvent->getName() ] = pEvent;
	}
	ObjectMessage* Object::createMessage(const MessageId& id)
	{
		return new ObjectMessage(id,this);
	}
	void Object::addMessageHandler(const MessageId& id, const MessageHandlerFn& fn, Object* source /*= 0*/)
	{
		mMessageManager.addMessageHandler( id, fn, source );
	}
	void Object::addProperty(const String& name, Property* pProp)
	{
		YAKE_ASSERT( !name.empty() ).debug("No property name given!");
		if (!pProp)
			return;
		PropertyPtrMap::const_iterator itFind = mProps.find(name);
		if (itFind != mProps.end())
			delete itFind->second;
		mProps[ name ] = pProp;
	}
	Property* Object::getProperty(const String& name) const
	{
		PropertyPtrMap::const_iterator itFind = mProps.find(name);
		if (itFind != mProps.end())
			return itFind->second;
		return 0;
	}
	bool Object::setProperty(const String& propName, const boost::any& value)
	{
		ent::Property* pProp = getProperty(propName);
		YAKE_ASSERT(pProp).debug("Could not retrieve property.");
		if (!pProp)
			return false;
		pProp->setValue( value );
		return true;
	}

	Property* PropertyDef::instantiate() const
	{
		Property* pProp = new Property();
		pProp->setValue( defaultValue );
		return pProp;
	}


} // namespace yake
} // namespace ent
