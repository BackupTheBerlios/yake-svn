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
#ifndef YAKE_GENERIC_CONSOLE_H
#define YAKE_GENERIC_CONSOLE_H

namespace yake
{
namespace gui
{

/** Class represents generic shell-like console.
 * Console behaviour is controlled via policies. Fine-tuning of policies is done through
 * their public interface. 
 * On the other hand all methods that are for use by Console internally are protected
 * in policies. So at first user has to setup policies,
 * then access console functionality via 
 * Console public interface.
 */
template <	class MessageType,
class InputPolicy,
class Processor,
template <class> class HistoryPolicy,
template <class> class OutputPolicy >
		class Console :	public InputPolicy,
 						public Processor,
						public HistoryPolicy<MessageType>,
						public OutputPolicy<MessageType>
{
	public:
		typedef typename MessageType::char_t	char_t;
		typedef typename MessageType::string_t	string_t;
		typedef typename MessageType::ListType	MessageList;

	/**
		 * Add single character to command line
		 * @param c character to add
	 */
		void addCommandCharacter( char_t c )
		{
			InputPolicy::addCharacter( c );
		}

	/**
		 * Add text string to command line
		 * @param text string to add
	 */
		void addCommandText( const string_t& text )
		{
			InputPolicy::addText( text );
		}

	/**
		 *    Clear command line
	 */
		void clearCommandLine()
		{
			InputPolicy::clearCommandLine();
		}

	/**
		 *    Set command in command line 
		 * @param cmd appears in command line after call
	 */
		void setCommand( const string_t& cmd )
		{
			InputPolicy::setCommand( cmd );
		}
	
	/**
		 *    Add message to output
		 * @param msg message to add
	 */
		void addMessage( const MessageType& msg )
		{
			MessageList messages;
			messages.push_back( msg );
		
			OutputPolicy<MessageType>::addMessage( msg );
		}

	/**
		 *    Executes, processes command that is currently on command line.
		 *	If output was scrolled scroll is reset. Command goes to history.
	 */
		void execute()
		{
			string_t cmd = InputPolicy::getCommandLine();

			InputPolicy::clearCommandLine();

			addCommand( cmd );

			Processor::process( cmd );

			OutputPolicy<MessageType>::clearScroll();
			OutputPolicy<MessageType>::render();
		}

	/**
		 *    Puts previously executed command (from history) to command line.
		 * Output scrolling is reset.
	 */
		void previousCommand()
		{
			OutputPolicy<MessageType>::clearScroll();
		
			const string_t cmd = HistoryPolicy<MessageType>::getPreviousCommand();

			InputPolicy::setCommand( cmd );
		}

	/**
		 *    Puts next executed command (from history) to command line.
		 * Output scrolling is reset.
	 */
		void nextCommand()
		{
			OutputPolicy<MessageType>::clearScroll();
		
			const string_t cmd = HistoryPolicy<MessageType>::getNextCommand();

			InputPolicy::setCommand( cmd );
		}

	/**
		 *    Scrolls output lines up by a given amount
		 * @param lines quantity of lines to scroll
	 */
		void scrollOutputUp( std::size_t lines = 1 )
		{
			OutputPolicy<MessageType>::scrollUp( lines );
		}

	/**
		 *    Scrolls output lines down by a given amount
		 * @param lines quantity of lines to scroll
	 */
		void scrollOutputDown( std::size_t lines = 1 )
		{
			OutputPolicy<MessageType>::scrollDown( lines );
		}
};

}
}

#endif
