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
#ifndef YAKE_MSG_MESSAGE_H
#define YAKE_MSG_MESSAGE_H

#include "yake/msg/prerequisites.h"

MSG_NAMESPACE_BEGIN

	struct message_base
	{
	private:
		message_base(const message_base&);
		message_base& operator=(const message_base&);
	public:
		template<typename T>
		message_base(const T& value) : value_(value), msgTypeName_(typeid(T).name())
		{}
		const boost::any& value() const
		{ return value_; }
		const std::string& msgTypeName() const
		{ return msgTypeName_; }
	protected:
		boost::any		value_;
	private:
		std::string		msgTypeName_;
	};
	template<typename T>
	struct message : public message_base
	{
		message(const T& v) : message_base(v)
		{}
		message() : message_base(T())
		{}
		void setValue(const T& v)
		{ value_ = v; } //@todo add type mismatch tests
	};

	struct DefaultMessageAllocator
	{
		DefaultMessageAllocator& instance();
		template<typename T>
		static message<T>* create(const T& v)
		{
			return new message<T>(v);
		}
		template<typename MsgType>
		static void destroy(const MsgType* msg)
		{
			assert( msg );
			delete msg;
		}
	};

	template<typename T>
	message<T>* makeMessage(const T& v)
	{ 
		return DefaultMessageAllocator::create<T>(v); 
	}
	template<typename T>
	void destroyMessage(T* msg)
	{ 
		return DefaultMessageAllocator::destroy(msg); 
	}

MSG_NAMESPACE_END

#endif
