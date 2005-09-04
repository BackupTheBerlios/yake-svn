/**------------------------------------------------------------------------------------
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
*/
#include <yapp/plugins/modelLua/pch.h>
#include <yapp/plugins/modelLua/yakeLuaBinder.h>
#include <luabind/luabind.hpp>

#include <yapp/model/yakeGraphical.h>

namespace yake
{
	namespace model
	{
		namespace lua
		{
			YAKE_REGISTER_CONCRETE( Binder );
			void Binder::bind( scripting::IVM* pVM )
			{
				scripting::LuaVM* pL = static_cast<scripting::LuaVM*>(pVM);
				YAKE_ASSERT( pL );
				if (!pL)
					return;

				using namespace luabind;
				using namespace yake::model;

#define YAKE_MODEL_MODULE pL->getLuaState(), "yake"

				module( YAKE_MODEL_MODULE )
				[
				    class_<Graphical>( "Graphical" )
					.def( constructor<>() )
					.def( "addSceneNode", &Graphical::addSceneNode )
					.def( "getSceneNodes", &Graphical::getSceneNodes )
					.def( "getSceneNode", &Graphical::getSceneNode )
					.def( "fromDotScene", &Graphical::fromDotScene )
				];

			}


		} // namespace lua
	} // namespace model
} // namespace yake
