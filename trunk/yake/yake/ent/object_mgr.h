/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright (c) The YAKE Team
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
#ifndef YAKE_ENT_OBJECTMANAGER_H
#define YAKE_ENT_OBJECTMANAGER_H

#include "yake/ent/prerequisites.h"
#include "yake/ent/object.h"
#include "yake/ent/object_mgr_listener.h"
#include "yake/msg/yakeMessage.h"

namespace yake {
namespace ent {

	typedef object::ResultCode RegistrationResultCode;
	typedef std::pair<RegistrationResultCode,ClassId> RegistrationResult;

	struct YAKE_ENT_API ObjectManager : public ListenerManager<ObjectManagerListener>
	{
		// operations
		Object* makeObject(const ClassId clsId);
		Object* makeObject(const std::string strClsId);
		void destroyObject(Object*);
		void tick();

		template<typename T>
		inline RegistrationResult registerClass(const std::string& name)
		{
			return objMgr_.registerClass(name,&T::create,&T::destroy);
		}

		/** Post a global message. This message is *not* broadcasted to every object!
			@see msg::makeMessage
			@see msg::makeListener
			@see broadcastMessage
		*/
		void postMessage(msg::detail::message_base*, const msg::Source source = msg::kNullSource);

		/** Post a global message which is broadcasted to every object!
			@see msg::makeMessage
			@see msg::makeListener
		*/
		void broadcastMessage(msg::detail::message_base*, const msg::Source source = msg::kNullSource);

		/** Post message to a specific object.
			@see msg::makeMessage
			@see msg::makeListener
		*/
		void postMessage(Object*, msg::detail::message_base*, const msg::Source source = msg::kNullSource);

		/** Post message to a specific object.
			@see msg::makeMessage
			@see msg::makeListener
		*/
		void postMessage(ObjectId, msg::detail::message_base*, const msg::Source source = msg::kNullSource);

		/** Post message to objects in a STL(-like) container with object pointers.
			@see msg::makeMessage
			@see msg::makeListener
		*/
		template<typename T_container>
		void postMessage(const T_container& objCtr, msg::detail::message_base* msg, const msg::Source source = msg::kNullSource)
		{
			ConstVectorIterator<T_container> it(objCtr);
			while (it.hasMoreElements())
				postMessage( it.getNext(), msg, source );
		}
	protected:
		//
		typedef ListenerManager<ObjectManagerListener> listener_mgr_type;
		void listeners_onObjectCreated(Object* obj);
		void listeners_onObjectInitialized(Object* obj);
		void listeners_onDestroyObject(Object* obj);
	private:
		typedef std::deque<Object*> object_ptr_list;
		object_ptr_list						objs_;

		typedef object::ObjectManager<Object> obj_mgr_type;
		obj_mgr_type							objMgr_;
	};

} // namespace ent
} // namespace yake

#endif
