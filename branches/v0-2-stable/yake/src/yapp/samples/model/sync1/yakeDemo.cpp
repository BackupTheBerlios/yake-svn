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
#include <yake/samples/graphics/pch.h>
#include <yapp/model/yakeGraphical.h>
#include <yapp/base/yapp.h>
#include <yapp/model/yakePhysical.h>
#include <yapp/model/yakeModelMovableLink.h>

using namespace yake;
using namespace yake::graphics;
using namespace yake::base::templates;
using namespace yake::base::math;
using namespace yake::data;

//#define COUTLN(x)
#define COUTLN(x) { std::cout << x << "\n"; }

typedef uint64 SimTime;
SimTime toSimTime(const real timeInSec)
{
	return (timeInSec * 1000.);
}
real fromSimTime(const SimTime time)
{
	return real(time) / 1000.;
}

template<typename ValueType>
ValueType LinearInterpolate(
	ValueType y1,ValueType y2,
	real mu)
{
	return(y1*(1-mu)+y2*mu);
}

template<typename ValueType>
ValueType CubicInterpolate(
	ValueType y0,ValueType y1,
	ValueType y2,ValueType y3,
	real mu)
{
	real mu2 = mu*mu;
	ValueType a0 = y3 - y2 - y0 + y1;
	ValueType a1 = y0 - y1 - a0;
	ValueType a2 = y2 - y0;
	ValueType a3 = y1;

	return(a0*mu*mu2+a1*mu2+a2*mu+a3);
}

struct ExampleSimulation
{
	real getCurrentTime() const;
};
class TheApp : public yake::exapp::ExampleApplication
{
private:
	Vector< std::pair<IViewport*,ICamera*> >	mVPs;
	
	SharedPtr<physics::IWorld>					mPWorld;
	SharedPtr<graphics::IWorld>					mGWorld;

	struct Object
	{
		physics::IDynamicActor*				mPhysical;
		SharedPtr<app::model::Graphical>	mGraphical;
		graphics::ISceneNode*				mpSN;
		typedef app::model::DynamicsMoveHistory<TheApp> DynHistory;
		DynHistory							mHistory;
		typedef app::model::ModelMovableLink Link;
		Link*								mpLink;
		//MovableHistory						mPhysicalHistory;
		void updateGraphics(real currentSimTime, real timeElapsed)
		{
			mpLink->update( timeElapsed );
			/*
			Vector3 p = mPhysical->getPosition();
			mPhysicalHistory.interpolateTo( currentSimTime, p );
			//mpSN->setPosition( p );

			Vector3 currSnPos = mpSN->getPosition();
			mpSN->setPosition( currSnPos + (p-currSnPos) * 0.5 );
			*/
		}
		Object(TheApp& rSim) : mpSN(0), mHistory(rSim)
		{}
		void onPostStep(const real currentSimTime)
		{
			mHistory.addMove( DynHistory::Move( currentSimTime, mPhysical->getPosition(), mPhysical->getBody().getLinearVelocity() ) );
		}
	};
	SharedPtr<app::model::complex::Model>	mComplex;
	real									mSimTime;
public:
	real getCurrentTime()
	{ return mSimTime; }
	TheApp() :
		ExampleApplication( 	true /*graphics*/,
								true /*physics*/,
								false /*scripting*/,
								true /*input*/,
								false /*script bindings*/,
								false /*audio*/),
		mSimTime(0)
	{
	}
	void onKey(const yake::input::KeyboardEvent& rEvent)
	{
		if ( rEvent.keyCode == input::KC_ESCAPE )
			requestShutdown();
	}
	void onMB(uint8 btn)
	{
	}
	int createCameraViewportPair( real sx, real sy, real w, real h, int z )
	{
		ICamera* pC = mGWorld->createCamera();
		YAKE_ASSERT( pC );
		pC->setNearClipDistance( 1. );

		pC->setFarClipDistance( 2000 );

		mVPs.push_back( std::pair<IViewport*,ICamera*>(mGWorld->createViewport( pC ), pC) );
		size_t idx = mVPs.size()-1;
		YAKE_ASSERT( mVPs[idx].first );
		mVPs[idx].first->setDimensions( sx, sy, w, h );
		mVPs[idx].first->setZ( z );
		
		return static_cast<int>(idx);
	}
	void setupWorld(Object& obj)
	{
		mComplex.reset( new app::model::complex::Model() );
		using namespace app::model;
/*
		Graphical* pG = new Graphical();
		graphics::ISceneNode* pSN = mGWorld->createSceneNode();
		pSN->attachEntity( mGWorld->createEntity("box_1x1x1.mesh") );
		pG->addSceneNode( pSN, "root", true );
		mComplex->addGraphical( SharedPtr<Graphical>(pG), "box" );
*/
		physics::WeakIDynamicActorPtr pA = mPWorld->createDynamicActor();
		pA.lock()->createShape( physics::IShape::BoxDesc(Vector3(1,1,1)) );
		Physical* pP = new Physical();
		pP->addActor( physics::WeakIActorPtr(pA), "root" );
		mComplex->addPhysical( SharedPtr<Physical>(pP), "p_box" );
/*
		ModelMovableLink* pC = new ModelMovableLink();
		pC->setSource( pA.lock().get() );
		pC->subscribeToPositionChanged( pSN );
		pC->subscribeToOrientationChanged( pSN );
		mComplex->addController( SharedPtr<IObjectController>(pC), "p2g" );
*/
		// Loading graphical part
		obj.mGraphical.reset( new app::model::Graphical() );
		YAKE_ASSERT( obj.mGraphical );

		obj.mpSN = mGWorld->createSceneNode();
		obj.mGraphical->addSceneNode( obj.mpSN, "base" );
		obj.mpSN->attachEntity( mGWorld->createEntity("box_1x1x1.mesh") );

		// Loading physical part
		obj.mPhysical = pA.lock().get();
		//obj.mPhysical.reset( new ProxyMovable() );
		YAKE_ASSERT( obj.mPhysical );

		// linking
		{
			obj.mpLink = new Object::Link();
			obj.mpLink->setSource( obj.mPhysical );
			obj.mpLink->subscribeToPositionChanged( obj.mpSN );
			obj.mpLink->subscribeToOrientationChanged( obj.mpSN );
		}
	}

