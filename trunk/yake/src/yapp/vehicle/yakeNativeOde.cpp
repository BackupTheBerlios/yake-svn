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
#include <yapp/vehicle/yakePCH.h>
#include <yapp/vehicle/yakeVehicle.h>
#include <yapp/vehicle/yakeNativeOde.h>
#include <yapp/vehicle/yakeDotVehicle.h>

#if defined(YAKE_VEHICLE_USE_ODE)
#	include <dependencies/ode/include/ode/ode.h>
#	include <dependencies/ode/include/ode/odecpp.h>
#	include <dependencies/ode/include/ode/objects.h>
#	include <yake/plugins/physicsODE/OdeJoint.h>
#	include <yake/plugins/physicsODE/OdeBody.h>
#	include <yake/plugins/physicsODE/OdeWorld.h>
#	pragma comment(lib, "physicsODE.lib")
#	pragma comment(lib, "oded.lib")
#endif // #if defined(YAKE_VEHICLE_USE_ODE)

namespace yake {
namespace vehicle {

	YAKE_IMPLEMENT_REGISTRY(IVehicleSystem);
	YAKE_REGISTER_CONCRETE(GenericVehicleSystem);

	//-----------------------------------------------------
	// Class: GenericVehicleSystem
	//-----------------------------------------------------
	GenericVehicleSystem::GenericVehicleSystem()
	{}
	GenericVehicleSystem::~GenericVehicleSystem()
	{
		mVehicleTemplates.clear();
	}
	IVehicle* GenericVehicleSystem::create(const VehicleTemplate& tpl, physics::IWorld& PWorld)
	{
		GenericVehicle* pV = new GenericVehicle();
		pV->_create( tpl, PWorld );
		return pV;
	}
	bool GenericVehicleSystem::loadTemplates(const String& fn)
	{
		DotVehicleParser dvp;
		dvp.subscribeToOnVehicleTpl( boost::bind(&GenericVehicleSystem::_onVehicleTpl,this,_1,_2) );
		bool ret = dvp.parse( fn );
		return ret;
	}
	VehicleTemplate* GenericVehicleSystem::getTemplate(const String& tplId) const
	{
		VehTplList::const_iterator itFind = mVehicleTemplates.find( tplId );
		YAKE_ASSERT( itFind != mVehicleTemplates.end() );
		if (itFind == mVehicleTemplates.end() )
			return 0;
		VehicleTemplate* tpl = itFind->second.get();
		YAKE_ASSERT( tpl );
		if (!tpl)
			return 0;
		return tpl;
	}
	//VehicleTemplate* GenericVehicleSystem::cloneTemplate(const String& tpl)
	//{
	//	VehTplList::const_iterator itFind = mVehicleTemplates.find( tplId );
	//	YAKE_ASSERT( itFind != mVehicleTemplates.end() );
	//	if (itFind == mVehicleTemplates.end() )
	//		return 0;
	//	const VehicleTemplate* tpl = itFind->second.get();
	//	YAKE_ASSERT( tpl );
	//	if (!tpl)
	//		return 0;
	//	return tpl->clone();
	//}
	IVehicle* GenericVehicleSystem::create(const String& tplId, physics::IWorld& PWorld)
	{
		const VehicleTemplate* tpl = getTemplate( tplId );
		YAKE_ASSERT( tpl );
		if (!tpl)
			return 0;
		return create(*tpl,PWorld);
	}
	void GenericVehicleSystem::_onVehicleTpl(vehicle::DotVehicleParser& parser, const String& tplId)
	{
		YAKE_LOG("GenericVehicleSystem: loaded vehicle template'" + tplId + "'.");
		mVehicleTemplates[ tplId ] = SharedPtr<vehicle::VehicleTemplate>( parser.detachCurrentVehicleTpl() );
		YAKE_ASSERT( mVehicleTemplates[ tplId ] );
	}

