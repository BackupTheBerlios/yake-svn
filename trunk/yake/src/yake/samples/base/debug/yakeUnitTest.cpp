//****************************************************************************
//**
//**    UNITTEST.CPP
//**    Unit test
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// STL
#include <iostream>
// Yake
#include <yake/base/yake.h>
#include <yake/base/yakeDebugOutputStream.h>
#include <yake/base/yakeLog.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base;

namespace yake
{
namespace testsuite
{
	
class Dilemma
{
YAKE_DECLARE_CLASS( yake::testsuite::Dilemma )
// Class
public:
	// Destructor.
	~Dilemma() {}

	// Constructor.
	Dilemma()
	{
		YAKE_DECLARE_FUNCTION( Dilemma )
		throw Exception( "Oh my god!", YAKE_HERE );
	}
};

} // testsuite
} // yake

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
YAKE_DECLARE_GLOBAL
int main()
{
	YAKE_DECLARE_FUNCTION( main )

	using namespace yake;
	using namespace yake::testsuite;

	// Catch exception
	try
	{
		Dilemma myDilemma;
	}
	catch( const Exception & ex )
	{
		YAKE_LOG_ERROR( "Yet another dilemma!" )
		DebugOutputStream::instance() << ex.what();
	}

	// Print some stuff
	YAKE_LOG_INFORMATION( native::getApplicationDir() )
	YAKE_LOG_WARNING( "Alert!" )
	DebugOutputStream::instance() << "hello, I am a your friendly debug stream!";

	// Smart assert
	YAKE_ASSERT( false == true ).debug( "Smart assert is working, obviously!" );

	// Wait for a keystroke (in debug builds only).
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}