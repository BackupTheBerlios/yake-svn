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
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SCRIPTINGLUA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SCRIPTINGLUA_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef YAKESCRIPTINGLUABINDINGS_EXPORTS
#define SCRIPTINGBINDINGSLUA_API DLLEXPORT
#else
#define SCRIPTINGBINDINGSLUA_API DLLIMPORT
#endif

SCRIPTINGBINDINGSLUA_API extern "C" yake::base::Plugin* dynlibStartPlugin();

using namespace yake::base;
using namespace yake::base::templates;

class LuaBindingsPlugin : public yake::scripting::ScriptingBindingsPlugin
{
public:
	LuaBindingsPlugin();
	virtual ~LuaBindingsPlugin();

	virtual String getName() const;
	//virtual yake::version getVersion() const;
	virtual bool initialise();
	virtual bool shutdown();

	virtual SharedPtr<yake::scripting::IBinder> createBinder();
};
