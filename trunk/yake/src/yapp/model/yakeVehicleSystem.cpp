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
	YAKE_IMPLEMENT_REGISTRY( IVehicleSystem );
	namespace privateVehSys {
		YAKE_REGISTER_CONCRETE( NativeVehicleSystem );
	}

	//-----------------------------------------------------
	class VehicleCreatorHelperV1
	{
	public:
		VehicleCreatorHelperV1( VehicleTemplate & tpl, IPhysicalVehicleComponent* pPC, model::complex::Model* pComplex, graphics::IWorld* pGWorld );
		bool create();
	protected:
		virtual void onChassis( VehicleTemplate::ChassisTemplate & tpl );
		virtual void onEngine( const size_t index, const VehicleTemplate::EngineTemplate & tpl );
		virtual void onAxle( const size_t index, const VehicleTemplate::AxleTemplate & tpl );
		virtual void onWheel( const size_t index, const VehicleTemplate::WheelTemplate & tpl );
	private:
		model::complex::Model*				mComplex;
		graphics::IWorld*			mGWorld;
		VehicleTemplate						mTpl;
		IPhysicalVehicleComponent*			mPC;
	};	
	//-----------------------------------------------------
	VehicleCreatorHelperV1::VehicleCreatorHelperV1(VehicleTemplate & tpl, IPhysicalVehicleComponent* pPC, model::complex::Model* pComplex, graphics::IWorld* pGWorld ) :
			mTpl( tpl ),
			mComplex( pComplex ),
			mGWorld( pGWorld ),
			mPC( pPC )
	{
		YAKE_ASSERT( mComplex );
		YAKE_ASSERT( mGWorld );
		YAKE_ASSERT( mPC );
	}
	//-----------------------------------------------------
	bool VehicleCreatorHelperV1::create()
	{
		// physical representation of the wheel
		YAKE_ASSERT( mPC );
		mPC->createFromTemplate( mTpl );

		// graphics representation
		onChassis( mTpl.chassisTpl_ );

		for (size_t i=0; i<mTpl.getNumEngines(); ++i)
			onEngine( i, mTpl.getEngine(i) );
		for (size_t i=0; i<mTpl.getNumWheels(); ++i)
			onWheel( i, mTpl.getWheel(i) );

		return true;
	}
	//-----------------------------------------------------
	void VehicleCreatorHelperV1::onChassis( VehicleTemplate::ChassisTemplate & tpl )
	{
		COUTLN("onChassis");
		// graphics representation
		model::Graphical* pGraphical = new model::Graphical();
		YAKE_ASSERT( pGraphical );
		pGraphical->fromDotScene( tpl.gfxDescriptor_, mGWorld );
		mComplex->addGraphical( SharedPtr<model::Graphical>(pGraphical) );

		// tie all root scene nodes to the physics object
		const model::Graphical::SceneNodeList & nodes = pGraphical->getRootSceneNodes();
/*		VectorIterator< model::Graphical::SceneNodeList > it(nodes.begin(), nodes.end());
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pSN = it.getNext();
			YAKE_ASSERT( pSN );

			Movable* pM = mPC->getChassisMovable();
			COUTLN("  binding sn to chassis (pos=" << pM->getPosition().x << ", " << pM->getPosition().y << ", " << pM->getPosition().z << ")");

			model::MovableUpdateController* pMUC = new model::MovableUpdateController();
			YAKE_ASSERT( pMUC );
			mComplex->addController( SharedPtr<model::IObjectController>(pMUC) );
			pMUC->setUpdateSource( mPC->getChassisMovable() );
			pMUC->subscribeToPositionChanged( pSN );
			pMUC->subscribeToOrientationChanged( pSN );
		}*/
	}
	//-----------------------------------------------------
	void VehicleCreatorHelperV1::onEngine( const size_t index, const VehicleTemplate::EngineTemplate & tpl )
	{
	}
	//-----------------------------------------------------
	void VehicleCreatorHelperV1::onAxle( const size_t index, const VehicleTemplate::AxleTemplate & tpl )
	{
	}
	//-----------------------------------------------------
	void VehicleCreatorHelperV1::onWheel( const size_t index, const VehicleTemplate::WheelTemplate & tpl )
	{
		COUTLN("onWheel");

		YAKE_ASSERT( mPC );
		YAKE_ASSERT( mComplex );

		// graphics representation
		model::Graphical* pGraphical = new model::Graphical();
		YAKE_ASSERT( pGraphical );
		pGraphical->fromDotScene( tpl.gfxDescriptor_, mGWorld );
		mComplex->addGraphical( SharedPtr<model::Graphical>(pGraphical) );

		// tie all root scene nodes to the physics object
		const model::Graphical::SceneNodeList & nodes = pGraphical->getRootSceneNodes();
/*		VectorIterator< model::Graphical::SceneNodeList > it(nodes.begin(), nodes.end());
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pSN = it.getNext();
			YAKE_ASSERT( pSN );

			Movable* pM = mPC->getWheelMovable(index);
			COUTLN("  binding sn to wheel (pos=" << pM->getPosition().x << ", " << pM->getPosition().y << ", " << pM->getPosition().z << ")");

			model::MovableUpdateController* pMUC = new model::MovableUpdateController();
			YAKE_ASSERT( pMUC );
			mComplex->addController( SharedPtr<model::IObjectController>(pMUC) );
			pMUC->setUpdateSource( mPC->getWheelMovable(index) );
			pMUC->subscribeToPositionChanged( pSN );
			pMUC->subscribeToOrientationChanged( pSN );
		}*/
	}

	//-----------------------------------------------------
	Vehicle* NativeVehicleSystem::createVehicle(const ::yake::base::String & rTemplate, physics::IWorld* pPWorld, graphics::IWorld* pGWorld)
	{
		YAKE_ASSERT( pPWorld );
		YAKE_ASSERT( pGWorld );
		Vehicle* pVehicle = new Vehicle();
		YAKE_ASSERT( pVehicle );

		::yake::app::model::complex::Model* pModel = new ::yake::app::model::complex::Model();
		YAKE_ASSERT( pModel );

		// create physics component
		pVehicle->setModel( pModel );
		NativePhysicalVehicleComponent* pPhysicsComponent = //create<IPhysicalVehicleComponent>("yake.native");
			new NativePhysicalVehicleComponent( pPWorld );
		YAKE_ASSERT( pPhysicsComponent );
		pVehicle->addComponent( pPhysicsComponent, "core" );

		// load vehicle (physics + graphics)
		_loadModelFromDotVehicle( pModel, rTemplate, pPWorld, pGWorld, pPhysicsComponent );

		// create input component
		// create control component

		pPhysicsComponent->translate( Vector3(0,10,0) );

		return pVehicle;
	}
	//-----------------------------------------------------
	bool NativeVehicleSystem::_loadModelFromDotVehicle(::yake::app::model::complex::Model* pModel,
								const ::yake::base::String & rFN, 
								physics::IWorld* pPWorld,
								graphics::IWorld* pGWorld,
								IPhysicalVehicleComponent* pPC
								)
	{
		using namespace data;
		YAKE_ASSERT( pPWorld );
		YAKE_ASSERT( pGWorld );
		YAKE_ASSERT( pModel );

		// 1. read DOM

		dom::xml::XmlSerializer ser;
		ser.parse(rFN, false);

		// 2. parse DOM - load vehicle template

		VehicleTemplate tpl;
		serializer::vehicle::VehicleSerializerV1 vs1;
		YAKE_ASSERT( ser.getDocumentNode().get() );
		vs1.load( ser.getDocumentNode(), tpl );

		VehicleCreatorHelperV1 vc( tpl, static_cast<NativePhysicalVehicleComponent*>(pPC), pModel, pGWorld );
		bool bRet = vc.create();
		YAKE_ASSERT( bRet );
		if (!bRet)
			return false;

		return true;
	}

}// ns vehicle
} // ns model
} // ns app
} // ns yake

