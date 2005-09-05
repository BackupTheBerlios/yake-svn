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
#ifndef YAKE_CONSOLE_LUA_PROCESSOR_H
#define YAKE_CONSOLE_LUA_PROCESSOR_H

#include <yake/base/yake.h>
#include <yake/scripting/yakeScriptingSystem.h>
#include <yake/plugins/scriptingLua/ScriptingSystemLua.h>

namespace yake
{
	namespace gui
	{

		/**	Processor class for use with generic console.
		 * Command is being processed by Lua interpreter.
		 */
		class LuaInterpreter
		{
		public:
			typedef yake::String	string_t;

			/**
				 *     Set scripting virtual machine to use as input interpreter
				 * @param pLuaVM Pointer to Lua virtual machine
			 */
			void setScriptingVirtualMachine( yake::scripting::IVM* pLuaVM )
			{
				YAKE_ASSERT( pLuaVM != NULL ).error( "Oh no! How could you do that?!! Monster!" );
				lua_ = pLuaVM;
			}

		protected:
			/**
			 *     Process command ( send to interpreter )
			 * @param cmd Command to execute
			 */
			void process( const string_t& cmd )
			{
				lua_->execute( cmd );
			}

		private:
			/// Pointer to Lua scripting VM
			yake::scripting::IVM*	lua_;
		};

	}
}

#endif
