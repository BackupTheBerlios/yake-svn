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
using namespace yake::math;
using namespace yake::data;

static const real physicsStepTime = real(0.1);

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
ValueType linearInterpolate(
	ValueType y1,ValueType y2,
	real mu)
{
	return(y1*(1-mu)+y2*mu);
}

template<typename ValueType>
ValueType cubicInterpolate(
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
namespace yake {
namespace model {
	class DynamicsMoveHistory
	{
	public:
		struct Move
		{
			real		t;
			Vector3		pos;
			Vector3		linVel;
			Vector3		linAcc;
			Quaternion	rot;
			Move(	const real time,
					const Vector3& position, 
					const Vector3& linearVel,
					const Quaternion& orientation) :
				t(time),
				pos(position),
				linVel(linearVel),
				linAcc(Vector3::kZero),
				rot(orientation)
			{}
		};
		void add( const Move& move );
		void interpolateTo( const real time, Vector3& retPos, Quaternion& retRot );
	private:
		typedef std::deque<Move> MoveList;
		MoveList	mMoves;

		real		mTimeRange;
	};
	void DynamicsMoveHistory::add(const Move& move)
	{
		mMoves.push_front( move );
		if (mMoves.size() > 1)
		{
			const real deltaT = mMoves[0].t - mMoves[1].t;
			if (deltaT > 0.)
				mMoves[0].linAcc = (mMoves[0].linVel - mMoves[1].linVel) / (deltaT);
		}

		// keep at least 10 moves and at least the moves for the last 5 seconds.
		while (mMoves.size() > 10 && (move.t - mMoves.back().t) > 5.)
			mMoves.pop_back();
	}
	void DynamicsMoveHistory::interpolateTo( const real time, Vector3& retPos, Quaternion& retRot )
	{
		if (mMoves.empty())
			return;
		if (time < mMoves.back().t)
		{
			retPos = mMoves.back().pos;
			retRot = mMoves.back().rot;
			return;
		}
		if (time > mMoves.front().t)
		{
			// extrapolate
			// @todo use better extrapolating algorithm... and extrapolate orientation, too.
			retPos = mMoves.front().pos;
			retRot = mMoves.front().rot;
			if (mMoves.size() > 1)
			{
				const real deltaT = time - mMoves.front().t;
				retPos += mMoves[1].linAcc * deltaT * deltaT;
			}
			return;
		}
		// interpolate
		if (mMoves.size() < 2)
			return;
		for (size_t i=0; i<mMoves.size()-1; ++i)
		{
			if (time <= mMoves[i].t && time >= mMoves[i+1].t)
			{
				const real startT = mMoves[i+1].t;
				const real endT = mMoves[i].t;
				const real deltaT = endT - startT;
				const real offsetRatioT = (time - startT) / deltaT;

				// simple linear interpolation

				const Vector3 startPos = mMoves[i+1].pos;
				const Vector3 endPos = mMoves[i].pos;
				const Vector3 deltaPos = endPos - startPos;
				const Quaternion startQ = mMoves[i+1].rot;
				const Quaternion endQ = mMoves[i].rot;

				if (deltaT == real(0.))
				{
					retPos = startPos;
					retRot = startQ;
					return;
				}

				//retPos = startPos + deltaPos * offsetT;
#if 0
				retPos = linearInterpolate( startPos, endPos, offsetRatioT );
#else
				// final pos = start pos + start vel * dt + 1/2 * start acc * dt^2;
				const real offsetT = offsetRatioT * deltaT;
				retPos = startPos + mMoves[i+1].linVel * offsetT + .5 * mMoves[i+1].linAcc * offsetT * offsetT;
#endif
				retRot = Quaternion::Slerp( offsetRatioT, startQ, endQ );

				return;
			}
		}
	}


	class ModelLink_DynActorToMovable : public model::ModelLinkController< physics::IActor >
	{
	public:
		YAKE_DECLARE_CONCRETE( ModelLink_DynActorToMovable, "yake.DynamicsActorToMovable" );

		ModelLink_DynActorToMovable(const real startTime = real(0.), const real interpTimeOffset = real(0.));

		virtual void update(real timeElapsed);
		void updateHistoryFromSource(real theTime);

		typedef Signal1< void(const Vector3 &) > PositionSignal;
		typedef Signal1< void(const Quaternion &) > OrientationSignal;

		SignalConnection subscribeToPositionChanged( const PositionSignal::slot_type & slot );
		SignalConnection subscribeToOrientationChanged( const OrientationSignal::slot_type & slot );
		SignalConnection subscribeToPositionChanged( Movable* pMovable );
		SignalConnection subscribeToOrientationChanged( Movable* pMovable );

		typedef model::DynamicsMoveHistory MoveHistory;
		//void addMove( const MoveHistory::Move& move );
	private:
		MoveHistory				mMoveHistory;
		real					mTime;
		real					mInterpTimeOffset;

		PositionSignal			mPositionSignal;
		OrientationSignal		mOrientationSignal;

		Vector3					mLastPosition;
		Quaternion				mLastOrientation;
	};
	ModelLink_DynActorToMovable::ModelLink_DynActorToMovable(const real startTime /*= real(0.)*/, const real interpTimeOffset /*= real(0.)*/) :
		mTime(startTime),
		mInterpTimeOffset(interpTimeOffset)
	{
	}
	void ModelLink_DynActorToMovable::update(real timeElapsed)
	{
		mTime += timeElapsed;
		const physics::IActor* pSource = getSource();
		Vector3 position = pSource ? pSource->getPosition() : Vector3::kZero;
		Quaternion orientation = pSource ? pSource->getOrientation() : Quaternion::kIdentity;
		mMoveHistory.interpolateTo( mTime + mInterpTimeOffset, position, orientation );

		// fire signals if necessary
		if (mLastPosition != position)
		{
			mPositionSignal( position );
			mLastPosition = position;
		}
		if (mLastOrientation != orientation)
		{
			mOrientationSignal( orientation );
			mLastOrientation = orientation;
		}
	}
	void ModelLink_DynActorToMovable::updateHistoryFromSource(real theTime)
	{
		const physics::IActor* pSource = getSource();
		if (!pSource)
			return;
		mMoveHistory.add( MoveHistory::Move(
							theTime,
							pSource->getPosition(), 
							pSource->getBody().getLinearVelocity(),
							pSource->getOrientation()
							)
					);
	}
	SignalConnection ModelLink_DynActorToMovable::subscribeToPositionChanged( Movable* pMovable )
	{
		return mPositionSignal.connect( Bind1( &Movable::setPosition, pMovable ) );
	}
	SignalConnection ModelLink_DynActorToMovable::subscribeToOrientationChanged( Movable* pMovable )
	{
		return mOrientationSignal.connect( Bind1( &Movable::setOrientation, pMovable ) );
	}
} // ns model
} // ns yake

class TheApp : public yake::exapp::ExampleApplication
{
private:
	Vector< std::pair<IViewport*,ICamera*> >	mVPs;
	
	SharedPtr<physics::IWorld>					mPWorld;
	SharedPtr<graphics::IWorld>					mGWorld;

	struct Object
	{
		SharedPtr<model::complex::Model>	mComplex;
		SharedPtr<model::Graphical>		mGraphical;

		physics::IActor*						mPhysical;
		graphics::ISceneNode*					mpSN;

		typedef yake::model::ModelLink_DynActorToMovable Link;
		Link*									mpLink;
		ScopedSignalConnection					mPostStepConn;

		Object() : mpSN(0), mPhysical(0)
		{}
	};
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
	void setupObject(Object& obj)
	{
		obj.mComplex.reset( new model::complex::Model() );
		using namespace model;

		// Loading physical part
		obj.mPhysical = mPWorld->createActor( physics::ACTOR_DYNAMIC );
		YAKE_ASSERT( obj.mPhysical );
		obj.mPhysical->createShape( physics::IShape::BoxDesc(Vector3(1,1,1)) );
		Physical* pP = new Physical();
		pP->addActor( obj.mPhysical, "root" );
		obj.mComplex->addPhysical( SharedPtr<Physical>(pP), "p_box" );

		// Loading graphical part
		obj.mGraphical.reset( new model::Graphical() );
		YAKE_ASSERT( obj.mGraphical );

		obj.mpSN = mGWorld->createSceneNode();
		obj.mGraphical->addSceneNode( obj.mpSN, false );
		obj.mpSN->attachEntity( mGWorld->createEntity("box_1x1x1.mesh") );

		// linking
		{
			obj.mpLink = new Object::Link(real(0.),-physicsStepTime);
			obj.mpLink->setSource( obj.mPhysical );
			obj.mpLink->subscribeToPositionChanged( obj.mpSN );
			obj.mpLink->subscribeToOrientationChanged( obj.mpSN );

			obj.mComplex->addLink( SharedPtr<model::ModelLink>( obj.mpLink ), "body2scenenode" );
		}

		obj.mPostStepConn = mPWorld->subscribeToPostStep(
			Bind1( &Object::Link::updateHistoryFromSource, obj.mpLink ) );
	}

	virtual void run()
	{
		// setup event input generators
		mKeyboardEventGenerator.subscribeToKeyDown( Bind1( &TheApp::onKey, this ) );
		mMouseEventGenerator.subscribeToMouseButtonDown( Bind1( &TheApp::onMB, this ) );
		
		// physics
		mPWorld = getPhysicsSystem().createWorld();
		YAKE_ASSERT( mPWorld );
		
		//mPWorld->setGlobalGravity( Vector3( 0, -9.81, 0 ) );
		mPWorld->setGlobalGravity( Vector3( 0, -1., 0 ) );

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
		Object obj;
		setupObject(obj);

		// main loop
		real lastTime = native::getTime();
		while (!shutdownRequested())
		{
			// timing
			real time = native::getTime();
			real timeElapsed = time - lastTime;
			if (timeElapsed > real(0.05))
				timeElapsed = real(0.05);
			lastTime += timeElapsed;

			static real timeScale = 1.;//0.05;
			timeElapsed *= timeScale;

			mSimTime += timeElapsed;
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
			
				//// physics: may use fixed time step, so funtion may return without actually stepping!

				// TEMP: We simulate a fixed time step... just for testing. normally it's handled by the
				//       physics::IWorld::step() code.
				static real ptimeElapsed = 0.;
				ptimeElapsed += timeElapsed;
				while (ptimeElapsed > physicsStepTime)
				{
					mPWorld->step( physicsStepTime );
					ptimeElapsed -= physicsStepTime;
				}

				//// graphics: variable time step: as fast as possible
				obj.mComplex->updateGraphics( timeElapsed );
				
				mGWorld->render( timeElapsed );
			}
		}

		obj.mGraphical.reset();
		//obj.mPhysical.reset();
		obj.mComplex.reset();
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
	catch (const yake::Exception& rException)
	{
		std::cout << std::endl << rException.what() << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}
