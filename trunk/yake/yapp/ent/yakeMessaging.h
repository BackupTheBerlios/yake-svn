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
#ifndef YAKE_ENT_MESSAGING_H
#define YAKE_ENT_MESSAGING_H

namespace yake {
namespace ent {

	typedef msg::MessageId				MessageId;
	typedef msg::Message				Message;
	typedef msg::MessageQ				MessageQ;
	typedef msg::ResultCode				MsgResultCode;
	typedef msg::MessageHandler			MessageHandler;
	typedef	msg::MessageHandlerFn		MessageHandlerFn;
	typedef msg::MessageIdManager		MessageIdManager;
	//typedef yake::msg::MessageParams		MessageParams;

	extern YAKE_ENT_API const MessageId MSGID_EntitySpawned;
	extern YAKE_ENT_API const MessageId CMDID_MachineEnterState;
	extern YAKE_ENT_API const MessageId CMDID_MachineExitState;
	extern YAKE_ENT_API const MessageId CMDID_MachineChangeTo;
	extern YAKE_ENT_API const MessageId MSGID_MachineStateEntered;
	extern YAKE_ENT_API const MessageId MSGID_MachineStateLeft;

	class YAKE_ENT_API MessageParams
	{
	public:
		MessageParams()
		{ mNone = false; }
		void set(const String& name, const boost::any& value)
		{ mParams[name] = value; }
		const boost::any& get(const String& name) const
		{ 
			List::const_iterator itFind = mParams.find(name);
			if (itFind == mParams.end())
				return mNone;
			return itFind->second; 
		}
	private:
		typedef std::map<String,boost::any> List;
		List		mParams;
		boost::any	mNone;
	};

	class Object;
	class ObjectMessage : public Message
	{
		friend class Object;
	protected:
		ObjectMessage(const MessageId& id, const Object* o) :
			 msg::Message(id,o,true)
		{}
	public:
		const MessageParams& getParams() const
		{ return mParams; }
		void set(const String& name, const boost::any& value)
		{ mParams.set(name,value); }
		const boost::any& get(const String& name) const
		{ return mParams.get(name); }
	private:
		MessageParams	mParams;
	};

} // namespace yake
} // namespace ent
#endif
