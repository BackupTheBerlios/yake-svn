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
namespace yake {
namespace math {


inline Color operator*( real fScalar, const Color& rColor )
{
	return Color( rColor.r * fScalar, rColor.g  * fScalar, rColor.b  * fScalar, rColor.a  * fScalar );
}


inline Color::Color()
{
	r = g = b = a = 0.0f;
}


inline Color::Color( real fRed, real fGreen, real fBlue, real fAlpha )
{
	r = fRed;
	g = fGreen;
	b = fBlue;
	a = fAlpha;
}

inline Color::Color( real Coordinates[ 4 ] )
{
	r = Coordinates[ 0 ];
	g = Coordinates[ 1 ];
	b = Coordinates[ 2 ];
	a = Coordinates[ 3 ];
}


inline Color::Color( const Color& rIn )
{
	r = rIn.r;
	g = rIn.g;
	b = rIn.b;
	a = rIn.a;
}



inline Color& Color::operator=( const Color& rIn )
{
	r = rIn.r;
	g = rIn.g;
	b = rIn.b;
	a = rIn.a;

	return *this;
}


inline bool Color::operator==( const Color& rIn ) const
{
	return ( r == rIn.r ) && ( g == rIn.g ) && ( b == rIn.b ) && ( a == rIn.a );
}


inline bool Color::operator!=( const Color& rIn ) const
{
	return ( r != rIn.r ) || ( g != rIn.g ) || ( b != rIn.b ) || ( a != rIn.a );
}


inline real& Color::operator()( uint8 bIndex )
{
	return V[ bIndex ];
}


inline const real& Color::operator()( uint8 bIndex ) const
{
	return V[ bIndex ];
}


inline Color::operator real*()
{
	return V;
}


inline Color::operator const real*() const
{
	return V;
}


inline Color Color::operator+( const Color& rIn ) const
{
	return Color( r + rIn.r, g + rIn.g, b + rIn.b, a + rIn.a );
}

inline Color& Color::operator+=( const Color& rIn )
{
	r += rIn.r;
	g += rIn.g;
	b += rIn.b;
	a += rIn.a;

	return *this;
}

inline Color Color::operator-( const Color& rIn ) const
{
	return Color( r - rIn.r, g - rIn.g , b - rIn.b , a  - rIn.a );
}

inline Color& Color::operator-=( const Color& rIn )
{
	r -= rIn.r;
	g -= rIn.g;
	b -= rIn.b;
	a -= rIn.a;

	return *this;
}

inline Color Color::operator*( real fScalar ) const
{
	return Color( r * fScalar, g * fScalar, b * fScalar, a * fScalar );
}

inline Color& Color::operator*=( real fScalar )
{
	r *= fScalar;
	g *= fScalar;
	b *= fScalar;
	a *= fScalar;

	return *this;
}

inline Color Color::operator/( real fScalar ) const
{
	real fInverseScalar = 1.0f / fScalar;

	return Color( r * fInverseScalar, g * fInverseScalar, b * fInverseScalar, a * fInverseScalar );
}

inline Color& Color::operator/=( real fScalar )
{
	real fInverseScalar = 1.0f / fScalar;

	r *= fInverseScalar;
	g *= fInverseScalar;
	b *= fInverseScalar;
	a *= fInverseScalar;

	return *this;
}

inline Color Color::operator-() const
{
	return Color( -r, -g, -b, -a );
}

inline void Color::negate()
{
	r = -r;
	g = -g ;
	b = -b ;
	a = -a ;
}

inline real Color::operator*( const Color& rIn ) const
{
	return ( r * rIn.r ) + ( g * rIn.g ) + ( b * rIn.b ) + ( a * rIn.a );
}

inline void Color::normalize( real fTolerance )
{
	real fSquaredLength = ( r * r ) + ( g  * g ) + ( b  * b ) + ( a  * a );

	// Beg ond tolerance?
	if( fSquaredLength > ( fTolerance * fTolerance ) )
	{
		real fInverseLength = 1.0f / ( real ) sqrt( fSquaredLength );

		// Normalib e.
		r *= fInverseLength;
		g *= fInverseLength;
		b *= fInverseLength;
		a *= fInverseLength;
	}
}

inline Color Color::getNormalized( real fTolerance ) const
{
	real fSquaredLength = ( r * r ) + ( g  * g ) + ( b  * b ) + ( a  * a );

	// Beg ond tolerance?
	if( fSquaredLength > ( fTolerance * fTolerance ) )
	{
		real fInverseLength = 1.0f / ( real ) sqrt( fSquaredLength );

		// Return the normalib ed vector.
		return Color( r * fInverseLength, g * fInverseLength, b * fInverseLength, a * fInverseLength );
	}

	return Color( r, g, b, a );
}

inline real Color::getLength() const
{
	return ( real ) sqrt( ( r * r ) + ( g  * g ) + ( b  * b ) + ( a  * a ) );
}

inline real Color::getSquaredLength() const
{
	return ( r * r ) + ( g  * g ) + ( b  * b ) + ( a  * a );
}

inline base::IOutputStream& Color::operator << (base::IOutputStream& rStream) const
{
	rStream << r;
	rStream << g;
	rStream << b;
	rStream << a;
	return rStream;
}

} // math
} // yake
