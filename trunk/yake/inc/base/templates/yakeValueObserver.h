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
#ifndef YAKE_BASE_TEMPLATES_VALUEOBSERVER_H
#define YAKE_BASE_TEMPLATES_VALUEOBSERVER_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#	include "yakePrerequisites.h"
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace templates
{

/** EXPERIMENTAL
*/
template < typename T >
class ValueObserver
{
protected:
	T& Ref;

	typedef Signal0<void> TriggerSignal;
	TriggerSignal				mTriggerSignal;
public:
	ValueObserver( T& refVar ) : mRef( refVar )
	{}
	virtual ~ValueObserver() {}

	virtual void check() = 0;
};

/** EXPERIMENTAL
*/
template < typename T >
class EqualsValueObserver
{
protected:
	T	mVal;
public:
	EqualsValueObserver( T & refVar, const T & val ) : ValueObserver( refVar ), mVal( val )

	virtual void check()
	{
		if ( mVal == mRef )
			mTriggerSignal();
		}
	};

} // templates
} // base
} // yake

#endif // YAKE_BASE_TEMPLATES_VALUEOBSERVER_H
