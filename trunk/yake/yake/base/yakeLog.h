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
#ifndef YAKE_BASE_LOG_H
#define YAKE_BASE_LOG_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include <yake/base/yakeString.h>
#include <yake/base/templates/yakeSingleton.h>
#include <yake/base/templates/yakeSignals.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

// todo: printf sucks <= no it doesn't! <= it does, use templates for type safety

// todo: replace macros with inline functions
#define YAKE_LOGPRINTF yake::base::Log::instance().logPrintf
#define YAKE_LOG_INFORMATION( what ) yake::base::Log::instance().log( what, yake::base::Log::INFORMATIONS, YAKE_HERE );
#define YAKE_LOG_WARNING( what ) yake::base::Log::instance().log( what, yake::base::Log::WARNINGS, YAKE_HERE );
#define YAKE_LOG_ERROR( what ) yake::base::Log::instance().log( what, yake::base::Log::ERRORS, YAKE_HERE );
#define YAKE_LOG( what ) yake::base::Log::instance().log( what, yake::base::Log::INFORMATIONS );

class YAKE_BASE_API Log
{
YAKE_DECLARE_CLASS( yake::base::Log )
// Types
public:
	enum Severity { INFORMATIONS, WARNINGS, ERRORS };
	typedef Signal3< void (const String&, Severity, const String&) > OnLog;

// Class
public:
	Log();
	virtual ~Log();

// Methods
public:
	void onLog( const OnLog::slot_type& rSlot );
	void log( const String& rWhat, Severity severity );
	void log( const String& rWhat, Severity severity, const String& rSource );
	void logPrintf(char *fmt, ...); // todo: remove this <= Er.. no!

// Data
private:
	OnLog mOnLog;

	YAKE_BUILD_PHOENIX_SINGLETON(Log)
};

inline void log_warning(std::string where, std::string what)
{ yake::base::Log::instance().log(what, yake::base::Log::WARNINGS, where); }

} // base
} // yake

#endif // YAKE_BASE_LOG_H