	//-----------------------------------------------------
	// Class: GenericVehicle
	//-----------------------------------------------------
	GenericVehicle::GenericVehicle() : mpChassis(0), mDebugModel(0)
	{
	}
	GenericVehicle::~GenericVehicle()
	{
		YAKE_SAFE_DELETE( mDebugModel );

		for (SteeringGroupList::const_iterator it = mSteeringGroups.begin(); it != mSteeringGroups.end(); ++it)
		{
			ConstDequeIterator< Deque<OdeWheel*> > itWheel( it->second );
			while (itWheel.hasMoreElements())
				delete itWheel.getNext();
		}
		mSteeringGroups.clear();
		mWheels.clear();

		ConstDequeIterator< EnginePtrList > itEngine( mEngines );
		while (itEngine.hasMoreElements())
			delete itEngine.getNext().second;
		mEngines.clear();

		ConstDequeIterator< MountPointList > itMP( mMountPoints );
		while (itMP.hasMoreElements())
			delete itMP.getNext().second;
		mMountPoints.clear();

		mpChassis->getCreator()->destroyActor( mpChassis );
		mpChassis = 0;
	}
	void GenericVehicle::updateSimulation(real timeElapsed)
	{
		ConstDequeIterator< EnginePtrList > itEngine( mEngines );
		while (itEngine.hasMoreElements())
		{
			IEngine* pEngine = itEngine.getNext().second;
			pEngine->updateSimulation(timeElapsed);
		}
		//
		mSigUpdateEngineSimulation( timeElapsed );
		mSigApplyThrusterToTargets();
		_applyDriveTorqueToAxles( timeElapsed );

		if (mDebugModel)
		{
			mDebugModel->updatePhysics(timeElapsed);
			mDebugModel->updateGraphics(timeElapsed);
		}
	}
	void GenericVehicle::_applyDriveTorqueToAxles( real timeElapsed )
	{
		ConstDequeIterator< AxleList > itAxle( mAxles );
		while (itAxle.hasMoreElements())
		{
			CarEngineWheelsPair cewp = itAxle.getNext().second;

			const real torque = timeElapsed * cewp.first->getDriveTorque();
			ConstDequeIterator< Deque<OdeWheel*> > itWheel( cewp.second );
			while (itWheel.hasMoreElements())
			{
				OdeWheel* pW = itWheel.getNext();
				pW->_applyDriveTq( torque );
			}
		}
	}
	MountPoint* GenericVehicle::getMountPoint(const String& id) const
	{
		YAKE_ASSERT( !id.empty() );
		if (id.empty())
			return 0;
		MountPointList::const_iterator itFind = mMountPoints.find(id);
		YAKE_ASSERT( itFind != mMountPoints.end() );
		if (itFind == mMountPoints.end())
			return 0;
		return itFind->second;
	}
	IEngine* GenericVehicle::getEngineInterface(const String& id) const
	{
		YAKE_ASSERT( !id.empty() );
		if (id.empty())
			return 0;
		EnginePtrList::const_iterator itFind = mEngines.find(id);
		YAKE_ASSERT( itFind != mEngines.end() );
		if (itFind == mEngines.end())
			return 0;
		return itFind->second;
	}
	IEnginePtrList GenericVehicle::getEngineInterfaces() const
	{
		IEnginePtrList engines;
		ConstVectorIterator< EnginePtrList > itEngine( mEngines );
		while (itEngine.hasMoreElements())
			engines.push_back( itEngine.getNext().second );
		return engines;
	}
	IWheel* GenericVehicle::getWheelInterface(const String& id) const
	{
		YAKE_ASSERT( !id.empty() );
		if (id.empty())
			return 0;
		WheelList::const_iterator itFind = mWheels.find(id);
		//YAKE_ASSERT( itFind != mWheels.end() )( id );
		if (itFind == mWheels.end())
			return 0;
		return itFind->second;
	}
	Vector3 GenericVehicle::getChassisPosition() const
	{
		YAKE_ASSERT( mpChassis );
		return mpChassis->getPosition();
	}
	Quaternion GenericVehicle::getChassisOrientation() const
	{
		YAKE_ASSERT( mpChassis );
		return mpChassis->getOrientation();
	}
	Movable* GenericVehicle::getChassisMovable() const
	{
		YAKE_ASSERT( mpChassis );
		return mpChassis;
	}
	void GenericVehicle::_createMountPoint(const String& id, const VehicleTemplate::MountPointTpl& mtPtTpl,MountPoint* parentMtPt)
	{
		MountPoint* thisMtPt = new GenericMountPoint();

		// link to parent if necessary
		if (parentMtPt)
			parentMtPt->addChild( thisMtPt );
		else
		{
			mMountPoints[ id ] = thisMtPt;
			thisMtPt->setOverrideParentMovable( mpChassis );
		}

		//
		thisMtPt->setPosition( mtPtTpl.mPosition );
		if (mtPtTpl.mUseDirection)
			thisMtPt->setDirection( mtPtTpl.mDirection );
		else
			thisMtPt->setOrientation( mtPtTpl.mOrientation );

		// child mount points
		ConstVectorIterator< VehicleTemplate::MountPointTplList > itMP( mtPtTpl.mChildren );
		while (itMP.hasMoreElements())
		{
			const std::pair<String,VehicleTemplate::MountPointTpl> mptTplEntry = itMP.getNext();
			const VehicleTemplate::MountPointTpl& childMtPtTpl = mptTplEntry.second;

			_createMountPoint( mptTplEntry.first, childMtPtTpl, thisMtPt );
		}
	}
	void GenericVehicle::_create(const VehicleTemplate& tpl, physics::IWorld& PWorld )
	{
		// chassis
		mpChassis = PWorld.createActor( physics::ACTOR_DYNAMIC );
		mpChassis->setPosition( tpl.mChassis.mPosition );
		//mpChassis->setOrientation( tpl.mChassis.mOrientation );
		ConstDequeIterator< VehicleTemplate::ShapeTplList > itShapeTpl( tpl.mChassis.mChassisShapes );
		while (itShapeTpl.hasMoreElements())
		{
			mpChassis->createShape( *itShapeTpl.getNext() );
		}
		mpChassis->getBody().setMass( tpl.mChassis.mMass );

		// mount points
		ConstDequeIterator< VehicleTemplate::MountPointTplList > itMP( tpl.mMountPoints );
		while (itMP.hasMoreElements())
		{
			const std::pair<String,VehicleTemplate::MountPointTpl> mptTplEntry = itMP.getNext();
			const VehicleTemplate::MountPointTpl& mptTpl = mptTplEntry.second;

			_createMountPoint( mptTplEntry.first, mptTpl );
		}

		// engines
		ConstDequeIterator< VehicleTemplate::EngineTplList > itEngineTpl( tpl.mEngines );
		while (itEngineTpl.hasMoreElements())
		{
			//@todo refactor to avoid casting... ?

			const VehicleTemplate::EngineTplList::value_type engineTplEntry = itEngineTpl.getNext();
			const VehicleTemplate::EngineTpl* engineTpl = engineTplEntry.second;

			//@todo replace dynamic_casts with static_casts in release mode ?

			const VehicleTemplate::CarEngineTpl* carEngineTpl =
					dynamic_cast<const VehicleTemplate::CarEngineTpl*>( engineTpl );
			if (carEngineTpl)
			{
				GenericCarEngine* pEngine = new GenericCarEngine();
				mEngines[ engineTplEntry.first ] = pEngine;
				pEngine->setParamMaxRPM( carEngineTpl->rpmMax_ );
				pEngine->setParamMinRPM( carEngineTpl->rpmMin_ );
				pEngine->setParamRedlineRPM( carEngineTpl->rpmRedline_ );
				pEngine->getGearBox().setFromTemplate( carEngineTpl->gears_ );
				subscribeToUpdateEngineSimulation(
					boost::bind( &GenericCarEngine::updateSimulation, pEngine, _1 ) );

				mAxles[ carEngineTpl->axle_ ] = std::make_pair( pEngine, Deque<OdeWheel*>() );
			}
			else
			{
				const VehicleTemplate::ThrusterTpl* thrusterTpl =
						dynamic_cast<const VehicleTemplate::ThrusterTpl*>( engineTpl );
				if (thrusterTpl)
				{
					// thruster itself
					GenericThruster* pEngine = new GenericThruster();
					mEngines[ engineTplEntry.first ] = pEngine;
					pEngine->setMinimumForce( thrusterTpl->minForce );
					pEngine->setMaximumForce( thrusterTpl->maxForce );
					pEngine->setThrottle(0.);
					subscribeToUpdateEngineSimulation(
						boost::bind( &GenericThruster::updateSimulation, pEngine, _1 ) );

					// the thruster is attached to a mount point, create wrapper:
					if (thrusterTpl->mountPt != MPID_NO_PARENT)
					{
						GenericMountedThruster* pMounted = new GenericMountedThruster();
						pMounted->setThruster( pEngine );

						pMounted->addTarget( mpChassis->getBodyPtr() );

						subscribeToApplyThrusterToTargets(
							boost::bind( &GenericMountedThruster::applyToTargets, pMounted ) );

						mMountPoints[thrusterTpl->mountPt]->attach( pMounted );
					}
				}
			}
		}
	
		// steering groups
		uint32 numStGrps = tpl.mSteeringGroups > 0 ? tpl.mSteeringGroups : 1;
		for (uint32 i=0; i<numStGrps; ++i)
			mSteeringGroups.insert( std::make_pair(i,Deque<OdeWheel*>()) );

		// wheels
		ConstDequeIterator< VehicleTemplate::WheelTplList > itWheelTpl( tpl.mWheels );
		while (itWheelTpl.hasMoreElements())
		{
			const std::pair<String,VehicleTemplate::WheelTpl> wtp = itWheelTpl.getNext();
			const VehicleTemplate::WheelTpl& wheelTpl = wtp.second;
			YAKE_ASSERT( wheelTpl.mSteeringGroup < mSteeringGroups.size() || wheelTpl.mSteeringGroup == SG_NO_STEERING_GROUP );

			OdeWheel* pW = new OdeWheel(mpChassis,wheelTpl,PWorld);

			if (wheelTpl.mSteeringGroup != SG_NO_STEERING_GROUP)
			{
				mSteeringGroups[ wheelTpl.mSteeringGroup ].push_back( pW );
			}

			mAxles[ wheelTpl.mAxle ].second.push_back( pW );

			mWheels[ wtp.first ] = pW;
		}
	}
	void GenericVehicle::setSteering( const uint32 sg, const real value )
	{
		//SteeringGroupList::iterator itFind = std::find( mSteeringGroups.begin(), mSteeringGroups.end(), sg );
		//YAKE_ASSERT( itFind != mSteeringGroups.end() )( sg ).debug("steering group not found!");
		//if (itFind == mSteeringGroups.end())
		//	return;
		real newVal = value;
		if (newVal < -1.)
			newVal = -1.;
		else if (newVal > 1.)
			newVal = 1.;
		ConstVectorIterator< Deque<OdeWheel*> > itWheel( mSteeringGroups[ sg ] );
		while (itWheel.hasMoreElements())
		{
			OdeWheel* pW = itWheel.getNext();
			YAKE_ASSERT( pW );
			pW->setSteering( newVal );
		}
	}
	real GenericVehicle::getSteering( const uint32 sg ) const
	{
		YAKE_ASSERT(0 && "NOT IMPLEMENTED")(sg);
		return 0.;
	}
	void createDebugGeometry(	const physics::IActor& actor, 
								graphics::IWorld& GWorld, 
								model::Graphical& graphical,
								model::complex::Model& theModel)
	{
		physics::IShapePtrList shapes = actor.getShapes();
		ConstDequeIterator<physics::IShapePtrList> itShape( shapes );
		while (itShape.hasMoreElements())
		{
			const physics::IShapePtr pShape = itShape.getNext();
			YAKE_ASSERT( pShape );
			graphics::ISceneNode* pSN = GWorld.createSceneNode();
			graphics::IEntity* pE = 0;
			switch (pShape->getType())
			{
			case physics::ST_BOX:
				{
					pE = GWorld.createEntity("box_1x1x1.mesh");
					pSN->attachEntity( pE );
					pSN->setScale( pShape->getPropertyVector3("dimensions") );
				}
				break;
			case physics::ST_SPHERE:
				{
					pE = GWorld.createEntity("sphere_1d.mesh");
					pSN->attachEntity( pE );
					pSN->setScale( Vector3::kUnitScale * pShape->getPropertyReal("radius") );
				}
				break;
			default:
				YAKE_ASSERT("0 && NOT IMPLEMENTED FOR THESE SHAPES!");
				break;
			}
			if (pE)
			{
				graphical.addSceneNode( pSN );

				model::ModelMovableLink* pLink = new model::ModelMovableLink();
				pLink->setSource( pShape );
				pLink->subscribeToPositionChanged( pSN );
				pLink->subscribeToOrientationChanged( pSN );
				theModel.addGraphicsController( pLink );
			}
			else
				delete pSN;
		} // for each shape
	} // createDebugGeometry()
	void GenericVehicle::enableDebugGeometry(graphics::IWorld& GWorld)
	{
		if (mDebugModel)
			delete mDebugModel;
		mDebugModel = new model::complex::Model();
		model::Graphical* pGraphical = new model::Graphical();
		createDebugGeometry( *mpChassis, GWorld, *pGraphical, *mDebugModel );
		mDebugModel->addGraphical( pGraphical );
	}
	void GenericVehicle::disableDebugGeometry()
	{
		YAKE_SAFE_DELETE( mDebugModel );
	}

#if defined(YAKE_VEHICLE_USE_ODE)
	//-----------------------------------------------------
	// Class: OdeWheel
	//-----------------------------------------------------
	OdeWheel::OdeWheel(	physics::IActorPtr chassisObj,
						const VehicleTemplate::WheelTpl& tpl,
						physics::IWorld& PWorld ) :
		mpJoint(0),
		mpWheel(0),
		mRadius(tpl.mRadius),
		mTargetSteer(0),
		mCurrSteer(0),
		mBrakeRatio(real(0.))
	{
		{
			physics::OdeWorld* pW = dynamic_cast<physics::OdeWorld*>( &PWorld );
			YAKE_ASSERT( pW ).debug("Incorrect physics world provider used! ODE expected!");
			if (!pW)
				YAKE_EXCEPT("Incorrect physics world provider used! ODE expected!");
		}

		YAKE_ASSERT( chassisObj );
		mpWheel = PWorld.createActor( physics::ACTOR_DYNAMIC );
		mpWheel->createShape( physics::IShape::SphereDesc( mRadius, tpl.mMaterial ) );
		real mass = tpl.mMassRelativeToChassis ?
							(tpl.mMass * chassisObj->getBody().getMass()) : tpl.mMass;
		mpWheel->setPosition( tpl.mPosition );
		mpWheel->setOrientation( tpl.mOrientation );
		mpWheel->getBody().setMass( mass );

		physics::OdeBody* pBody = static_cast<physics::OdeBody*>( mpWheel->getBodyPtr() );
		dBodyID bodyId = pBody->_getOdeBody()->id();
		dBodySetFiniteRotationMode( bodyId, 1 );

		//if (tpl.mSteeringGroup == SG_NO_STEERING_GROUP)
		//	mpJoint = PWorld.createJoint( physics::IJoint::DescFixed( chassisObj, mpWheel ) );
		//else
		{
			mpJoint = PWorld.createJoint( physics::IJoint::DescHinge2( 
				chassisObj, mpWheel, 
				tpl.mOrientation * Vector3::kUnitY,
				tpl.mOrientation * Vector3::kUnitX,
				tpl.mPosition ) );

			mpJoint->setSpring( tpl.mSuspensionSpring );
			mpJoint->setDamping( tpl.mSuspensionDamping );
			//-> equals: dJointSetHinge2Param( jID, dParamSuspensionERP, _getERPFromSpring() );
			//           and dJointSetHinge2Param( jID, dParamSuspensionCFM, _getCFMFromSpring() );

			if (tpl.mSteeringGroup == SG_NO_STEERING_GROUP)
			{
				mpJoint->setLimits( 0, 0.0, 0.0 );
				mpJoint->setLimits( 1, 0.0, 0.0 );
			}
			else
			{
				mpJoint->setLimits( 0, -0.1, 0.1 );
				mpJoint->setLimits( 1,  0.0, 0.0 );
			}

			//physics::OdeHinge2Joint* pJ = static_cast<physics::OdeHinge2Joint*>( mpJoint );
			//dJointID jID = pJ->_getOdeJoint()->id();

			//dJointSetHinge2Param( jID, dParamSuspensionERP, 0.95 );
			//dJointSetHinge2Param( jID, dParamSuspensionCFM, 0.2 );

			//dJointSetHinge2Param( jID, dParamStopERP, 0.95 );
			//dJointSetHinge2Param( jID, dParamStopCFM, 0.1 );

			//dJointSetHinge2Param( hinges_[i], dParamStopERP, (!(i&2) ? FRONT_TURN_ERP : REAR_TURN_ERP) );
			//dJointSetHinge2Param( hinges_[i], dParamStopCFM, (!(i&2) ? FRONT_TURN_CFM : REAR_TURN_CFM) );
		}
		
		YAKE_ASSERT( mpJoint );


		mPostStepSigConn = PWorld.subscribeToPreStepInternal( boost::bind(&OdeWheel::_onPreStepInternal,this,_1) );
	}
	OdeWheel::~OdeWheel()
	{
		mPostStepSigConn.disconnect();
		YAKE_ASSERT( mpWheel );
		YAKE_ASSERT( mpJoint );
		mpWheel->getCreator()->destroyJoint( mpJoint );
		mpJoint = 0;
		mpWheel->getCreator()->destroyActor( mpWheel );
		mpWheel = 0;
	}
	void OdeWheel::brake(const real ratio)
	{
		mBrakeRatio = ratio;
		if (mBrakeRatio > 1.)
			mBrakeRatio = 1.;
		else if (mBrakeRatio < 0.)
			mBrakeRatio = 0.;
	}
	void OdeWheel::setSteering( const real s )
	{
		mTargetSteer = s;
		if (mTargetSteer < -1.)
			mTargetSteer = -1.;
		else if (mTargetSteer > 1.)
			mTargetSteer = 1.;
	}
	void OdeWheel::_onPreStepInternal( const real dt )
	{
		// interpolate towards target steering
		mCurrSteer = mCurrSteer + /*@todo make this configurable:*/ 1.3 * dt * (mTargetSteer - mCurrSteer);

		YAKE_ASSERT( mpJoint );
		const real maxSteer = 0.7;
		mpJoint->setLimits( 0, maxSteer * (mCurrSteer - 0.05), maxSteer * (mCurrSteer + 0.05) );
	}
	real OdeWheel::getRadius() const
	{
		return mRadius;
	}
	Vector3 OdeWheel::getPosition() const
	{
		YAKE_ASSERT( mpWheel );
		return mpWheel->getPosition();
	}
	Quaternion OdeWheel::getOrientation() const
	{
		YAKE_ASSERT( mpWheel );
		return mpWheel->getOrientation();
	}
	void OdeWheel::_applyDriveTq( const real tq )
	{
		//std::cout << "DTQ=" << tq << "\n";
		//_applyTq( Vector3::kUnitX * tq );

		if (mBrakeRatio > 0.01)
			_applyBrakeTq( Vector3::kUnitX * mBrakeRatio * 1.5 );

		const real targetVel = tq < 0. ? -40 : 40;
		_applyMotor( targetVel, - tq * 0.0075 );
	}
	void OdeWheel::_applyTq( const Vector3& torque )
	{
		mpWheel->getBody().addTorque( mpWheel->getOrientation() * torque );
	}
	void OdeWheel::_applyBrakeTq( const Vector3 & torque )
	{
		Vector3 linVel = mpWheel->getBody().getLinearVelocity();
		Vector3 dir = mpWheel->getOrientation() * Vector3::kUnitX;
		if (dir.dotProduct(linVel) > 0)
		{
			std::cout << "BRK+\n";
			mpWheel->getBody().addLocalTorque( torque );
		}
		else
		{
			std::cout << "BRK-\n";
			mpWheel->getBody().addLocalTorque( -torque );
		}
	}
	void OdeWheel::_applyMotor( real velocity, real fmax )
	{
		if (mpJoint->getType() == physics::JT_HINGE)
		{
			mpJoint->setMotor( 0, velocity, fmax );
			mpJoint->setMotorEnabled( 0, true );
		}
		else if (mpJoint->getType() == physics::JT_HINGE2)
		{
			mpJoint->setMotor( 1, velocity, fmax );
			mpJoint->setMotorEnabled( 1, true );
		}
	}
#endif // YAKE_VEHICLE_USE_ODE
	//-----------------------------------------------------
	// Class: GenericThruster
	//-----------------------------------------------------
	GenericThruster::GenericThruster() : mThrottle(0)
	{
	}
	void GenericThruster::setThrottle( real throttle )
	{
		mThrottle = (throttle < 0.) ? 0. : ((throttle > 1.) ? 1. : throttle);
	}
	real GenericThruster::getThrottle() const
	{
		return mThrottle;
	}
	void GenericThruster::updateSimulation( real timeElapsed )
	{
		setForce( getMinimumForce() + mThrottle * ( getMaximumForce() - getMinimumForce() ) );
	}

	//-----------------------------------------------------
	// Class: GenericMountedThruster
	//-----------------------------------------------------
	GenericMountedThruster::GenericMountedThruster()
	{
	}
	void GenericMountedThruster::onApplyToTargets()
	{
		YAKE_ASSERT(mThruster);
		if (!mThruster)
			return;
		const MountPoint* mp = getMountPoint();
		const Vector3 pos = mp ? mp->getDerivedPosition() : Vector3::kZero;
		const Quaternion rot = mp ? mp->getDerivedOrientation() : Quaternion::kIdentity;
		const Vector3 f = mThruster->getForce() * ( rot * -Vector3::kUnitZ );
		
		ConstDequeIterator< BodyPtrList > itBody( mTargets );
		while (itBody.hasMoreElements())
		{
			physics::IBody* pBody = itBody.getNext();
			pBody->addForceAtPos( f, pos );
		}
	}
} // namespace vehicle
} // namespace yake
