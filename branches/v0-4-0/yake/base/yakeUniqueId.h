/*
   ------------------------------------------------------------------------------------
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
#ifndef YAKE_BASE_UNIQUEID_H
#define YAKE_BASE_UNIQUEID_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{

/** A unique id.
	\see ClassId
*/
struct GUID
{
	uint32		a;
	uint16		b;
	uint16		c;
	struct 
	{
		uint8	d1;
		uint8	d2;
		uint8	d3;
		uint8	d4;
		uint8	d5;
		uint8	d6;
		uint8	d7;
		uint8	d8;
	};

	static GUID kNull;

	/** Converts the GUID into a human-readable format.
	*/
	String toString() const;

	/** Gets data from string.
		\todo IMPLEMENT IT!
	*/
	void fromString( const String & id );
};

YAKE_BASE_API bool operator == (const GUID& lhs, const GUID& rhs);
YAKE_BASE_API bool operator != (const GUID& lhs, const GUID& rhs);

/** ClassId uniquely identifies the class of a given object at runtime.
	Moreover it can be used for persistent storage between sessions.

	\remarks Classes that should have the persistent storage and runtime class
				capabilities should use the following macros:\n\n
				\p YAKE_DECL_CLASSID( CLASS )
				\p YAKE_IMPL_CLASSID( CLASS )
				\n
				Then at some point, possibly during initialisation of the application,
				the classes have to be registered with the global singleton class manager:\n\n
				\n
				\code ClassManager::getSingleton().registerClass( CLASS::getClassId() );\endcode
				\n
				This is enough to be used for runtime class checks etc. If you want
				to add persistent storage capabilities, you have to inherit and override
				methods from PersistenObject.
	\see PersistentObject
*/
YAKE_BASE_API typedef GUID ClassId;

} // yake

#endif // YAKE_BASE_UNIQUEID_H
