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
*/
#ifndef INC_YAKE_SCRIPTINGSYSTEMLUA_H
#define INC_YAKE_SCRIPTINGSYSTEMLUA_H

struct lua_State;

namespace yake {
	using namespace base;
namespace scripting {

	class ScriptingSystemLua : public scripting::ScriptingSystem
	{
	public:
		ScriptingSystemLua();
		virtual ~ScriptingSystemLua();

		/** Get the language used by the scripting system.
		*/
		virtual Language getLanguage() const
		{ return L_LUA; }

		/** Get the version of the scripting system. */
		//virtual version getVersion() const
		//{ return version( 0, 1, 0 ); }

		/** Get a unique name for the system (e.g. "yake.core.lua", "yake.graphicsonly.python").
		*/
		virtual String getName() const
		{ return String("yake.core.lua"); }

		/** Create a scipting virtual machine.
		\see IScript
		*/
		virtual scripting::IVM* createVM();

		/** Create a script resource from a file. It can be executed
		by a virtual machine.
		\see IVM
		*/
		virtual scripting::IScript* createScriptFromFile( const String & rFile );

	protected:
		typedef std::vector< scripting::IVM* > VMList;
		VMList	mVMs;
	};

	//-----------------------------------------------------
	const uint32 LUALIB_BASE		= 0x00000001;
	const uint32 LUALIB_TABLE		= 0x00000002;
	const uint32 LUALIB_IO			= 0x00000004;
	const uint32 LUALIB_STRING		= 0x00000008;
	const uint32 LUALIB_MATH		= 0x00000010;
	const uint32 LUALIB_DEBUG		= 0x00000020;

	class LuaVM : public scripting::IVM
	{
	protected:
		ScriptingSystemLua		* mCreator;
		lua_State				* mLuaState;
	public:
		LuaVM( ScriptingSystemLua* pSystem, uint32 libs = LUALIB_BASE|LUALIB_TABLE|LUALIB_STRING|LUALIB_MATH|LUALIB_IO );
		virtual ~LuaVM();

		virtual const scripting::ScriptingSystem* getCreator() const
		{ return mCreator; }

		virtual void execute( const String & rData );

		virtual void execute( scripting::IScript * pScript );

		lua_State* getLuaState() const { return mLuaState; }
	protected:
/*
		class LuaVMPropertyInterface : public PropertyInterface
		{
		protected:
			LuaVM		* mOwner;
		public:
			LuaVMPropertyInterface( LuaVM * pOwner ) : mOwner(pOwner)
			{
			}

			virtual void getProperties( PropertyList & returnList ) const
			{
				returnList.clear();
				returnList.push_back( Property( "luastate", "A Lua virtual machine.") );
			}

			virtual PropertyType getProperty( const String & name ) const
			{
				if (name == "luastate")
				{
					YAKE_ASSERT( mOwner!=0 )( name ).debug("This property interface object should have an owner!");
					if (!mOwner)
						return PropertyType( 0 );
					return PropertyType( mOwner->getLuaState() );
				} // "luastate"
				return 0;
			}
		}; // LuaVMPropertyInterface

		LuaVMPropertyInterface mPropInterface;
	public:
		virtual PropertyInterface* getPropertyInterface()
		{
			return &mPropInterface;
		}*/
	};

	class LuaScript : public scripting::IScript
	{
	public:
		LuaScript( ScriptingSystem* pCreator );
		virtual ~LuaScript();
		/** Get a pointer to the scripting system that
			created this IScript object.
		*/
		virtual const ScriptingSystem* getCreator() const;

		virtual bool isLoaded();
		virtual void setData( const base::String& rData );

		const base::String& getData() const;

	private:
		base::String			mFileName;
		const ScriptingSystem*	mCreator;
	};

} // scripting
} // yake
#endif
