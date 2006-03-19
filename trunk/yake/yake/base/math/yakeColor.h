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
#include "yake/base/math/yakeMath.h"

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace math {

class YAKE_BASE_API Color
{
public:
	// Constructor.
	Color();
	Color( real fRed, real fGreen, real fBlue, real fAlpha = 1.0f );
	Color( real Coordinates[ 4 ] );
	Color( const Color& rIn );

	// Assignment Operator.
	Color& operator=( const Color& rIn );

	// Comparation Operators.
	bool operator==( const Color& rIn ) const;
	bool operator!=( const Color& rIn ) const;

	// Access Operators.
	real& operator()( uint8 bIndex );
	const real& operator()( uint8 bIndex ) const;
	operator real*();
	operator const real*() const;

	// Addition.
	Color operator+( const Color& rIn ) const;
	Color& operator+=( const Color& rIn );

	// Subtraction.
	Color operator-( const Color& rIn ) const;
	Color& operator-=( const Color& rIn );

	// Scalar Multiplication.
	Color operator*( real fScalar ) const;
	Color& operator*=( real fScalar );

	// Scalar Division.
	Color operator/( real fScalar ) const;
	Color& operator/=( real fScalar );

	// Negation.
	Color operator-() const;
	void negate();

	// Dot Product.
	real operator*( const Color& rIn ) const;

	// Normalization.
	void normalize( real fTolerance = 1E-06 );
	Color getNormalized( real fTolerance = 1E-06 ) const;

	// Miscellaneous Operations.
	real getLength() const;
	real getSquaredLength() const;

	// The Serialization Function.
	base::IOutputStream& operator << (base::IOutputStream& rStream) const;

	// The Data.
	union
	{
		// Either access by short name.
		struct
		{
			real r;
			real g;
			real b;
			real a;
		};

		// Or access by long name.
		struct
		{
			real m_fRed;
			real m_fGreen;
			real m_fBlue;
			real m_fAlpha;
		};

		// Or access like a vector.
		real V[ 4 ];
	};

	// Predefined Vectors.
	static const Color BLACK;
	static const Color WHITE;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
};

// Scalar Multiplication.
Color operator*( real fScalar, const Color& rColor );

} // math
} // yake

// Fast Inline Implementation.
#include <yake/base/math/yakeColor.inl>

#endif // YAKE_BASE_MATH_COLOR_H

