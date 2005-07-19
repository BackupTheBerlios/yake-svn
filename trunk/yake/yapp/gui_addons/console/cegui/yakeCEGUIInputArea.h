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
#ifndef YAKE_CEGUI_INPUT_AREA_H
#define YAKE_CEGUI_INPUT_AREA_H

#include <CEGUI/CEGUI.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUIWindow.h>


namespace yake
{
	namespace gui
	{

		/**	CEGUI widget based keyboard input policy for use with generic console.
		 */
		class CEGUIInputArea
		{
		public:
			/**
			 *       Set CEGUI::Editbox that will be the actual input receiver
			 * @param box Editbox pointer
			 */
			void setInputWidget( CEGUI::Editbox* box )
			{
				YAKE_ASSERT( box != NULL ).error( "Just horrible!" );

				editBox_ = box;
			}

		protected:
			typedef yake::String	string_t;

			/**
				 *       Add single character to editbox
				 * @param c Char to add
			 */
			void addCharacter( char c )
			{
				CEGUI::String cmd_line = editBox_->getText();

				cmd_line += c;

				editBox_->setText( cmd_line );
			}

			/**
				 *       Add a string with text to editbox
				 * @param text Text to add
			 */
			void addText( const string_t& text )
			{
				CEGUI::String cmd_line = editBox_->getText();

				cmd_line += text.c_str();

				editBox_->setText( cmd_line );
			}

			/**
				 *       Get command line text
				 * @return Text that is currently in editbox
			 */
			string_t getCommandLine()
			{
				return editBox_->getText().c_str();
			}

			/**
				 *       Clear command line. Used in some cases by generic console
			 */
			void clearCommandLine()
			{
				editBox_->setText( "" );
			}

			/**
				 *       Set command line text and place carat at the end
				 * @param cmd Command text to set
			 */
			void setCommand( const string_t& cmd )
			{
				editBox_->setText( cmd.c_str() );
				editBox_->setCaratIndex( cmd.size() );
			}

		private:
			/// Editbox representing command line input field
			CEGUI::Editbox*	editBox_;
		};

	}
}

#endif
