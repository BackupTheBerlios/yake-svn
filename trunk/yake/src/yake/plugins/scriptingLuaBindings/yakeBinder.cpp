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
#include <yake/plugins/scriptingLuaBindings/pch.h>
#include <yake/plugins/scriptingLuaBindings/yakeBinder.h>
#include <yake/plugins/scriptingLua/ScriptingSystemLua.h>

// headers with entities to bind
#include <yake/base/yakeTimer.h>

namespace luabind {
namespace converters {

	yes_t is_user_defined(by_value<yake::base::String>);
	yes_t is_user_defined(by_const_reference<yake::base::String>);

	void convert_cpp_to_lua(lua_State* L, const yake::base::String& s)
	{
		lua_pushstring(L, s.c_str());
	}

	yake::base::String convert_lua_to_cpp(
	lua_State* L, by_value<yake::base::String>, int index)
	{
		return yake::base::String(lua_tostring(L, index));
	}

	yake::base::String convert_lua_to_cpp(
	lua_State* L, by_const_reference<yake::base::String>, int index)
	{
		return yake::base::String(lua_tostring(L, index));
	}

	int match_lua_to_cpp(
	lua_State* L, by_value<yake::base::String>, int index)
	{
		if (lua_type(L, index) == LUA_TSTRING) return 0;
		else return -1;
	}

	int match_lua_to_cpp(
	lua_State* L, by_const_reference<yake::base::String>, int index)
	{
		if (lua_type(L, index) == LUA_TSTRING) return 0;
		else return -1;
	}

}
}


namespace yake {
namespace scripting {
namespace binder {
namespace lua {

	struct Player
	{
		String	mName;

		void setName( const String & name )
		{ mName = name; }
		String getName() const
		{ return mName; }
	};


	/**
	 *        Bind classes to script engine
	 * @param pVM - pointer to script engine Virtual Machine
	 */
	void Binder::bind(scripting::IVM* pVM)
	{
		YAKE_ASSERT( pVM );
		if (!pVM)
			return;

		LuaVM* pLuaVM = static_cast<LuaVM*>(pVM);
		YAKE_ASSERT( pLuaVM );

		lua_State* engineState = pLuaVM->getLuaState();
		YAKE_ASSERT( engineState );
		
		using namespace luabind;

		#define YAKE_MODULE engineState, "yake"
		
		luabind::open( engineState );
		module( YAKE_MODULE )
		[
			class_< Version >( "Version" )
				.def(constructor<uint16,uint16,uint16>())
//				.def("major", &yake::base::Version::major)
		];

		// yake::base bindings
		
		// Library
		module( YAKE_MODULE )
		[
			class_< Library >( "Library" )
				.def( constructor< String const& >() )
				.def( "getSymbol", &yake::base::Library::getSymbol )
		];
		
		// Timer
		module( YAKE_MODULE )
		[
			class_< Timer >( "Timer" )
				.def( "reset", &yake::base::Timer::reset )
				.def( "getMilliseconds", &yake::base::Timer::getMilliseconds )
				.def( "getSeconds", &yake::base::Timer::getSeconds ) 
		];
		
		module( YAKE_MODULE )
		[
			//def( "createTimer", &yake::base::timer::createTimer )
			def( "getTime", &yake::base::native::getTime )
		];
		
		// Log
		module( YAKE_MODULE )
		[
			class_< Log >( "Log" )
				.enum_( "Severity" )
				[
					value( "INFORMATIONS", 0 ),
					value( "WARNINGS", 1 ),
					value( "ERRORS", 2 )
				]
				.def( constructor<>() ) 
				.def( "onLog", &yake::base::Log::onLog )
//				.def( "log", &yake::base::Log::log )
				//.def( "logPrintf", &yake::base::Log::logPrintf ) // not typesafe
		];
		////TODO	define logging shortcuts like YAKE_LOG_ERROR etc.
		////TODO	log singleton interface
		
		// Plugin
		module( YAKE_MODULE )
		[
			class_< Plugin >( "Plugin" )
				.def( "getName", &yake::base::Plugin::getName )
				.def( "initialise", &yake::base::Plugin::initialise )
				.def( "shutdown", &yake::base::Plugin::shutdown )
		];
		
		// Math
		module( YAKE_MODULE )
		[
			class_< Math >( "Math" )
				.enum_( "AngleUnit" )
				[
					value( "AU_DEGREE", 0 ),
					value( "AU_RADIAN", 1 )
				]
				.def( constructor< unsigned int >() )
				//,def( "IAbs", &yake::base::math::Math::IAbs )
		];
		
		// Vector3
		module( YAKE_MODULE )
		[
			class_< Vector3 >( "Vector3" )
				.def_readwrite( "x", &yake::base::math::Vector3::x )
				.def_readwrite( "y", &yake::base::math::Vector3::y )
				.def_readwrite( "z", &yake::base::math::Vector3::z )
				.def( constructor< real, real, real >() )
				.def( constructor< const real* const >() )
				.def( constructor< Vector3 const& >() )
		];
		
		// Vector4
		module( YAKE_MODULE )
		[
			class_< Vector4 >( "Vector4" )
				.def( constructor<>() )
				.def( constructor< real, real, real, real >() )
				.def( constructor< const real* const >() )
				.def( constructor< Vector4 const& >() )
		];
		
		// Matrix3
		module( YAKE_MODULE )
		[
			class_< Matrix3 >( "Matrix3" )
				.def( constructor<>() )
				.def( constructor< Matrix3 const& >() )
				.def( "getColumn", &yake::base::math::Matrix3::GetColumn ) 
		];
		
		// Quaternion
		module( YAKE_MODULE )
		[
			class_< Quaternion >( "Quaternion" )
				.def_readwrite( "x", &yake::base::math::Quaternion::x )
				.def_readwrite( "y", &yake::base::math::Quaternion::y )
				.def_readwrite( "z", &yake::base::math::Quaternion::z )
				.def_readwrite( "w", &yake::base::math::Quaternion::w )
				.def( constructor< real, real, real, real >() )
				.def( constructor< Quaternion const& >() )
		];
		
		// RandomNumberGenerator
		module( YAKE_MODULE )
		[
			class_< RandomNumberGeneratorMT >( "RandomNumberGeneratorMT" )
				.def( constructor<>() )
		];
	}


}
}
}
}
