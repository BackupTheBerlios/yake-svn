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
#ifndef YAKE_BASE_MATH_COLOR_H
#define YAKE_BASE_MATH_COLOR_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include "yakeMath.h"

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace math
{

class YAKE_BASE_API Color : public yake::base::ISerializable
{
public:
	// Constructor.
	Color();
	Color( float fRed, float fGreen, float fBlue, float fAlpha = 1.0f );
	Color( float Coordinates[ 4 ] );
	Color( const Color& rIn );

	// Assignment Operator.
	Color& operator=( const Color& rIn );

	// Comparation Operators.
	bool operator==( const Color& rIn ) const;
	bool operator!=( const Color& rIn ) const;

	// Access Operators.
	float& operator()( uint8 bIndex );
	const float& operator()( uint8 bIndex ) const;
	operator float*();
	operator const float*() const;

	// Addition.
	Color operator+( const Color& rIn ) const;
	Color& operator+=( const Color& rIn );

	// Subtraction.
	Color operator-( const Color& rIn ) const;
	Color& operator-=( const Color& rIn );

	// Scalar Multiplication.
	Color operator*( float fScalar ) const;
	Color& operator*=( float fScalar );

	// Scalar Division.
	Color operator/( float fScalar ) const;
	Color& operator/=( float fScalar );

	// Negation.
	Color operator-() const;
	void negate();

	// Dot Product.
	float operator*( const Color& rIn ) const;

	// Normalization.
	void normalize( float fTolerance = 1E-06 );
	Color getNormalized( float fTolerance = 1E-06 ) const;

	// Miscellaneous Operations.
	float getLength() const;
	float getSquaredLength() const;

	// The Serialization Function.
	void serializeOut( IOutputStream& rStream ) const;

	// The Data.
	union
	{
		// Either access by short name.
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};

		// Or access by long name.
		struct
		{
			float m_fRed;
			float m_fGreen;
			float m_fBlue;
			float m_fAlpha;
		};

		// Or access like a vector.
		float V[ 4 ];
	};

	// Predefined Vectors.
	static const Color BLACK;
	static const Color WHITE;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
};

// Scalar Multiplication.
Color operator*( float fScalar, const Color& rColor );

} // math
} // base
} // yake

// Fast Inline Implementation.
#include <yake/base/math/yakeColor.Inl>

#endif // YAKE_BASE_MATH_COLOR_H
