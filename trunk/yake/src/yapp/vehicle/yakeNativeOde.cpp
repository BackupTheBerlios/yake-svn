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
#	include <ode/ode.h>
#	include <ode/odecpp.h>
#	include <ode/objects.h>
#	include <yake/plugins/physicsODE/OdeJoint.h>
#	include <yake/plugins/physicsODE/OdeBody.h>
#	include <yake/plugins/physicsODE/OdeWorld.h>
#	pragma comment(lib, "physicsODE.lib")
#	ifdef YAKE_DEBUG
#		pragma comment(lib, "ode.lib")
#	else
#		pragma comment(lib, "ode.lib")
#	endif
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
	IVehicle* GenericVehicleSystem::create(const VehicleTemplate& tpl, physics::IWorld& PWorld, model::Physical& physModel )
	{
		GenericVehicle* pV = new GenericVehicle();
		pV->_create( tpl, PWorld, physModel );
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
	IVehicle* GenericVehicleSystem::create(const String& tplId, physics::IWorld& PWorld, model::Physical& physModel)
	{
		const VehicleTemplate* tpl = getTemplate( tplId );
		YAKE_ASSERT( tpl );
		if (!tpl)
			return 0;
		return create(*tpl,PWorld,physModel);
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

		// NB Do not destroy the mpChassis actor!
		//    This object is owned by model::Physical and will be destroyed by it, too!
		//    [mpChassis->getCreator()->destroyActor( mpChassis );]
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
	}
	void GenericVehicle::_applyDriveTorqueToAxles( real timeElapsed )
	{
		ConstDequeIterator< AxleList > itAxle( mAxles );
		while (itAxle.hasMoreElements())
		{
			CarEngineWheelsPair cewp = itAxle.getNext().second;
			if (!cewp.first) // no engine attached to axle.
				continue;

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
	math::Vector3 GenericVehicle::getChassisPosition() const
	{
		YAKE_ASSERT( mpChassis );
		return mpChassis->getPosition();
	}
	math::Quaternion GenericVehicle::getChassisOrientation() const
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
	void GenericVehicle::_create(const VehicleTemplate& tpl, physics::IWorld& PWorld, model::Physical& physModel )
	{
		YAKE_LOG( "Creating vehicle from template..." );
		if ( tpl.mChassis.mPhysicsBody != "" ) // i.e. chassis is defined as reference to physical body
		{
		    YAKE_LOG( "External physical body found. Searching provided model for it..." );

		    // searching for actor in provided model
		    physics::IActorPtr actor = physModel.getActor( tpl.mChassis.mPhysicsBody );

		    YAKE_ASSERT( actor != NULL ).error( "Actor '" + tpl.mChassis.mPhysicsBody 
			    +  "' was not found in physical model! Are you sure you provided the right model???" );

		    YAKE_LOG( "Found. OK." );
		    // initial position is determined by body position
		    mpChassis = actor;
		}
		else // chassis is defined in .vehicle itself
		{
		    YAKE_LOG( "External physical body not found. Looking for body definition in .vehicle file." );
		    // chassis
		    mpChassis = PWorld.createActor( physics::ACTOR_DYNAMIC );

		    // adding chassis to physical model here...
		    // TODO: naming convention for chassis. 
		    // No more than one chassis per model now ;)
		    physModel.addActor( mpChassis, "vehicle_chassis" );

		    mpChassis->setPosition( tpl.mChassis.mPosition );
		    //mpChassis->setOrientation( tpl.mChassis.mOrientation );
		    ConstDequeIterator< VehicleTemplate::ShapeTplList > itShapeTpl( tpl.mChassis.mChassisShapes );
		    while (itShapeTpl.hasMoreElements())
		    {
				mpChassis->createShape( *itShapeTpl.getNext() );
		    }
		    mpChassis->getBody().setMass( tpl.mChassis.mMass );
		}
		
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
					IThruster* pThruster = NULL;
					if ( thrusterTpl->type == "linear" )
					{
					    GenericLinearThruster* pEngine = new GenericLinearThruster();
					    pThruster = pEngine; 
					    mEngines[ engineTplEntry.first ] = pEngine;
					    pEngine->setMinimumForce( thrusterTpl->minForce );
					    pEngine->setMaximumForce( thrusterTpl->maxForce );
					    pEngine->setInputSignal(0.);
					    subscribeToUpdateEngineSimulation(
						    boost::bind( &GenericLinearThruster::updateSimulation, pEngine, _1 ) );
					}
					else if ( thrusterTpl->type == "gain" )
					{
					    GenericThruster* pEngine = new GenericThruster();
					    pThruster = pEngine; 
					    mEngines[ engineTplEntry.first ] = pEngine;
					    pEngine->setGain( thrusterTpl->gain );
					    subscribeToUpdateEngineSimulation(
						    boost::bind( &GenericThruster::updateSimulation, pEngine, _1 ) );
					}

					// the thruster is attached to a mount point, create wrapper:
					if (thrusterTpl->mountPt != MPID_NO_PARENT)
					{
						GenericMountedThruster* pMounted = new GenericMountedThruster();
						pMounted->setThruster( pThruster );

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

			YAKE_ASSERT( mAxles[ wheelTpl.mAxle ].first ).warning("Need an engine for the axle?");
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
								model::Model& theModel)
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
					pSN->setScale( math::Vector3::kUnitScale * pShape->getPropertyReal("radius") );
				}
				break;
			default:
				YAKE_ASSERT("0 && NOT IMPLEMENTED FOR THESE SHAPES!");
				break;
			}
			if (pE)
			{
				graphical.addSceneNode( pSN, uniqueName::create("yake::vehicle_dbg_geom_") );

				model::ModelMovableLink* pLink = new model::ModelMovableDirectLink();
				pLink->setSource( pShape );
				pLink->subscribeToPositionChanged( pSN );
				pLink->subscribeToOrientationChanged( pSN );
				theModel.addLink( pLink );
			}
			else
				delete pSN;
		} // for each shape
	} // createDebugGeometry()
	void GenericVehicle::enableDebugGeometry(graphics::IWorld& GWorld)
	{
		if (mDebugModel)
			delete mDebugModel;
		mDebugModel = new model::Model();
		mDebugModel->setName( uniqueName::create("yake::vehicle_dbg_") );
		model::Graphical* pGraphical = new model::Graphical();
		createDebugGeometry( *mpChassis, GWorld, *pGraphical, *mDebugModel );
		mDebugModel->addComponent( pGraphical, "gfx" );
	}
	void GenericVehicle::disableDebugGeometry()
	{
		YAKE_SAFE_DELETE( mDebugModel );
	}
	void GenericVehicle::setPosition(const Vector3& position)
	{
		YAKE_ASSERT( mpChassis );
		const Vector3 delta = position - this->getChassisPosition();
		this->translate(delta);
	}
	void GenericVehicle::translate(const Vector3& delta)
	{
		YAKE_ASSERT( mpChassis );
		mpChassis->translate( delta );
		for (WheelList::const_iterator it = mWheels.begin(); it != mWheels.end(); ++it)
			it->second->translate(delta);
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
		mBrakeRatio(real(0.)),
		mSkid(0),
		mpChassis( chassisObj )
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
				tpl.mOrientation * math::Vector3::kUnitY,
				tpl.mOrientation * math::Vector3::kUnitX,
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


		mPreStepSigConn = PWorld.subscribeToPreStepInternal( boost::bind(&OdeWheel::_onPreStepInternal,this,_1) );
		mPostStepSigConn = PWorld.subscribeToPostStep( boost::bind(&OdeWheel::_onPostStep,this,_1) );
	}
	OdeWheel::~OdeWheel()
	{
		mPostStepSigConn.disconnect();
		mPreStepSigConn.disconnect();
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
	void OdeWheel::_onPostStep( const real dt )
	{
		YAKE_ASSERT( mpChassis );
		YAKE_ASSERT( mpWheel );
		const math::Vector3 chassisDir = mpChassis->getOrientation() * math::Vector3::kUnitZ;
		const math::Vector3 wheelMovementDir = mpWheel->getBody().getLinearVelocity().normalisedCopy();
		mSkid = 1. - chassisDir.dotProduct( wheelMovementDir );
		if (mSkid < 0)
			mSkid = 0.;
	}
	real OdeWheel::getRadius() const
	{
		return mRadius;
	}
	void OdeWheel::setPosition(const math::Vector3& pos)
	{
		YAKE_ASSERT( mpWheel );
		mpWheel->setPosition( pos );
	}
	void OdeWheel::setOrientation(const math::Quaternion& o)
	{
		YAKE_ASSERT( mpWheel );
		mpWheel->setOrientation( o );
	}
	math::Vector3 OdeWheel::getPosition() const
	{
		YAKE_ASSERT( mpWheel );
		return mpWheel->getPosition();
	}
	math::Vector3 OdeWheel::getDerivedPosition() const
	{
		YAKE_ASSERT( mpWheel );
		return mpWheel->getDerivedPosition();
	}
	math::Quaternion OdeWheel::getOrientation() const
	{
		YAKE_ASSERT( mpWheel );
		return mpWheel->getOrientation();
	}
	math::Quaternion OdeWheel::getDerivedOrientation() const
	{
		YAKE_ASSERT( mpWheel );
		return mpWheel->getDerivedOrientation();
	}
	void OdeWheel::_applyDriveTq( const real tq )
	{
		//std::cout << "DTQ=" << tq << "\n";
		//_applyTq( math::Vector3::kUnitX * tq );

		if (mBrakeRatio > 0.01)
			_applyBrakeTq( math::Vector3::kUnitX * mBrakeRatio * 1.5 );

		const real targetVel = tq < 0. ? -40 : 40;
		_applyMotor( targetVel, - tq * 0.0075 );
	}
	void OdeWheel::_applyTq( const math::Vector3& torque )
	{
		mpWheel->getBody().addTorque( mpWheel->getOrientation() * torque );
	}
	void OdeWheel::_applyBrakeTq( const math::Vector3 & torque )
	{
		math::Vector3 linVel = mpWheel->getBody().getLinearVelocity();
		math::Vector3 dir = mpWheel->getOrientation() * math::Vector3::kUnitX;
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
	real OdeWheel::getSkid() const
	{
		return mSkid;
	}
#endif // YAKE_VEHICLE_USE_ODE
	//-----------------------------------------------------
	// Class: GenericThruster
	//-----------------------------------------------------
	GenericThruster::GenericThruster() : mVoltage( 0 ), mGain( 1.0 )
	{
	}
	void GenericThruster::setInputSignal( real voltage )
	{
	    mVoltage = voltage;
	}
	real GenericThruster::getInputSignal() const
	{
	    return mVoltage;
	}
	void GenericThruster::updateSimulation( real timeElapsed )
	{
	   setForce( mVoltage * mGain ); 
	}
	void GenericThruster::setGain( real gain )
	{
	    mGain = gain;
	}
	real GenericThruster::getGain() const
	{
	    return mGain;
	}
	
	//-----------------------------------------------------
	// Class: GenericLinearThruster
	//-----------------------------------------------------
	GenericLinearThruster::GenericLinearThruster() : mVoltage( 0 )
	{
	}
	void GenericLinearThruster::setInputSignal( real voltage )
	{
		mVoltage = voltage;
	}
	real GenericLinearThruster::getInputSignal() const
	{
		return mVoltage;
	}
	void GenericLinearThruster::updateSimulation( real timeElapsed )
	{
	    real abs_force = getMinimumForce() + math::Math::Abs( mVoltage ) * ( getMaximumForce() - getMinimumForce() );
	    
	    if ( abs_force > getMaximumForce() )
		abs_force = getMaximumForce();
	    
	    setForce( math::Math::Sign( mVoltage )*abs_force );
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
		
		if ( mThruster->getForce() == 0 )
		    return;

		const MountPoint* mp = getMountPoint();
		const math::Vector3 pos = mp ? mp->getDerivedPosition() : math::Vector3::kZero;
		const math::Quaternion rot = mp ? mp->getDerivedOrientation() : math::Quaternion::kIdentity;
		const math::Vector3 f = mThruster->getForce() * ( rot * -math::Vector3::kUnitZ );

		ConstDequeIterator< BodyPtrList > itBody( mTargets );
		while (itBody.hasMoreElements())
		{
			physics::IBody* pBody = itBody.getNext();
			
			pBody->addForceAtPos( f, pos );
		}
	}
} // namespace vehicle
} // namespace yake

