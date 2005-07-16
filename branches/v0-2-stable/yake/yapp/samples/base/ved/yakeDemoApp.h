#ifndef YAPP_DEMO_VED_H
#define YAPP_DEMO_VED_H
#include <yapp/samples/common/yakeHelper.h>
#include <yapp/common/yakeCEGUIRendererAdapter.h>

using namespace yake;
using namespace yake::base::templates;
using namespace yake::base::math;
using namespace yake::graphics;
using namespace yake::data;

using namespace yapp;
using namespace yake::app;

namespace yake {
	class RoadCreator;
	class TrailCreator;
}
class TheApp : public yake::exapp::ExampleApplication
{
//private:
public:
	yake::ceguiadapter::RendererAdapterPlugin* mAdapterPlugin;
	CameraFollowController*		mpCfc;

	task::TaskManager					mTaskMgr;
	// task: render
	class RenderTask : public task::DefTask
	{
	private:
		IGraphicalWorld*	mWorld;
	public:
		RenderTask( IGraphicalWorld* pWorld );
		virtual void execute(real timeElapsed);

		typedef Signal1< void(real) > PreSignal;
		void subscribeToPre( const PreSignal::slot_type & slot )
		{ mPreSig.connect( slot ); }
	private:
		PreSignal	mPreSig;
	};
	RenderTask*							mRenderTask;

	// task: physics
	class PhysicsTask : public task::DefTask
	{
	private:
		physics::IWorld*	mWorld;
	public:
		PhysicsTask( physics::IWorld* pWorld );
		virtual void execute(real timeElapsed);

		typedef Signal0< void > VoidSignal;
		void subscribeToPre( const VoidSignal::slot_type & slot )
		{ mPreSignal.connect( slot ); }
		void subscribeToPost( const VoidSignal::slot_type & slot )
		{ mPostSignal.connect( slot ); }
	private:
		VoidSignal		mPreSignal;
		VoidSignal		mPostSignal;
	};
	PhysicsTask*						mPhysicsTask;

	// task: vehicle sim
	class VehicleSimTask : public task::DefTask
	{
	private:
		model::complex::vehicle::LandVehicle*	mCar;
	public:
		VehicleSimTask( model::complex::vehicle::LandVehicle* theCar );
		virtual void execute(real timeElapsed);
	};
	VehicleSimTask*						mVehicleSimTask;

	// task: input
	class InputTask : public task::DefTask
	{
	private:
		TheApp*	mApp;
		model::complex::vehicle::LandVehicle*	mCar;

	public:
		InputTask( 	TheApp* theApp, model::complex::vehicle::LandVehicle* theCar );
		virtual void execute(real timeElapsed);
	};
	InputTask*							mInputTask;

	Vector<std::pair<IViewport*,ICamera*> >	mVPs;
	SharedPtr< IGraphicalWorld >			mGWorld;
	physics::IWorld*						mPWorld;
	audio::IWorld*							mpAWorld;

	physics::IComplexObject* dbgChassis;

	audio::IListener*					mpAListener;
	audio::ISource*						mpASource;
	audio::ISoundData*					mpAData;

	graphics::ISceneNode*				mLightOneNode;
	graphics::IParticleSystem*			mSmoke;

	struct SimpleOne {
		graphics::ISceneNode*			pSN;
		graphics::IEntity*				pE;
		physics::IComplexObject*		pCO;
		model::MovableUpdateController*	updCtrl;
	};
	SimpleOne								mGround;
	model::complex::Model*					mCarModel;
	model::complex::vehicle::LandVehicle*	mCar;
	SimpleOne								mBall;

	RoadCreator*						mRoadCreator;
	TrailCreator*						mSkidCreator;

	real								mCurrentSimTime;
public:
	TheApp() : ExampleApplication( true /*graphics*/,
									true /*physics*/,
									false /*scripting*/,
									true /*input*/,
									false /*script bindings*/,
									false /*audio*/),
				mCarModel(0),
				mCar(0),
				mpAWorld(0),
				mpASource(0),
				mpAListener(0),
				mpAData(0),
				mLightOneNode(0),
				mRenderTask(0),
				mPhysicsTask(0),
				mInputTask(0),
				mVehicleSimTask(0),
				mAdapterPlugin(0),
				mRoadCreator(0),
				mSkidCreator(0),
				mCurrentSimTime(0),
				mpCfc(0)
	{
	}

	InputTask* getInputTask() const
	{
		return mInputTask;
	}
	RenderTask* getRenderTask() const
	{
		return mRenderTask;
	}
	PhysicsTask* getPhysicsTask() const
	{
		return mPhysicsTask;
	}

	real getSimulationTime()
	{
		return mCurrentSimTime;
	}

	void preRender( real timeElapsed );

	void handleInput( real timeElapsed );

	void onKey(const yake::input::KeyboardEvent & e)
	{
		if (e.keyCode == input::KC_ESCAPE)
			requestShutdown();
	}
	void onMBUp(uint8 btn)
	{
		CEGUI::System::getSingleton().injectMouseButtonUp( CEGUI::LeftButton );
	}
	void onMBDown(uint8 btn)
	{
		CEGUI::System::getSingleton().injectMouseButtonDown( CEGUI::LeftButton );
	}
	void onMouseMoved( const Vector3 & position )
	{
		real scale = 1;
		static Vector3 lastPosition = position;
		Vector3 diff = position - lastPosition;
		lastPosition = position;

		CEGUI::System::getSingleton().injectMouseMove(diff.x*scale, diff.y*scale);
		//CEGUI::Renderer* rend = CEGUI::System::getSingleton().getRenderer();
		//CEGUI::System::getSingleton().injectMouseMove(e->getRelX() * rend->getWidth(), e->getRelY() * rend->getHeight());
	}

	int createCameraViewportPair( real sx, real sy, real w, real h, int z );
	void setupDotScene();
	void setupGround();
	void setupAudio();
	void cleanUpAudio();

	void setupBall();
	void cleanUpBall();

			void createWheel(
				model::complex::vehicle::Wheel *& pWheel,
				physics::IComplexObject *& pCO,
				SharedPtr<physics::IJoint> & pJ,
				physics::IComplexObject* pCOChassis,
				const real radius,
				const Vector3 & position,
				const Quaternion & orientation,
				const Vector3 & anchorPt,
				const Vector3 & hingeSteeringAxis,
				const Vector3 & hingeSuspensionAxis,
				const real suspSpring,
				const real suspDamping,
				const real wheelMass );
			model::complex::vehicle::Wheel* createWheel(	
								const Vector3 & position,
								const Quaternion & orientation,
								physics::IComplexObject* pCOChassis,
								SharedPtr<model::Physical> & pPhysical,
								const real radius,
								const real suspSpring,
								const real suspDamping,
								const real wheelMass,
								Vector<graphics::ISceneNode*> & nodeList,
								SharedMUCList & mucList,
								const String & wheelMesh = "wheel1.mesh");
	void setupCar();
	void setupLights();
	void setupGui();
	void cleanUpGui();
	void createGuiWindows();

	void setupRoad();
	void cleanUpRoad();

	virtual void run();
};


#endif