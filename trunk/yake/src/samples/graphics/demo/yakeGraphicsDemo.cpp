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
#include <inc/graphics/yakeGraphicsSystem.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base::templates;
using namespace yake::graphics;

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================

YAKE_DECLARE_GLOBAL
int main()
{
	YAKE_DECLARE_FUNCTION( main )

	YAKE_USE_LIB( "graphicsOgre.dll" );	
	Pointer< IGraphicsSystem > pGraphicsSystem = create< IGraphicsSystem >();
	Pointer< IGraphicalWorld > pWorld( pGraphicsSystem->create< IGraphicalWorld >() );

#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}