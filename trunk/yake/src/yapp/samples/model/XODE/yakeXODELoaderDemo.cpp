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
#include <yapp/samples/model/yakePCH.h>
#include <yapp/model/yakePhysical.h>
#include <yapp/loader/yakeXODEParser.h>

using namespace yake;
using namespace yake::base::templates;
using namespace yake::base::math;
using namespace yake::data;

class TheApp : public yake::exapp::ExampleApplication
{
private:
	SharedPtr<physics::IWorld>			mPWorld;

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
		YAKE_ASSERT( mPhysical );
		
		yake::data::dom::xml::XmlSerializer ser;
		ser.parse( mWorldFilename, false );
		YAKE_ASSERT( ser.getDocumentNode() );

		
		parser::xode::XODEParserV1 parser( *mPhysical );
		parser.load( ser.getDocumentNode() , mPWorld.get() );
	}

	virtual void run()
	{
		// physics
		mPWorld = getPhysicsSystem().createWorld();
		YAKE_ASSERT( mPWorld );

		// objects
		setupWorld();

		YAKE_LOG("Running simulation...");

		// main loop
		real lastTime = base::native::getTime();
		while (!shutdownRequested())
		{
			// timing
			static real totalTimeElapsed = 0;
			real time = base::native::getTime();
			real timeElapsed = time - lastTime;
			lastTime = time;
			totalTimeElapsed += timeElapsed;
			if (totalTimeElapsed > 1) // run not more than 1 second
				requestShutdown();

			// step simulation
			if ( !shutdownRequested() )
				mPWorld->step( timeElapsed );
		}
		YAKE_LOG("...done!");

		YAKE_SAFE_DELETE( mPhysical );

		mPWorld.reset();
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
