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
#include <yapp/base/yappPCH.h>
#include <yapp/base/yapp.h>
#include <yapp/model/yakeVehiclePhysicalComponent.h>
//#include <yake/samples/common/yakeTrailGeometryCreator.h>

namespace yake {
namespace model {
namespace vehicle {

	//-----------------------------------------------------
	Vehicle::Vehicle() : mComplex(0), mPhysics(0)
	{
	}
	//-----------------------------------------------------
	Vehicle::~Vehicle()
	{
		YAKE_SAFE_DELETE( mComplex );
		YAKE_SAFE_DELETE( mPhysics );
	}
	//-----------------------------------------------------
	void Vehicle::setPhysicalComponent(IPhysicalVehicleComponent* pPhysical)
	{
		YAKE_ASSERT( !mPhysics );
		mPhysics = pPhysical;
	}
	//-----------------------------------------------------
	void Vehicle::update(real timeElapsed)
	{
		if (mPhysics)
			mPhysics->update(timeElapsed);
		if (mComplex)
		{
			mComplex->updatePhysics(timeElapsed);
			mComplex->updateGraphics(timeElapsed);
		}
	}
	//-----------------------------------------------------
	//void Vehicle::onAct()
	//{
	//	IPhysicalVehicleComponent* pPC = 
	//		static_cast<IPhysicalVehicleComponent*>(getComponent("core"));
	//	if (!pPC)
	//		return;
	//	pPC->update(real(0.01)); //@todo use elapsed time!

	//	YAKE_ASSERT( mComplex );
	//	mComplex->updatePhysics( real(0.01) ); //@todo use elapsed time!
	//	mComplex->updateGraphics( real(0.01) ); //@todo use elapsed time!
	//}
	//-----------------------------------------------------
	void Vehicle::setModel( complex::Model* pComplex )
	{
		YAKE_ASSERT( pComplex );
		mComplex = pComplex;
	}


}
}
}
