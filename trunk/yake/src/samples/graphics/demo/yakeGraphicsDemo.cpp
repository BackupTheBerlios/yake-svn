//****************************************************************************
//**
//**    Sample
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
	{
		// Either create default graphics system ...
		Pointer< IGraphicsSystem > pGraphicsSystem = create< IGraphicsSystem >();

		// ... or alternatively we can create a graphics system by name:
		//Pointer< IGraphicsSystem > pGraphicsSystem = create< IGraphicsSystem >("ogre3d");

		Pointer< IGraphicalWorld > pWorld( pGraphicsSystem->create< IGraphicalWorld >() );
	}

#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}