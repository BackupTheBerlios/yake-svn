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
#include <yake/base/yakePCH.h>
#include <yake/base/yakeException.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

Exception::Exception() throw () : mLine(-1)
{
}

Exception::Exception( const Exception& rException ) throw() : mLine(-1)
{
  *this = rException;
}

Exception::Exception( const String& rMessage ) throw() 
: mMessage( rMessage ), mLine(-1)
{
}

Exception::Exception( const String& rMessage, const String& rSource ) throw()
: mMessage( rMessage ), mSource( rSource ), mLine(-1)
{
}

Exception::Exception( const String& rMessage, const String& rSource, const char* file, int line ) throw()
: mMessage( rMessage ), mSource( rSource ), mFile(file), mLine(line)
{
}

Exception::~Exception() throw()
{
}


Exception& Exception::operator=( const Exception& rException ) throw()
{
  mMessage = rException.mMessage;
  mSource = rException.mSource;
  mFile = rException.mFile;
  mLine = rException.mLine;

  return *this;
}

String Exception::getMessage() const throw()
{
  return mMessage;
}

String Exception::getSource() const throw()
{
  return mSource;
}

String Exception::getFile() const throw()
{
  return mFile;
}

int Exception::getLine() const throw()
{
  return mLine;
}

const char* Exception::what() const throw()
{
	if (mAsciiWhat.length() == 0)
	{
		std::stringstream ss;
		ss << "YAKE EXCEPTION" << std::endl;
		if (mFile.length() > 0)
		{
			ss << "FILE: " << mFile.c_str() << std::endl;
			if (mLine >= 0)
				ss << "LINE: " << mLine << std::endl;
		}
		ss << "SOURCE: " << mSource.c_str() << std::endl;
		ss << "REASON: " << mMessage.c_str() << std::endl;
		mAsciiWhat = ss.str();
	}
	return mAsciiWhat.c_str();
}

} // yake
