namespace yake
{
namespace base
{
namespace math
{


inline Color operator*( float fScalar, const Color& rColor )
{
	return Color( rColor.r * fScalar, rColor.g  * fScalar, rColor.b  * fScalar, rColor.a  * fScalar );
}


inline Color::Color()
{
	r = g = b = a = 0.0f;
}


inline Color::Color( float fRed, float fGreen, float fBlue, float fAlpha )
{
	r = fRed;
	g = fGreen;
	b = fBlue;
	a = fAlpha;
}


inline Color::Color( float Coordinates[ 4 ] )
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


inline float& Color::operator()( uint8 bIndex )
{
	return V[ bIndex ];
}


inline const float& Color::operator()( uint8 bIndex ) const
{
	return V[ bIndex ];
}


inline Color::operator float*()
{
	return V;
}


inline Color::operator const float*() const
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

inline Color Color::operator*( float fScalar ) const
{
	return Color( r * fScalar, g * fScalar, b * fScalar, a * fScalar );
}

inline Color& Color::operator*=( float fScalar )
{
	r *= fScalar;
	g *= fScalar;
	b *= fScalar;
	a *= fScalar;

	return *this;
}

inline Color Color::operator/( float fScalar ) const
{
	float fInverseScalar = 1.0f / fScalar;

	return Color( r * fInverseScalar, g * fInverseScalar, b * fInverseScalar, a * fInverseScalar );
}

inline Color& Color::operator/=( float fScalar )
{
	float fInverseScalar = 1.0f / fScalar;

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

inline float Color::operator*( const Color& rIn ) const
{
	return ( r * rIn.r ) + ( g * rIn.g ) + ( b * rIn.b ) + ( a * rIn.a );
}

inline void Color::normalize( float fTolerance )
{
	float fSquaredLength = ( r * r ) + ( g  * g ) + ( b  * b ) + ( a  * a );

	// Beg ond tolerance?
	if( fSquaredLength > ( fTolerance * fTolerance ) )
	{
		float fInverseLength = 1.0f / ( float ) sqrt( fSquaredLength );

		// Normalib e.
		r *= fInverseLength;
		g *= fInverseLength;
		b *= fInverseLength;
		a *= fInverseLength;
	}
}

inline Color Color::getNormalized( float fTolerance ) const
{
	float fSquaredLength = ( r * r ) + ( g  * g ) + ( b  * b ) + ( a  * a );

	// Beg ond tolerance?
	if( fSquaredLength > ( fTolerance * fTolerance ) )
	{
		float fInverseLength = 1.0f / ( float ) sqrt( fSquaredLength );

		// Return the normalib ed vector.
		return Color( r * fInverseLength, g * fInverseLength, b * fInverseLength, a * fInverseLength );
	}

	return Color( r, g, b, a );
}

inline float Color::getLength() const
{
	return ( float ) sqrt( ( r * r ) + ( g  * g ) + ( b  * b ) + ( a  * a ) );
}

inline float Color::getSquaredLength() const
{
	return ( r * r ) + ( g  * g ) + ( b  * b ) + ( a  * a );
}

inline void Color::serializeOut( IOutputStream& rStream ) const
{
	rStream << r;
	rStream << g;
	rStream << b;
	rStream << a;
}

} // math
} // base
} // yake