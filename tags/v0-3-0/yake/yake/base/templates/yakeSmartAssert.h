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
#ifndef YAKE_BASESMARTASSERT_H
#define YAKE_BASESMARTASSERT_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif
// Yake
#include <yake/base/yakeString.h>
#include <yake/base/templates/yakeFunctor.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake 
{
namespace base
{
namespace assert 
{

/** NOT USED. Has to be redone for proper assertion handler registration.
*/
YAKE_BASE_API bool initAssert();

/** NOT USED. Has to be redone for proper assertion handler registration.
*/
YAKE_BASE_API bool initAssertDefault();

} // assert

enum AssertionLevel
{
	AL_WARNING,
	AL_DEBUG,
	AL_ERROR,
	AL_FATAL
};

/** Assertion context. Holds information on an assertion.
	\see SmartAssert
*/
struct YAKE_BASE_API SmartAssertContext
{
private:
	String mFile;
	int32	mLine;
	String	mExpr;
	typedef HashMap< String, String, _StringHash > ValueMapT;
	ValueMapT mValueMap;
	std::vector< String >	mMsgs;
	AssertionLevel	mLevel;
public:
	SmartAssertContext( const char * strExpr );
	void setLevel( AssertionLevel lvl ) { mLevel = lvl; }
	AssertionLevel getLevel() const { return mLevel; }
	void addValuePair( const String & name, const String & value );
	void setFileContext( const String & name, int line );
	void addMessage( const String & msg );
	void toStream(std::ostream & out) const;

public:
	String getExpression() const;
	String getFile() const;
	int32 getLine() const;
	String getMessage() const;
};

/** Assertion handler.
*/
typedef templates::function1< void, const SmartAssertContext& > SMARTASSERT_HANDLER_FUN;

/** Smart assert struct/class.

	Acknowledgement: After following a different approach for a similar thing
	I went for an implementation based on this article:
	http://www.cuj.com/documents/s=8464/cujcexp0308alexandr/cujcexp0308alexandr.html
		
	Usage:

	The assertion macro can be used like this:

		general:
		\code YAKE_ASSERT( expr )(var1)(var2)...(varn).msg( msg1 ).msg( msg2 ).warning( "strange things happen, my friend!" )\endcode

		Example:
		\code YAKE_ASSERT( strName=="psyc" )(strName).fatal("you have to be 'psyc'.");\endcode

	If strName is not "psyc" this will trigger an assertion which outputs the
	contents of the variable strName and calls the assertion handler for fatal
	level assertions.

	It is possible to output more than one variable:

		\code YAKE_ASSERT( a==b )(a)(b); // this defaults to 'debug' level, btw.\endcode

	You can add any number of messages, like this:

		\code YAKE_ASSERT( a==b )(a)(b).msg("hola").msg("compadres");\endcode

	The following assertion levels exist:
		
		warning -> warning()
		debug -> debug()
		error -> error()
		fatal -> fatal()
*/
struct YAKE_BASE_API SmartAssert
{
private:
	SmartAssertContext	mCtx;

	typedef std::map< AssertionLevel, SMARTASSERT_HANDLER_FUN > HandlerMap;
	static HandlerMap mHandlers;
public:
	SmartAssert( const char * strExpr ) : YAKE_ASSERT_A(*this), YAKE_ASSERT_B(*this), mCtx(strExpr) {}
	~SmartAssert();

	SmartAssert& YAKE_ASSERT_A;
	SmartAssert& YAKE_ASSERT_B;

	template< class type >
	SmartAssert & printValue( const type & value, const char * msg )
	{
		std::ostringstream out;
		out << value;
		mCtx.addValuePair( msg, out.str() );
		return *this;
	}

	SmartAssert& setFileContext( const String & name, int line )
	{
		mCtx.setFileContext( name, line );
		return *this;
	}

	void warning( const String & msg );
	void debug( const String & msg );
	void error( const String & msg );
	void fatal( const String & msg );
	SmartAssert& msg( const String & msg );
private:
	SMARTASSERT_HANDLER_FUN getHandler();
};


namespace detail
{
	/** Use to create a temporarily used Assert object. */
	inline SmartAssert makeAssert( const char * strExpr )
	{
		return SmartAssert( strExpr );
	}
} // detail

#define YAKE_ASSERT_A(x) YAKE_ASSERT_OP(x, B)
#define YAKE_ASSERT_B(x) YAKE_ASSERT_OP(x, A)

#define YAKE_ASSERT_OP(x, next) \
	YAKE_ASSERT_A.printValue((x), #x).YAKE_ASSERT_ ## next \

#ifdef YAKE_DEBUG

#define YAKE_ASSERT( expr ) \
	if ( (expr) ) ; \
	else \
		yake::base::detail::makeAssert( #expr ).setFileContext( __FILE__, __LINE__ ).YAKE_ASSERT_A

#else

#define YAKE_ASSERT( expr) \
	if ( (true) ) ; \
	else \
		yake::base::detail::makeAssert( #expr ).setFileContext( __FILE__, __LINE__ ).YAKE_ASSERT_A

#endif

} // base
} // yake

#endif // YAKE_BASESMARTASSERT_H
