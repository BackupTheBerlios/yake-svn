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
#ifndef YAKE_BASE_EXCEPTION_H
#define YAKE_BASE_EXCEPTION_H


#define YAKE_EXCEPT(message, source) throw( yake::Exception(message, source,__FILE__,__LINE__) )

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif
// Yake
#include <yake/base/mpl/yakeCast.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

class YAKE_BASE_API Exception : public std::exception
{
// Class
public:
  	Exception() throw();
	Exception( const Exception& rException ) throw();
	Exception( const String& rMessage ) throw();
	Exception( const String& rMessage, const String& rSource ) throw ();
	Exception( const String& rMessage, const String& rSource, const char* file, int line ) throw();
	virtual ~Exception() throw();

// Methods
public:
	Exception& operator=( const Exception& rException ) throw();

	virtual String getMessage() const throw();
	virtual String getSource() const throw();
	virtual String getFile() const throw();
	virtual int getLine() const throw();

	const char* what() const throw();

// Data
private:
	mutable String mAsciiWhat;
	String mMessage;
	String mSource;
	String mFile;
	int    mLine;
};

} // yake

#endif // YAKE_BASE_EXCEPTION_H

