/**------------------------------------------------------------------------------------
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
#include <yapp/base/yappPCH.h>
#include <yapp/base/yapp.h>
#include <yapp/model/yakeActor.h>
#include <yapp/model/yakeVehicleSystem.h>
#include <yapp/model/yakeEngine.h>
#include <yapp/model/yakeNativeEngine.h>
#include <yapp/model/yakeNativeWheel.h>
#include <yapp/model/yakeVehiclePhysicalComponent.h>
#include <yapp/model/yakeNativeVehiclePhysicalComponent.h>
#include <yapp/model/yakeVehicleSerializerXML.h> // temporarily
#include <yake/data/yakeData.h>

#ifdef YAKE_DEBUG
#	define COUTLN( X ) \
		std::cout << X << std::endl;
#	define COUTLN_V3( X ) \
		std::cout << #X << " (" << X.x << ", " << X.y << ", " << X.z << ")" << std::endl;
#else
#	define COUTLN( X )
#	define COUTLN_V3( X )
#endif

namespace yake {
namespace app {
namespace model {
namespace vehicle {

	//-----------------------------------------------------
	namespace privatePhysCo {
		YAKE_REGISTER_CONCRETE( NativePhysicalVehicleComponent );
	}

	//-----------------------------------------------------
	NativePhysicalVehicleComponent::NativePhysicalVehicleComponent(physics::IWorld* pWorld) : 
		mpPWorld(pWorld),
		mpPhysical(0),
		mpChassis(0)
	{
		YAKE_ASSERT( mpPWorld );
	}
	//-----------------------------------------------------
	NativePhysicalVehicleComponent::~NativePhysicalVehicleComponent()
	{
		_reset();
	}
	//-----------------------------------------------------
	Movable* NativePhysicalVehicleComponent::getChassisMovable()
	{
		return mpChassis;
	}
	//-----------------------------------------------------
	Movable* NativePhysicalVehicleComponent::getWheelMovable(const size_t index)
	{
		return _getWheel( index );
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::_reset()
	{
		YAKE_SAFE_DELETE( mpPhysical );

		// wheels
		templates::VectorIterator< WheelList > itWheel( mWheels.begin(), mWheels.end() );
		while (itWheel.hasMoreElements())
		{
			NativeWheel* pW = itWheel.getNext();
			if (pW)
				delete pW;
		}
		// steer groups
		templates::VectorIterator< SteerGroupList > itSG( mSteerGroups.begin(), mSteerGroups.end() );
		while (itSG.hasMoreElements())
		{
			SteerGroup* pSG = itSG.getNext();
			if (pSG)
				delete pSG;
		}
		// axles
		templates::VectorIterator< AxleList > itAxle( mAxles.begin(), mAxles.end() );
		while (itAxle.hasMoreElements())
		{
			Axle* pA = itAxle.getNext();
			if (pA)
				delete pA;
		}
		// engines
		templates::VectorIterator< EngineList > itEngine( mEngines.begin(), mEngines.end() );
		while (itEngine.hasMoreElements())
		{
			NativeEngine* pEngine = itEngine.getNext();
			if (pEngine)
				delete pEngine;
		}
		// chassis
		YAKE_SAFE_DELETE( mpChassis );
	}
	//-----------------------------------------------------
	bool NativePhysicalVehicleComponent::createFromTemplate( const VehicleTemplate & tpl )
	{
		COUTLN("NativePhysicalVehicleComponent::createFromTemplate");
		_reset();

		// chassis
		YAKE_ASSERT( mpChassis == 0 );
		if (mpChassis)
			return false;
		//mpChassis = mpPWorld->createBox(1,1,1);
		mpChassis = mpPWorld->createEmptyPhysicsObject();
		mpChassis->setBody( mpPWorld->createBody() );
		mpChassis->getBody()->setMass( tpl.chassisTpl_.mass_ );
		mpChassis->setPosition( tpl.chassisTpl_.position_ );

		ConstVectorIterator< VehicleTemplate::GeomTemplateList > geomIt( tpl.chassisTpl_.geoms_.begin(), tpl.chassisTpl_.geoms_.end() );
		while (geomIt.hasMoreElements())
		{
			const VehicleTemplate::GeomTemplate & geomTpl = geomIt.getNext();
			COUTLN(	"  geom type=" << geomTpl.type_ <<
					" pos=" << geomTpl.position_.x << ", " << geomTpl.position_.y << ", " << geomTpl.position_.z <<
					" dim=" << geomTpl.dimensions_.x << ", " << geomTpl.dimensions_.y << ", " << geomTpl.dimensions_.z );
			physics::ICollisionGeometry* pGeom = 0;
			if (geomTpl.type_ == physics::ICollisionGeometry::CGT_BOX)
			{
				const Vector3 & dim = geomTpl.dimensions_;
				pGeom = mpPWorld->createCollisionGeomBox( dim.x, dim.y, dim.z );
				YAKE_ASSERT( pGeom );
			}
			else if (geomTpl.type_ == physics::ICollisionGeometry::CGT_BOX)
			{
				const Vector3 & dim = geomTpl.dimensions_;
				pGeom = mpPWorld->createCollisionGeomBox( dim.x, dim.y, dim.z );
				YAKE_ASSERT( pGeom );
			}
			else
			{
				YAKE_ASSERT( 1==0 ).warning("unsupported geom type!");
			}
			if (!pGeom)
				continue;
			physics::ICollisionGeometry* pTransformGeom = mpPWorld->createCollisionGeomTransform();
			YAKE_ASSERT( pTransformGeom );
			pTransformGeom->tfAttachGeom( pGeom );
			mpChassis->addCollisionGeometry( pTransformGeom );
		}

		YAKE_ASSERT( mpPhysical == 0 );
		mpPhysical = new Physical();
		YAKE_ASSERT( mpPhysical );

		// engines
		for (size_t i=0; i<tpl.getNumEngines(); ++i)
		{
			COUTLN("engine");
			const VehicleTemplate::EngineTemplate& engineTpl = tpl.getEngine(i);

			NativeEngine* pEngine = new NativeEngine();
			if (!pEngine)
				continue;
			mEngines.push_back( pEngine );

			pEngine->setFromTemplate( engineTpl );
		}

		// axles
		for (i=0; i<tpl.getNumAxles(); ++i)
		{
			const VehicleTemplate::AxleTemplate& axleTpl = tpl.getAxle(i);
			COUTLN("  axle " << i << " engine= " << axleTpl.idxEngine_);

			YAKE_ASSERT( _getEngine( axleTpl.idxEngine_ ) != 0 );
			// create axle
			Axle* pAxle = new Axle();
			YAKE_ASSERT( pAxle );
			pAxle->engine = _getEngine(axleTpl.idxEngine_);
			YAKE_ASSERT( pAxle->engine );
			// store axle
			mAxles.push_back( pAxle );
		}

		// steer groups
		for (i=0; i<tpl.getNumSteeringGroups(); ++i)
		{
			COUTLN("  steer group " << i);
			mSteerGroups.push_back( new SteerGroup() );
		}

		// wheels
		for (i=0; i<tpl.getNumWheels(); ++i)
		{
			const VehicleTemplate::WheelTemplate& wheelTpl = tpl.getWheel(i);
			COUTLN("  wheel " << i);

			YAKE_ASSERT( _getAxle( wheelTpl.idxAxle_ ) != 0 );

			// create wheel
			NativeWheel* pW = new NativeWheel(mpPWorld);
			YAKE_ASSERT( pW );
			pW->setPosition( wheelTpl.position_ );
			pW->setRadius( wheelTpl.radius_ );
			// add wheel to steering group
			if (wheelTpl.idxSteeringGroup_ >= 0)
			{
				SteerGroup* pSG = _getSteerGroup(wheelTpl.idxSteeringGroup_);
				YAKE_ASSERT( pSG );
				pSG->wheels.push_back( pW );
			}
			// add wheel to axle
			Axle* pAxle = _getAxle(wheelTpl.idxAxle_);
			YAKE_ASSERT( pAxle );
			pAxle->wheels.push_back( pW );
			// store wheel
			mWheels.push_back( pW );
			// create wheel joint
			SharedPtr<physics::IJoint> pJ( mpPWorld->createJoint( physics::IJoint::JT_HINGE2 ) );
			YAKE_ASSERT( pJ.get() );
			pJ->attach( mpChassis->getBody(), pW->getPhysicsComplex()->getBody() );
			pJ->setAnchor( pW->getPosition() );
			pJ->setAxis1( Vector3::kUnitY ); // steering axis
			pJ->setAxis2( Vector3::kUnitX ); // suspension axis
			pJ->setHighStop(0);
			pJ->setLowStop(0);
			pJ->setSpring( 100 );
			pJ->setDamping( 10 );
			//
			pW->setJoint( pJ );
			mpPhysical->addJoint( pJ );
			//
			pW->setSuspension( wheelTpl.suspensionSpring_, wheelTpl.suspensionDamping_ );
		}

		return true;
	}
	//-----------------------------------------------------
	NativePhysicalVehicleComponent::SteerGroup* 
		NativePhysicalVehicleComponent::_getSteerGroup(const size_t index) const
	{
		if (index >= mSteerGroups.size())
			return 0;
		return mSteerGroups[index];
	}
	//-----------------------------------------------------
	NativeEngine* 
		NativePhysicalVehicleComponent::_getEngine(const size_t index) const
	{
		if (index >= mEngines.size())
			return 0;
		return mEngines[index];
	}
	//-----------------------------------------------------
	NativePhysicalVehicleComponent::Axle* 
		NativePhysicalVehicleComponent::_getAxle(const size_t index) const
	{
		if (index >= mAxles.size())
			return 0;
		return mAxles[index];
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::update(const real timeElapsed)
	{
		//YAKE_ASSERT( mpModel );
		//mpModel->updatePhysics( timeElapsed );
		//mpModel->updateControllers( timeElapsed );
	}
	//-----------------------------------------------------
	IEngine* NativePhysicalVehicleComponent::getEngine(const size_t index)
	{
		return _getEngine(index);
	}
	//-----------------------------------------------------
	NativeWheel* NativePhysicalVehicleComponent::_getWheel(const size_t index) const
	{
		if (index >= mWheels.size())
			return 0;
		return mWheels[index];
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::setWheelMass(const size_t index, const real mass)
	{
		NativeWheel* pWheel = _getWheel( index );
		YAKE_ASSERT( pWheel );
		pWheel->setMass( mass );
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::setWheelRadius(const size_t index, const real r)
	{
		NativeWheel* pWheel = _getWheel( index );
		YAKE_ASSERT( pWheel );
		pWheel->setRadius( r );
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::setWheelPosition(const size_t index, const Vector3 & position)
	{
		NativeWheel* pWheel = _getWheel( index );
		YAKE_ASSERT( pWheel );
		pWheel->setPosition( position );
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::setWheelSuspension(const size_t index, const real stiffness) 
	{
		NativeWheel* pWheel = _getWheel( index );
		YAKE_ASSERT( pWheel );
		YAKE_ASSERT( 1== 0 );
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::setWheelFriction(const size_t index, const real f) 
	{
		NativeWheel* pWheel = _getWheel( index );
		YAKE_ASSERT( pWheel );
		YAKE_ASSERT( 1== 0 );
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::setWheelLateralFriction(const size_t index, const real f)
	{
		NativeWheel* pWheel = _getWheel( index );
		YAKE_ASSERT( pWheel );
		YAKE_ASSERT( 1== 0 );
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::setSteer(const size_t index, const real steerValue) 
	{
		SteerGroup* pSG = _getSteerGroup(index);
		YAKE_ASSERT( pSG );
		pSG->steer = steerValue;
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::setBrake(const size_t index, const real brakeValue)
	{
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::setEngineThrottle(const size_t index, const real throttle)
	{
		NativeEngine* pEngine = _getEngine(index);
		YAKE_ASSERT( pEngine );
		pEngine->setThrottle( throttle );
	}
	//-----------------------------------------------------
	void NativePhysicalVehicleComponent::translate( const Vector3 & d )
	{
		YAKE_ASSERT( mpChassis );
		mpChassis->translate( d );
		VectorIterator< WheelList > itW( mWheels.begin(), mWheels.end() );
		while (itW.hasMoreElements())
			itW.getNext()->translate( d );
	}

} // ns vehicle
} // ns model
} // ns app
} // ns yake

