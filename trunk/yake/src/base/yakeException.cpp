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
#include <inc/base/yakePCH.h>
#include <inc/base/yakeException.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

Exception::Exception()
{
}

Exception::Exception( const Exception& rException )
{
  *this = rException;
}

Exception::Exception( const String& rMessage )
: mMessage( rMessage )
{
}

Exception::Exception( const String& rMessage, const String& rSource )
: mMessage( rMessage ), mSource( rSource )
{
}

Exception::~Exception()
{
}


Exception& Exception::operator=( const Exception& rException )
{
  mMessage = rException.mMessage;
  mSource = rException.mSource;

  return *this;
}


String Exception::getMessage() const
{
  return mMessage;
}

String Exception::getSource() const
{
  return mSource;
}


const char* Exception::what() const throw()
{
  return mAsciiWhat.c_str();
}


} // base
} // yake