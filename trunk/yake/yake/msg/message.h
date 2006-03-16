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

	/** Represents the (optionally to be used) source of a message. */
	typedef const void* source_type;

	/** @copydoc source_type
		@Remarks This is an alias for good looking client code.
	*/
	typedef source_type Source;

	namespace detail {
		/** Internal base class for messages providing the very basic interface.
		*/
		struct message_base
		{
		private:
			message_base(const message_base&);
			message_base& operator=(const message_base&);
		public:
			template<typename T>
			message_base(const T& value, source_type src = 0) : value_(value), msgTypeName_(typeid(T).name()), src_(src)
			{}
			const boost::any& value() const
			{ return value_; }
			const std::string& msgTypeName() const
			{ return msgTypeName_; }
			source_type source() const
			{ return src_; }
		protected:
			boost::any		value_;
		private:
			std::string		msgTypeName_;
			source_type		src_;
		};
	} // namespace detail
	/** Represents a concrete message for the type T.
		Provides value assignment.
	*/
	template<typename T>
	struct message : public detail::message_base
	{
		/** Construct a message with value and optional source (0 by default).
			
		*/
		message(const T& v, source_type src = 0) : detail::message_base(v,src)
		{}
		/** Construct a message with default value for type T and source 0. */
		message() : detail::message_base(T())
		{}
		/** Set the value to v. */
		void setValue(const T& v)
		{ value_ = v; } //@todo add type mismatch tests!?
	};

	/** As the name implies this allocator is used by default for message creation and destruction. */
	struct DefaultMessageAllocator
	{
		DefaultMessageAllocator& instance();
		template<typename T>
		static message<T>* create(const T& v, source_type src = 0)
		{
			return new message<T>(v,src);
		}
		template<typename MsgType>
		static void destroy(const MsgType* msg)
		{
			YAKE_ASSERT( msg );
			delete msg;
		}
	};

	/** Helper function to easily create messages.
	*/
	template<typename T>
	message<T>* makeMessage(const T& v, source_type src = 0)
	{ 
		return DefaultMessageAllocator::create<T>(v,src); 
	}

	/** Destroys the message. */
	template<typename T>
	void destroyMessage(T* msg)
	{ 
		return DefaultMessageAllocator::destroy(msg); 
	}

MSG_NAMESPACE_END

#endif
