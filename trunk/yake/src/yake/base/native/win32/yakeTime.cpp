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

namespace Win32
{
#	define STRICT
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <mmsystem.h>
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
	// Public member for a gain of speed.
	Win32::LARGE_INTEGER mTimerFrequency;
	real mStart;

	// Constructor.
	TimerInit()
	{
		if( !Win32::QueryPerformanceFrequency( &mTimerFrequency ) )
		{
			mTimerFrequency.QuadPart = 0;
			mStart = real( Win32::timeGetTime() ) / 1000.0f;
		}
		else
		{
			Win32::LARGE_INTEGER largeCount;
			Win32::QueryPerformanceCounter( &largeCount );

			double count = double( largeCount.QuadPart );
			double frequency = double( mTimerFrequency.QuadPart );

			mStart = real( count / frequency );
		}
	}
} TimerInit_g;


real getTime()
{
	// No performance timer available.
	if( TimerInit_g.mTimerFrequency.QuadPart == 0 )
		return real( Win32::timeGetTime() ) / 1000.0f - TimerInit_g.mStart;

	Win32::LARGE_INTEGER largeCount;
	Win32::QueryPerformanceCounter( &largeCount );

	double count = double( largeCount.QuadPart );
	double frequency = double( TimerInit_g.mTimerFrequency.QuadPart );

	return real( count / frequency ) - TimerInit_g.mStart;
}


} // native
} // yake
