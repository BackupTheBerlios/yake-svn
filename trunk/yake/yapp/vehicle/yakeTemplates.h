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
#ifndef YAKE_VEHICLE_TEMPLATES_H
#define YAKE_VEHICLE_TEMPLATES_H

#include "yakePrerequisites.h"

namespace yake {
namespace vehicle {

	const size_t MPID_NO_PARENT = 0xffffffff;
	struct VehicleTemplate
	{
		struct MountPointTpl
		{
			Vector3		mPosition;
			Quaternion	mOrientation;
			Vector3		mDirection;
			size_t		mParentMountPointIdx;
			bool		mUseDirection;
			MountPointTpl(
				const Vector3& pos = Vector3::kZero,
				const Quaternion rot = Quaternion::kIdentity,
				size_t parentMountPoint = MPID_NO_PARENT) :
				mPosition(pos),
				mOrientation(rot),
				mParentMountPointIdx(parentMountPoint),
				mUseDirection(false)
			{}
			MountPointTpl(
				const Vector3& pos,
				const Vector3& dir,
				size_t parentMountPoint = MPID_NO_PARENT) :
				mPosition(pos),
				mDirection(dir),
				mParentMountPointIdx(parentMountPoint),
				mUseDirection(true)
			{}
		};
		typedef Deque< MountPointTpl > MountPointTplList;
		enum GearMode
		{
			GM_FORWARD,
			GM_NEUTRAL,
			GM_REVERSE
		};
		struct GearTpl
		{
			real				ratio_;
			GearMode			mode_;
			GearTpl() : ratio_(1.), mode_(GM_FORWARD) {}
		};
		typedef Vector<GearTpl> GearTplList;
		struct EngineTpl
		{
			virtual ~EngineTpl() {}
		};
		struct ThrusterTpl : public EngineTpl
		{
			real				minForce;
			real				maxForce;
			size_t				mountPtIdx;
			ThrusterTpl(real minF = 0., real maxF = 1., size_t mtPt = 0) : minForce(minF), maxForce(maxF), mountPtIdx(mtPt) {}
		};
		struct CarEngineTpl : public EngineTpl
		{
			real				rpmMin_;
			real				rpmMax_;
			real				rpmRedline_;
			real				rpmDieOff_;
			real				rpmRise_;
			real				minShiftingTime_;
			GearTplList			gears_;
			//Curve*			torqueCurve_;
			real				differentialRatio;
			CarEngineTpl() :
				rpmMin_(real(0.)),
				rpmMax_(real(8000.)),
				rpmRedline_(real(6000.)),
				rpmDieOff_(real(800.)),
				rpmRise_(real(2000.)),
				minShiftingTime_(real(0.15)),
				differentialRatio(real(3.14))
			{}
		};

		typedef Deque<EngineTpl*> EngineTplList;

		typedef Deque<physics::IShape::Desc*> ShapeTplList;

		struct ChassisTpl
		{
			Vector3			mPosition; // initial position
			real			mMass;
			ShapeTplList	mChassisShapes;
			String			mGfxReference; // e.g. dotScene file
			ChassisTpl() : 
				mPosition(Vector3::kZero),
				mMass(real(1.6))
			{}
		};

		struct WheelTpl
		{
			Vector3			mPosition;
			Quaternion		mOrientation;
			uint32			mSteeringGroup;
			real			mRadius;
			real			mMass;
			bool			mMassRelativeToChassis;
			real			mSuspensionSpring;
			real			mSuspensionDamping;
			String			mGfxReference; // e.g. dotScene file
			String			mGfxReferenceType; // e.g. "dotscene"
			WheelTpl() :
				mPosition(Vector3::kZero),
				mOrientation(Quaternion::kIdentity),
				mSteeringGroup(0),
				mRadius(real(0.8)),
				mMass(real(0.015)),
				mMassRelativeToChassis(true),
				mSuspensionSpring(real(1)),
				mSuspensionDamping(real(0.9))
			{}
		};

		typedef Deque< WheelTpl > WheelTplList;

		//

		ChassisTpl		mChassis;
		MountPointTplList	mMountPoints;
		EngineTplList	mEngines;
		uint32			mSteeringGroups;
		WheelTplList	mWheels;

		VehicleTemplate() : mSteeringGroups(0)
		{}
		~VehicleTemplate()
		{
			ConstVectorIterator< EngineTplList > itEngine( mEngines );
			while (itEngine.hasMoreElements())
				delete itEngine.getNext();
			mEngines.clear();
			ConstVectorIterator< ShapeTplList > itShape( mChassis.mChassisShapes );
			while (itShape.hasMoreElements())
				delete itShape.getNext();
			mChassis.mChassisShapes.clear();
		}
	};

} // namespace vehicle
} // namespace yake


#endif
