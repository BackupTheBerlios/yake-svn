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
#ifndef YAKE_BASE_STRING_H
#define YAKE_BASE_STRING_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

//typedef std::string YAKE_BASE_STRING;

/** The YAKE string class. It's derived from the STL string and enhanced by a few useful functions.
	NB If you need to store Unicode use the UString class.
	It knows how to convert between Unicode formats and internally holds a UTF-8 string.
	\todo UTF-8 code removed! Has to be redone. Current string class is based on Ogre's ( www.ogre3d.org ).
*/
class YAKE_BASE_API String : public YAKE_BASE_STRING
{
public:
	String() : YAKE_BASE_STRING() {}
	String( const String& rhs ) : YAKE_BASE_STRING( static_cast< const YAKE_BASE_STRING& >(rhs) ) {}
	String( const YAKE_BASE_STRING& rhs ) : YAKE_BASE_STRING( rhs ) {}
	String( const char * rhs ) : YAKE_BASE_STRING( rhs ) {}

	void trim(bool left = true, bool right = true);
	std::vector< String > split( const String& delims = "\t\n", unsigned int maxSplits = 0) const;
	String toLowerCase( void );
	String toUpperCase( void );
	real toReal( void );


	size_t operator ()( const YAKE_BASE_STRING& rStringBase ) const
	{
		register size_t ret = 0;
		for( YAKE_BASE_STRING::const_iterator it = rStringBase.begin(); it != rStringBase.end(); ++it )
			ret = 5 * ret + *it;
		return ret;
	};

	char charAt( size_t pos ) const
	{
		return YAKE_BASE_STRING::operator [](pos);
	}

	template< typename T >
	String& operator << ( const T& t )
	{
		std::ostringstream out;
		out << t;
		(*this) += out.str();
		return *this;
	}
};


class StringConverter 
{
public:
	static real parseReal( const String& rString )
	{
		return (real) atof( rString.c_str() );
	}
};

#undef YAKE_BASE_STRING

} // base
} // yake

#endif // YAKE_BASE_STRING_H