	virtual void run()
	{
		// setup event input generators
		mKeyboardEventGenerator.subscribeToKeyDown( Bind1( &TheApp::onKey, this ) );
		mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( &TheApp::onMB, this ) );
		
		// physics
		mPWorld = getPhysicsSystem().createWorld();
		YAKE_ASSERT( mPWorld );
		
		//@todo mPWorld->setGlobalGravity( Vector3( 0, 0, -9.81 ) );

		// graphics
		mGWorld = getGraphicsSystem().createWorld();
		YAKE_ASSERT( mGWorld );
		
		createCameraViewportPair( 0.0, 0.0, 1, 1, 10 );

		if (mVPs[0].second)
		{
			mVPs[0].second->translate( Vector3( -2,-2,10 ) );
			//mVPs[0].second->rotate( Vector3( 0, 1, 0 ), 45 );
			//mVPs[0].second->pitch(-10);
		}

		graphics::ILight* pL = mGWorld->createLight();
		pL->setType( graphics::ILight::LT_DIRECTIONAL );
		pL->setDirection( Vector3(1,-1,0).normalisedCopy() );

		// objects
		Object obj(*this);
		setupWorld(obj);

		// main loop
		real lastTime = base::native::getTime();
		while (!shutdownRequested())
		{
			// timing
			real time = base::native::getTime();
			real timeElapsed = time - lastTime;
			if (timeElapsed > 0.1)
				timeElapsed = 0.1;
			lastTime += timeElapsed;

			static real timeScale = 1.;//0.05;
			timeElapsed *= timeScale;

			SimTime simtime = toSimTime( mSimTime );
			
			// handle input
			getInputSystem().update();
			mMouseEventGenerator.update();
			mKeyboardEventGenerator.update();
			
			YAKE_ASSERT( getKeyboard() );
			if ( getKeyboard() )
			{
				static  unsigned iCam = 0;
				real distance = 200. * timeElapsed;
				real angle = 10 * timeElapsed;
				if (getKeyboard()->isKeyDown(input::KC_LEFT))
					mVPs[iCam].second->translate( Vector3( distance, 0, 0 ) );
				if (getKeyboard()->isKeyDown(input::KC_RIGHT))
					mVPs[iCam].second->translate( Vector3( -distance, 0, 0 ) );
				if (getKeyboard()->isKeyDown(input::KC_UP))
					mVPs[iCam].second->translate( Vector3( 0, 0, distance ) );
				if (getKeyboard()->isKeyDown(input::KC_DOWN))
					mVPs[iCam].second->translate( Vector3( 0, 0, -distance ) );
				if (getKeyboard()->isKeyDown(input::KC_Z))
					mVPs[iCam].second->translate( Vector3( 0, distance, 0 ) );
				if (getKeyboard()->isKeyDown(input::KC_X))
					mVPs[iCam].second->translate( Vector3( 0, -distance, 0 ) );
				if (getKeyboard()->isKeyDown(input::KC_Q))
					mVPs[0].second->rotate( Vector3( 0, 1, 0 ), angle );
				if (getKeyboard()->isKeyDown(input::KC_W))
					mVPs[0].second->rotate( Vector3( 0, 1, 0 ), -angle );
				if (getKeyboard()->isKeyDown(input::KC_PGUP))
					obj.mPhysical->getBody().addForce( Vector3(0,300,0) );
			}

			// step simulation
			if ( !shutdownRequested() )
			{
			
				//// physics: uses fixed time step, so funtion may return without actually stepping!
				//obj.mPhysical->step( timeElapsed );
				mPWorld->step( timeElapsed );

				obj.onPostStep( mSimTime );

				//// graphics: variable time step: as fast as possible
				obj.updateGraphics( mSimTime, timeElapsed );
				//obj.updateGraphics( simTime - PHYSICS_STEP_TIME, timeElapsed );

				mComplex->updateControllers( timeElapsed );
				
				mGWorld->render( timeElapsed );

				mSimTime += timeElapsed;
			}
		}

		obj.mGraphical.reset();
		//obj.mPhysical.reset();
		mGWorld.reset();
		mPWorld.reset();
	}
};

//============================================================================
//    MAIN ENTRY
//============================================================================

int main( int argc, char** argv )
{
	try
	{
		std::cout << std::endl << "A simple demo ;) provided for YAKE by Stephan Kaiser" << std::endl;
		std::cout << std::endl;
		
		TheApp theApp;
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
