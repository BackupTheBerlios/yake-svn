/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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
#include <yake/base/native/win32/yakeConfig.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace native
{

YAKE_BASE_NATIVE_API CriticalSectionHandle criticalSection_Create()
{
  CRITICAL_SECTION* criticalSectionImpl = new CRITICAL_SECTION;
  InitializeCriticalSection( criticalSectionImpl );

  return ( CriticalSectionHandle )criticalSectionImpl;
}

YAKE_BASE_NATIVE_API void criticalSection_Enter( CriticalSectionHandle criticalSection )
{
  CRITICAL_SECTION* criticalSectionImpl = ( CRITICAL_SECTION* )criticalSection;
  EnterCriticalSection( criticalSectionImpl );
}

YAKE_BASE_NATIVE_API void criticalSection_Leave( CriticalSectionHandle criticalSection )
{
  CRITICAL_SECTION* criticalSectionImpl = ( CRITICAL_SECTION* )criticalSection;
  LeaveCriticalSection( criticalSectionImpl );
}

YAKE_BASE_NATIVE_API void criticalSection_Delete( CriticalSectionHandle criticalSection )
{
  CRITICAL_SECTION* criticalSectionImpl = ( CRITICAL_SECTION* )criticalSection;
  DeleteCriticalSection( criticalSectionImpl );
  delete criticalSectionImpl;
}

} // native
} // base
} // yake