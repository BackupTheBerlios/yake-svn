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
#ifndef INC_YAKE_SCRIPTINGSYSTEM_H
#define INC_YAKE_SCRIPTINGSYSTEM_H

//#include <yakeBase/yake/yakeStableHeaders.h>

namespace yake {
	namespace scripting {
		class IVM;
	}

	/** The scripting registration manager forwards requests to
		subscribed objects to register their script bindings.
		\remarks A scripting system can use any kind of scripting language.
			To detect whether the scripting VM language is compatible, check the language of its creator
			system and the description string and the version!
	*/
	class /*YAKE_BASE_API*/ ScriptingRegistrationManager
	{
	protected:
		typedef Signal1< void, scripting::IVM* > RegisterScriptBindingsSignal;
	public:
		void subscribeToRegisterScriptBindingsSignal( const RegisterScriptBindingsSignal::slot_type & slot );
		void registerScriptBindings( scripting::IVM* pVM );
	};

	namespace scripting {

		class ScriptingSystem;
		class IVM;

		class IBinder
		{
		public:
			virtual ~IBinder() {}

			virtual void bind( scripting::IVM* pVM ) = 0;
		};

		/**
		*/
		class /*YAKE_BASE_API*/ ScriptingPlugin : public ::yake::base::Plugin
		{
		public:
			virtual ScriptingSystem* createSystem() = 0;
		};

		/**
		*/
		class /*YAKE_BASE_API*/ ScriptingBindingsPlugin : public ::yake::base::Plugin
		{
		public:
			virtual yake::base::templates::SharedPtr<IBinder> createBinder() = 0;
		};

		/** A script that can be executed by a virtual machine.
			\see IVM
			\see ScriptingSystem
		*/
		class /*YAKE_BASE_API*/ IScript
		{
		public:
			/** Get a pointer to the scripting system that
				created this IScript object.
			*/
			virtual const ScriptingSystem* getCreator() const = 0;

			virtual bool isLoaded() = 0;
			virtual void setData( const base::String & data ) = 0;
		};

		/** Scripting virtual machine.
			A virtual machine can execute scripting code.
			\see IScript
			\see ScriptingSystem
		*/
		class /*YAKE_BASE_API*/ IVM
		{
		public:
			virtual ~IVM() {}

			/** Get a pointer to the scripting system that
				created this IVM object.
			*/
			virtual const ScriptingSystem* getCreator() const = 0;

			/** Execute the script data contained in pScript.
				\param pScript script object
			*/
			virtual void execute( IScript* pScript ) = 0;

			/** Execute the data contained in the string.
				\param data a string containing scripting data to execute.
			*/
			virtual void execute( const base::String & data ) = 0;

			//virtual PropertyInterface* getPropertyInterface()
			//{ return 0; }
		};

		/** Scripting system interface.
			Provides means to create virtual machines and scripts.
			\see IVM
			\see IScript
		*/
		class /*YAKE_BASE_API*/ ScriptingSystem// : public System
		{
		public:
			//ScriptingSystem( SystemType type ) : System( type ) {}
			enum Language
			{
				L_LUA,
				L_PYTHON,
				L_PERL,
				L_RUBY,
				L_CPP
			};
			/** Get the language used by the scripting system.
			*/
			virtual Language getLanguage() const = 0;

			/** Get the version of the scripting system. */
			//virtual version getVersion() const = 0;

			/** Get a unique name for the system (e.g. "yake.core.lua", "yake.graphicsonly.python").
			*/
			virtual base::String getName() const = 0;

			/** Create a scipting virtual machine.
				\see IScript
			*/
			virtual IVM* createVM() = 0;
			
			/** Create a script resource from a file. It can be executed
				by a virtual machine.
				\see IVM
			*/
			virtual IScript* createScriptFromFile( const base::String & file ) = 0;
		};

	}
}


#endif
