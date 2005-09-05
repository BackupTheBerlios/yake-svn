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
#ifndef YAKE_CEGUI_OUTPUT_AREA_H
#define YAKE_CEGUI_OUTPUT_AREA_H

#include <CEGUI/CEGUI.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/elements/CEGUIListbox.h>
#include <CEGUI/elements/CEGUIListboxTextItem.h>

namespace yake
{
	namespace gui
	{

		/**	Output component for using with generic console.
		 * Uses CEGUI::Listbox to represent console output area. 
		 * Implements scrolling in a bash-like manner
		*/
		template <class MessageType>
		class CEGUIOutputArea
		{
		public:
			/**
			*     Set Listbox widget that'll represent console output area
			* @param listbox Pointer to widget
			*/
			void setOutputWidget( CEGUI::Listbox* listbox )
			{
				YAKE_ASSERT( listbox != NULL ).error( "How unkind of you to provide such a shit" );
				outputBox_ = listbox;
			}

		protected:
			typedef typename MessageType::char_t	char_t;
			typedef typename MessageType::string_t	string_t;
			typedef typename MessageType::ListType	MessageList;

			/**
			 *     Rendering function is required by Console interface. 
			 * In case of Lua, rendering occurs implicitly, so function body is empty.
			 */
			void render()
			{
				//does nothing here, 'cause CEGUI widget performs render on item addition
			}

			/**
			 *     Add a message to output
			 * @param msg Message to add
			 */
			void addMessage( const MessageType& msg )
			{
				CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem( msg.text_.c_str() );

				outputBox_->addItem( item );

				visibleItemIndex_ = outputBox_->getItemCount();

				outputBox_->ensureItemIsVisible( visibleItemIndex_ );
			}

			/**
			 *     Scroll output up by a given amount of lines
			 * @param lines Lines quantity
			 */
			void scrollUp( std::size_t lines )
			{
				if ( visibleItemIndex_ == 0 )
					return;

				if ( visibleItemIndex_ == outputBox_->getItemCount() )
					lines *= 2;

				if ( int( visibleItemIndex_ ) - int( lines ) > 0 )
					visibleItemIndex_ -= lines;
				else
					visibleItemIndex_ = 0;

				outputBox_->ensureItemIsVisible( visibleItemIndex_ );
			}

			/**
			 *     Scroll output down by a given amount of lines
			 * @param lines Lines quantity
			 */
			void scrollDown( std::size_t lines )
			{
				if ( visibleItemIndex_ == 0 )
					lines *= 2;

				if ( visibleItemIndex_ + lines < outputBox_->getItemCount() )
					visibleItemIndex_ += lines;
				else
					visibleItemIndex_ = outputBox_->getItemCount();

				outputBox_->ensureItemIsVisible( visibleItemIndex_ );
			}

			/**
					 *     Clear scrolling.
					 * The very last output message becomes visible
			 */
			void clearScroll()
			{
				visibleItemIndex_ = outputBox_->getItemCount();
				outputBox_->ensureItemIsVisible( visibleItemIndex_ );
			}

		private:
			/// CEGUI widget representing output area
			CEGUI::Listbox*	outputBox_;

			/// Index of visible item, used for scrolling
			std::size_t			visibleItemIndex_;
		};

	}
}

#endif
