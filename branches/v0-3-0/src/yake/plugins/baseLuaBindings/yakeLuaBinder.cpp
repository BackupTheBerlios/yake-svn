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
#include <yake/base/yake.h>
#include <yake/plugins/baseLuaBindings/yakePrerequisites.h>
#include <yake/scripting/yakeScriptingSystem.h>
#include <yake/plugins/scriptingLua/ScriptingSystemLua.h>
#include <yake/plugins/baseLuaBindings/yakeLuaBinder.h>
#include <luabind/luabind.hpp>


#include <yake/graphics/yakeGraphicsSystem.h>
#include <yake/graphics/yakeGeometryAccess.h>

namespace yake
{
	namespace base
	{
		namespace lua
		{
			YAKE_REGISTER_CONCRETE( MathBinder );
			
			void MathBinder::bind( scripting::IVM* pVM )
			{
				scripting::LuaVM* pL = static_cast<scripting::LuaVM*>(pVM);
				YAKE_ASSERT( pL );
				if (!pL)
					return;

				using namespace luabind;
				using namespace yake::math;

#define YAKE_MATH_MODULE pL->getLuaState(), "yake"

				///TODO Bind most common operators!

				module( YAKE_MATH_MODULE )
				[
				    class_<Color>("Color")
				    .def( constructor<>() )
				    .def( constructor<real,real,real,real>() )
				    //FIXME some strange members in anonymous union::struct
				    /*						.def_readwrite( "r", &Color::r )
				    						.def_readwrite( "g", &Color::g )
				    						.def_readwrite( "b", &Color::b )
				    						.def_readwrite( "a", &Color::a )*/
				];

				//TODO Bind the whole math::Math class
				// Math
				module( YAKE_MATH_MODULE )
				[
				    class_< Math >( "Math" )
				    .enum_( "AngleUnit" )
				    [
				        value( "AU_DEGREE", 0 ),
				        value( "AU_RADIAN", 1 )
				    ]
				    .def( constructor< unsigned int >() )
				    //,def( "IAbs", &yake::math::Math::IAbs )
				];

				// Vector3
				module( YAKE_MATH_MODULE )
				[
				    class_< Vector3 >( "Vector3" )
				    .def_readwrite( "x", &yake::math::Vector3::x )
				    .def_readwrite( "y", &yake::math::Vector3::y )
				    .def_readwrite( "z", &yake::math::Vector3::z )
				    .def( constructor< real, real, real >() )
				    .def( constructor< const real* const >() )
				    .def( constructor< Vector3 const& >() )
				];

				// Vector4
				module( YAKE_MATH_MODULE )
				[
				    class_< Vector4 >( "Vector4" )
				    .def( constructor<>() )
				    .def( constructor< real, real, real, real >() )
				    .def( constructor< const real* const >() )
				    .def( constructor< Vector4 const& >() )
				];

				// Matrix3
				module( YAKE_MATH_MODULE )
				[
				    class_< Matrix3 >( "Matrix3" )
				    .def( constructor<>() )
				    .def( constructor< Matrix3 const& >() )
				    .def( "getColumn", &yake::math::Matrix3::GetColumn )
				];

				// Quaternion
				module( YAKE_MATH_MODULE )
				[
				    class_< Quaternion >( "Quaternion" )
				    .def_readwrite( "x", &yake::math::Quaternion::x )
				    .def_readwrite( "y", &yake::math::Quaternion::y )
				    .def_readwrite( "z", &yake::math::Quaternion::z )
				    .def_readwrite( "w", &yake::math::Quaternion::w )
				    .def( constructor< real, real, real, real >() )
				    .def( constructor< Quaternion const& >() )
				];

				// RandomNumberGenerator
				module( YAKE_MATH_MODULE )
				[
				    class_< RandomNumberGeneratorMT >( "RandomNumberGeneratorMT" )
				    .def( constructor<>() )
				];

			}

			YAKE_REGISTER_CONCRETE( Binder );
			
			void Binder::bind( scripting::IVM* pVM )
			{
				scripting::LuaVM* pL = static_cast<scripting::LuaVM*>(pVM);
				YAKE_ASSERT( pL );
				if (!pL)
					return;

				using namespace luabind;

#define YAKE_BASE_MODULE pL->getLuaState(), "yake"

				module( YAKE_BASE_MODULE )
				[
				    class_<Movable>( "Movable" )
				    .def( "setPosition", &Movable::setPosition )
				    .def( "setOrientation", &Movable::setOrientation )
				    .def( "getPosition", &Movable::getPosition )
				    .def( "getOrientation", &Movable::getOrientation )
				    .def( "translate", &Movable::translate )
				];

				module( YAKE_BASE_MODULE )
				[
				    class_< Version >( "Version" )
				    .def(constructor<uint16,uint16,uint16>())
				    .def("getMajor", &yake::Version::getMajor)
				    .def("getMinor", &yake::Version::getMinor)
				    .def("getSub", &yake::Version::getSub)
				];

				// Library
				module( YAKE_BASE_MODULE )
				[
				    class_< Library >( "Library" )
				    .def( constructor< String const& >() )
				    //.def( "getSymbol", &yake::base::Library::getSymbol )
				];

				// Timer
				module( YAKE_BASE_MODULE )
				[
				    //def("createTimer", &yake::base::timer::createTimer())
				    class_< Timer >( "Timer" )
				    .def( "reset", &yake::base::Timer::reset )
				    .def( "getMilliseconds", &yake::base::Timer::getMilliseconds )
				    .def( "getSeconds", &yake::base::Timer::getSeconds )
				];

				module( YAKE_BASE_MODULE )
				[
				    //def( "createTimer", &yake::base::timer::createTimer )
				    def( "getTime", &yake::base::native::getTime )
				];

				// Log
				module( YAKE_BASE_MODULE )
				[
				    class_< Log >( "Log" )
				    .enum_( "Severity" )
				    [
				        value( "INFORMATIONS", 0 ),
				        value( "WARNINGS", 1 ),
				        value( "ERRORS", 2 )
				    ]
				    .def( constructor<>() )
				    .def( "onLog", &Log::onLog )
				    .def( "log", (void(Log::*)( const String&,Log::Severity))&Log::log )
				    .def( "logAt", (void(Log::*)( const String&,Log::Severity,const String&))&Log::log )
				    .def( "instance", &Log::instance )
				];

				// Log shortcuts
				module( YAKE_BASE_MODULE )
				[
				    def( "log_warning", &yake::base::log_warning ),
				    def( "log_information", &yake::base::log_information ),
				    def( "log_error", &yake::base::log_error )
				];

				// Plugin
				module( YAKE_BASE_MODULE )
				[
				    class_< Plugin >( "Plugin" )
				    .def( "getName", &yake::base::Plugin::getName )
				    .def( "initialise", &yake::base::Plugin::initialise )
				    .def( "shutdown", &yake::base::Plugin::shutdown )
				];

				// uniqueName::create
				module( YAKE_BASE_MODULE )
				[
				    def( "createUniqueName", &uniqueName::create )
				];
				
			}


		} // namespace lua
	} // namespace base
} // namespace yake
