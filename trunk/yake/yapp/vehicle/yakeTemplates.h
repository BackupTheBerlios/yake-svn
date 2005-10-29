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

	const String MPID_NO_PARENT = String("");
	const uint32 SG_NO_STEERING_GROUP = 0xffffffff;
	struct VehicleTemplate
	{
		struct MountPointTpl;
		typedef AssocVector< String, MountPointTpl > MountPointTplList;
		struct MountPointTpl
		{
			Vector3				mPosition;
			Quaternion			mOrientation;
			Vector3				mDirection;
			bool				mUseDirection;
			MountPointTplList	mChildren;
			MountPointTpl(
				const Vector3& pos = Vector3::kZero,
				const Quaternion rot = Quaternion::kIdentity) :
				mPosition(pos),
				mOrientation(rot),
				mUseDirection(false)
			{}
			MountPointTpl(
				const Vector3& pos,
				const Vector3& dir) :
				mPosition(pos),
				mDirection(dir),
				mUseDirection(true)
			{}
			MountPointTpl& addMountPoint(
				const Vector3& pos = Vector3::kZero,
				const Quaternion rot = Quaternion::kIdentity);
			MountPointTpl& addMountPoint(
				const Vector3& pos,
				const Vector3& dir);
		};
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
			GearTpl(const real r = 1., const GearMode m = GM_FORWARD) : ratio_(r), mode_(m) {}
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
			String				mountPt;
			ThrusterTpl(real minF = 0., real maxF = 1., const String& mtPt = "") : minForce(minF), maxForce(maxF), mountPt(mtPt) {}
		};
		struct CarEngineTpl : public EngineTpl
		{
			uint32				axle_;
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
				differentialRatio(real(3.14)),
				axle_(0)
			{}
		};

		typedef AssocVector<String,EngineTpl*> EngineTplList;

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
			uint32			mAxle;
			Vector3			mPosition;
			Quaternion		mOrientation;
			uint32			mSteeringGroup;
			real			mRadius;
			real			mMass;
			bool			mMassRelativeToChassis;
			real			mSuspensionSpring;
			real			mSuspensionDamping;
			//String			mGfxReference; // e.g. dotScene file
			//String			mGfxReferenceType; // e.g. "dotscene"
			String			mMaterial;
			WheelTpl(
				const Vector3& position = Vector3::kZero,
				const Quaternion& orientation = Quaternion::kIdentity,
				const real radius = real(0.2),
				const real mass = real(0.018),
				const bool massRelativeToChassis = true,
				const uint32 steeringGrp = SG_NO_STEERING_GROUP,
				const real suspensionSpring = real(30),
				const real suspensionDamping = real(3),
				const String& material = "",
				const uint32 axle = 0
				) :
				mPosition(position),
				mOrientation(orientation),
				mSteeringGroup(steeringGrp),
				mRadius(radius),
				mMass(mass),
				mMassRelativeToChassis(massRelativeToChassis),
				mSuspensionSpring(suspensionSpring),
				mSuspensionDamping(suspensionDamping),
				mMaterial(material),
				mAxle(axle)
			{}
		};

		typedef AssocVector< String, WheelTpl > WheelTplList;

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
				delete itEngine.getNext().second;
			mEngines.clear();
			ConstVectorIterator< ShapeTplList > itShape( mChassis.mChassisShapes );
			while (itShape.hasMoreElements())
				delete itShape.getNext();
			mChassis.mChassisShapes.clear();
		}
		//VehicleTemplate* clone() const;
	};

} // namespace vehicle
} // namespace yake


#endif
