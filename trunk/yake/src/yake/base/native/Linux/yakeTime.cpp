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

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================

// Yake
#include <yake/base/yakePCH.h>
#include <yake/base/native/yakeNative.h>
#include <yake/base/yakeTimer.h>

extern "C"
{

#	include <sys/time.h>

}


//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================

namespace yake
{
namespace native
{

	static class TimerInit
	{
	public:
		struct timeval mStart;
	
		// Constructor.
		TimerInit()
		{
			gettimeofday( &mStart, 0 );
		}
 
	} TimerInit_g;
 
	float YAKE_BASE_NATIVE_API getTime()
	{
		struct timeval t_now;
		
		gettimeofday( &t_now, 0 );
		
		return static_cast< float >( 
			( t_now.tv_sec - TimerInit_g.mStart.tv_sec ) * 1000 + 
			( t_now.tv_usec - TimerInit_g.mStart.tv_usec ) / 1000 ) / 1000;
	}

} // native

namespace timer 
{	
	class LinuxTimer : public Timer
	{
	public:
		LinuxTimer::LinuxTimer()
		{
			reset();
		}
		
		virtual ~LinuxTimer() {}

	// Methods
	public:
		virtual void reset() 
		{
			gettimeofday( &mStart, NULL );
		}

		/** Return milliseconds since instantiation or last call to reset(). */
		virtual uint32 getMilliseconds()
		{
			struct timeval now;
			
			gettimeofday( &now, NULL );
			
			return ( now.tv_sec - mStart.tv_sec )*1000 + ( now.tv_usec - mStart.tv_usec )/1000;
		}

		/** Return seconds since instantiation or last call to reset(). */
		virtual real getSeconds()
		{
			return static_cast< real > ( getMilliseconds() ) / 1000;
		}
	
	protected:
		struct timeval mStart;
	};

	Timer* createTimer()
	{
		return new LinuxTimer();
	}
	
} // timer
} // yake

