/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yapp/samples/model/yakePCH.h>
#include <yapp/model/yakePhysical.h>
#include <yapp/loader/yakeXODE.h>

using namespace yake;
using namespace yake::base::templates;
using namespace yake::base::math;
using namespace yake::data;

class TheApp : public yake::exapp::ExampleApplication
{
private:
	physics::IWorld*				mPWorld;

	app::model::Physical* 				mPhysical;
	
	String	mWorldFilename;
public:
	TheApp( const String filename ) : ExampleApplication(   false /*graphics*/,
								true /*physics*/,
								false /*scripting*/,
								false /*input*/,
								false /*script bindings*/,
								false /*audio*/),
								mWorldFilename( filename )
	{
	}

	void setupWorld()
	{
		mPhysical = new app::model::Physical();
		YAKE_ASSERT( pPhysical );
		
		yake::data::dom::xml::XmlSerializer ser;
		ser.parse( mWorldFilename, false );
		YAKE_ASSERT( ser.getDocumentNode() );

		
		parser::xode::XODEParserV1 parser( *mPhysical );
		parser.load( ser.getDocumentNode() , mPWorld );
	}

	virtual void run()
	{
		// physics
		mPWorld = getPhysicsSystem().createWorld();
		YAKE_ASSERT( mPWorld );

		// objects
		setupWorld();

		// main loop
		real lastTime = base::native::getTime();
		while (!shutdownRequested())
		{
			// timing
			real time = base::native::getTime();
			real timeElapsed = time - lastTime;
			lastTime = time;

			// step simulation
			if ( !shutdownRequested() )
				mPWorld->update( timeElapsed );
		}

		YAKE_SAFE_DELETE( mPhysical );

		YAKE_SAFE_DELETE( mPWorld );
	}
};

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================

int main( int argc, char** argv )
{
	if ( argc <= 1 )
	{
		std::cout << "Usage: XODELoaderDemo filename.xode " << std::endl;
		return 0; 
	}
	
	try
	{
		std::cout << std::endl << "A simple demo ;) provided for YAKE by Nikita Buida" << std::endl;
		std::cout << std::endl;
		
		TheApp theApp( argv[1] );
		theApp.initialise();
		theApp.run();
	}
	catch (const yake::base::Exception& rException)
	{
		std::cout << std::endl << rException.what() << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}
