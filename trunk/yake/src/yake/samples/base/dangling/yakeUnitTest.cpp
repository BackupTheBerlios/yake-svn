//****************************************************************************
//**
//**    UNITTEST.CPP
//**    Unit test
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Yake
#include <yake/base/yake.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base;

struct A {};
void foo( int i ) { DebugOutputStream::instance() << "int - failure"; }
void foo( A* p )  { DebugOutputStream::instance() << "ptr - ok"; }

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
int main()
{
	A* a = DANGLING;
	foo( DANGLING );

// Wait for a keystroke (in debug builds only).
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}