/*
   ------------------------------------------------------------------------------------
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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/base/yakePCH.h>
#include <yake/base/yakeLog.h>
#include <yake/base/yakeDebugLogAppender.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

Log::Log()
{
#ifdef YAKE_DEBUG_BUILD
	mOnLog.connect( &DebugLogAppender::onLog );
#endif
}

Log::~Log()
{
}

void Log::onLog( const OnLog::slot_type& rSlot )
{
  mOnLog.connect( rSlot );
}

void Log::log( const String& rWhat,
					Severity severity,
					const String& rSource,
					const String& rAdditionalInfo )
{
	mOnLog( Message( rWhat, severity, rSource, rAdditionalInfo ) );
}

void log_information( const String& what, const String& source, const String& where )
{
	Log::instance().log( what, Log::INFORMATIONS, source, where );
}

void log_warning( const String& what, const String& source, const String& where )
{
	Log::instance().log( what, Log::WARNINGS, source, where );
}

void log_error( const String& what, const String& source, const String& where )
{
	Log::instance().log( what, Log::ERRORS, source, where );
}

void log( const String& what )
{
	Log::instance().log( what );
}

} // base
} // yake
