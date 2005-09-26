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
#ifndef YAKE_DEFAULT_HISTORY_H
#define YAKE_DEFAULT_HISTORY_H

#include <yake/base/yake.h>

namespace yake
{
namespace gui
{

/** Default history policy class.
	Implements bash-like command history
 */
template <class MessageType>
class History
{
	protected:
		typedef typename MessageType::char_t	char_t;
		typedef typename MessageType::string_t	string_t;
		typedef typename MessageType::ListType	MessageList;

	/**
		 *    Return command previous to last returned
		 * @return command text
	 */
		string_t getPreviousCommand()
		{
			if ( commands_.empty() )
				return "";

			if ( current_ == commands_.rend() )
			{
				current_ = commands_.rbegin();
				return *current_;
			}

			if ( current_ == ( commands_.rend() - 1 ) )
				return *current_;
			else
				return *( ++current_ );
		}

	/**
		 *    Return command next to last returned
		 * @return Command text
	 */
		string_t getNextCommand()
		{
			if ( commands_.empty() || current_ == commands_.rend() || current_ == commands_.rbegin() )
				return "";
			else
				return *( --current_ );
		}

	/**
		 *    Add command to history list
		 * @param cmd Command text
	 */
		void addCommand( const string_t& cmd )
		{
			commands_.push_back( cmd );

			current_ = commands_.rend();
		}

	private:
	/// Type to store command list
		typedef yake::Vector<string_t> command_list_t;

		command_list_t										commands_; ///< Saved commands
		typename command_list_t::reverse_iterator	current_; ///< Current command pointer
};

}
}

#endif 
