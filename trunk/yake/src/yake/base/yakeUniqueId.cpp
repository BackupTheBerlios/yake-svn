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
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/base/yakePCH.h>
#include <yake/base/yakeUniqueId.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

	//-----------------------------------------------------
	GUID GUID::kNull = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

	//-----------------------------------------------------
	bool operator == (const GUID & lhs, const GUID & rhs)
	{
		return memcmp( &lhs, &rhs, sizeof(GUID) ) == 0;
	}

	//-----------------------------------------------------
	bool operator != (const GUID & lhs, const GUID & rhs)
	{
		return memcmp( &lhs, &rhs, sizeof(GUID) ) != 0;
	}

	//-----------------------------------------------------
	String GUID::toString() const
	{
		char buf[36+4+1]; // 36 chars (hexadecimal display of GUID ) + 4 '-' characters + 1 '\0'
		sprintf( buf, "%8X-%4X-%4X-%2X%2X-%2X%2X%2X%2X%2X%2X",
					a, b, c, d1, d2, d3, d4, d5, d6, d7, d8 );
		return std::string(buf);
	}

	//-----------------------------------------------------
	void GUID::fromString( const String & id )
	{
		YAKE_ASSERT( 1==0 ).fatal("NOT IMPLEMENTED! That reason enough ?");
		std::vector< String > elems = id.split( "-" );
		if (elems.size() < 5)
			return; //FIXME: USE YAKE_ASSERT

		//FIXME:
	}

} // base
} // yake
