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
#ifndef YAKE_MSG_LISTENER_H
#define YAKE_MSG_LISTENER_H

#include "yake/msg/prerequisites.h"

MSG_NAMESPACE_BEGIN

	namespace detail {
		/** Internal base class for listeners providing the basic interface and functionality. */
		struct listener_base
		{
		private:
			listener_base(const listener_base&);
			listener_base& operator = (const listener_base&);
		public:
			listener_base(const std::string& msgTypeName) : msgTypeName_(msgTypeName) {}
			virtual ~listener_base() {}
			void execute(const message_base& msg)
			{ doExecute(msg); }
			const std::string& msgTypeName() const
			{ return msgTypeName_; }
			void destroy()
			{
				assert( this );
				delete this;
			}
		protected:
			virtual void doExecute(const message_base&) = 0;
		private:
			std::string		msgTypeName_;
		};
	} // namespace detail
	/*
	enum ListenerResult
	{
		LR_UNHANDLED,
		LR_HANDLED,
		LR_FAILED
	};
	*/
	/** Represents a listener for a specific type; \
		the listener object encapsulates a user provided message handling function object.
	*/
	template<typename MsgType>
	struct listener : public detail::listener_base
	{
	private:
		listener(const listener&);
		listener& operator=(const listener&);
	public:
		typedef boost::function<void(const MsgType&)> HandlerFn;

		/** Construct a listener object with a handler function. The handler function has to be valid (i.e. non-empty).
		*/
		listener(const HandlerFn& fn) : detail::listener_base(typeid(MsgType).name()),fn_(fn)
		{}
	protected:
		/** Executes the message handler function object and provides the message 'msgbase' as a parameter. */
		virtual void doExecute(const detail::message_base& msgbase)
		{
			try {
				fn_( static_cast<const MsgType&>(boost::any_cast<MsgType>(msgbase.value())) );
			}
			catch( boost::bad_any_cast& e )
			{
				std::cerr << "msg.exception: " << e.what() << "\n";
			}
			catch( ...)
			{
				std::cerr << "msg.exception: unhandled exception\n";
			}
		}
	private:
		HandlerFn			fn_;
	};

	/** Helper function to easily create listener objects for a given message type. */
	template<typename T>
	listener<T>* makeListener(const boost::function<void(const T&)>& fn)
	{ return new listener<T>(fn); }

MSG_NAMESPACE_END

#endif
