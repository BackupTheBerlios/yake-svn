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
#ifndef CONCRETE_H
#define CONCRETE_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/registryPluginInterface/interface.h>

#if defined( YAKE_REGISTRYPLUGINCONCRETE_EXPORTS )
#	define YAKE_REGISTRY_PLUGIN_CONCRETE_API DLLEXPORT
#else
#	define YAKE_REGISTRY_PLUGIN_CONCRETE_API DLLIMPORT
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================

class YAKE_REGISTRY_PLUGIN_CONCRETE_API NetworkDriver : public INetworkDriver
{		
YAKE_DECLARE_CLASS( NetworkDriver )
// Class
public:
	// Destructor.
	virtual ~NetworkDriver();

// Methods
public:
	// Resolve host.
	const yake::base::String sayHelloMyFriend() const;

YAKE_DECLARE_CONCRETE( NetworkDriver, "winsock" )
};

#endif // CONCRETE_H