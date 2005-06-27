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
#include <yake/base/templates/yakeSmartAssert.h>
#include <yake/base/native/yakeNative.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake 
{
namespace base
{
namespace assert 
{

		bool initAssert()
		{
			return true;
		}

} // assert

	//-----------------------------------------------------
	SmartAssertContext::SmartAssertContext( const char * strExpr ) : mExpr(strExpr), mLevel(AL_DEBUG)
	{
	}

	//-----------------------------------------------------
	void SmartAssertContext::addValuePair( const String & name, const String & value )
	{
		if ( name.length() == 0 ) return;

		if ( value.length() > 0)
			mValueMap[ name ] = value;
		else
			mValueMap[ name ] = "null [or couldn't be streamed]";
	}

	//-----------------------------------------------------
	void SmartAssertContext::setFileContext( const String & name, int line )
	{
		mFile = name;
		mLine = line;
	}

	//-----------------------------------------------------
	void SmartAssertContext::addMessage( const String & msg )
	{
		mMsgs.push_back( msg );
	}

	//-----------------------------------------------------
	String SmartAssertContext::getExpression() const
	{
		return mExpr;
	}

	//-----------------------------------------------------
	String SmartAssertContext::getFile() const
	{
    return mFile;
	}

	//-----------------------------------------------------
	int32 SmartAssertContext::getLine() const
	{
    return mLine;
	}

	//-----------------------------------------------------
	String SmartAssertContext::getMessage() const
	{
		String sTemp;
		for ( std::vector< String >::const_iterator it = mMsgs.begin();
				it != mMsgs.end();
				++it )
		{
			sTemp += *it;
		}
    return sTemp;
	}

	//-----------------------------------------------------
	void SmartAssertContext::toStream( std::ostream & out ) const
	{
		out << "---------------------------------------------------------\n";
		if (mLevel == AL_WARNING)
			out << "WARNING";
		else if (mLevel == AL_DEBUG)
			out << "DEBUG";
		else if (mLevel == AL_ERROR)
			out << "ERROR";
		else if (mLevel == AL_FATAL)
			out << "FATAL";
		out << "LEVEL ASSERTION FAILED \n";

		out << "Expression: " << mExpr << "\n";
		out << "File: " << mFile << " (" << mLine << ")\n";

		// output messages
		for ( std::vector< String >::const_iterator it = mMsgs.begin();
				it != mMsgs.end();
				++it )
		{
			out << *it << "\n";
		}

		// output values
		for ( HashMap< String, String, _StringHash >::const_iterator it = mValueMap.begin();
				it != mValueMap.end();
				++it )
		{
			out << "\t" << it->first << ": " << it->second << "\n";
		}
		out << "---------------------------------------------------------\n";
	}

	//-----------------------------------------------------
	void defaultHandler( const SmartAssertContext& ctx )
	{
		// print all messages to "cout"
		std::ostringstream out;
		ctx.toStream( out );
		std::cout << std::endl << out.str();

		bool IgnoreAlways = false;

		if (ctx.getLevel() != AL_WARNING) // "warning" assertions are logged only.
		{
			if( !native::debug_AssertFailed(
				ctx.getMessage().c_str(),
				ctx.getExpression().c_str(), 
				ctx.getFile().c_str(),
				ctx.getLine(), 
				IgnoreAlways ) 
				&& !IgnoreAlways )
			{
				abort();
			}
		}

		if (ctx.getLevel() == AL_ERROR || ctx.getLevel() == AL_FATAL)
			abort();
	}

	//-----------------------------------------------------
	SmartAssert::HandlerMap SmartAssert::mHandlers;

	//-----------------------------------------------------
	SMARTASSERT_HANDLER_FUN SmartAssert::getHandler()
	{
		if (mHandlers.size() < 4)
		{
			// set defaults
			mHandlers[ AL_WARNING ] = defaultHandler;
			mHandlers[ AL_DEBUG ] = defaultHandler;
			mHandlers[ AL_ERROR ] = defaultHandler;
			mHandlers[ AL_FATAL ] = defaultHandler;
		}
		return mHandlers[ mCtx.getLevel() ];
	}

	//-----------------------------------------------------
	SmartAssert::~SmartAssert()
	{
		if (getHandler())
			getHandler()( mCtx );
		else
		{
			std::ostringstream out;
			out << "\n[ COULD NOT GET AN ASSERTION HANDLER. WRITING TO STDOUT. ]\n";
			mCtx.toStream( out );

			std::cout << out;
		}
	}

	//-----------------------------------------------------
	void SmartAssert::warning( const String & msg )
	{
		mCtx.setLevel( AL_WARNING );
		mCtx.addMessage( "Warning: " + msg );
	}

	//-----------------------------------------------------
	void SmartAssert::debug( const String & msg )
	{
		mCtx.setLevel( AL_DEBUG );
		mCtx.addMessage( "Debug: " + msg );
	}

	//-----------------------------------------------------
	void SmartAssert::error( const String & msg )
	{
		mCtx.setLevel( AL_ERROR );
		mCtx.addMessage( "Error: " + msg );
	}

	//-----------------------------------------------------
	void SmartAssert::fatal( const String & msg )
	{
		mCtx.setLevel( AL_FATAL );
		mCtx.addMessage( "Fatal: " + msg );
	}

	//-----------------------------------------------------
	SmartAssert& SmartAssert::msg( const String & msg )
	{
		mCtx.addMessage( "Message: " + msg );
		return *this;
	}

} // base
} // yake
