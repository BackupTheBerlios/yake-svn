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
#ifndef YAKE_APP_MODEL_VEHICLE_TEMPLATE_H
#define YAKE_APP_MODEL_VEHICLE_TEMPLATE_H

#include <yapp/base/yappPrerequisites.h>

namespace yake {
namespace app {
namespace model {
namespace vehicle {

	struct VehicleTemplate
	{
	public:
		VehicleTemplate();
		~VehicleTemplate();

		struct GeomTemplate
		{
			Vector3											position_;
			Vector3											dimensions_;
			physics::ShapeType								type_;
		};
		typedef Deque<GeomTemplate> GeomTemplateList;

		enum GearMode
		{
			GM_FORWARD,
			GM_NEUTRAL,
			GM_REVERSE
		};
		struct GearTemplate
		{
			real				ratio_;
			GearMode			mode_;
		};
		typedef Vector<GearTemplate> GearTemplateList;
		struct EngineTemplate
		{
			virtual ~EngineTemplate() {}
		};
		struct CarEngineTemplate : public EngineTemplate
		{
			real				rpmMin_;
			real				rpmMax_;
			real				rpmRedline_;
			real				rpmDieOff_;
			real				rpmRise_;
			real				minShiftingTime_;
			GearTemplateList	gears_;
			//Curve*			torqueCurve_;
			real				differentialRatio;
		};
		size_t getNumEngines() const;
		const EngineTemplate& getEngine(const size_t index) const;
		void addEngine( const EngineTemplate & tpl);

		struct AxleTemplate
		{
			int32				idxEngine_;
			real				efficiency_;
		};
		size_t getNumAxles() const;
		const AxleTemplate& getAxle(const size_t index) const;
		void addAxle( const AxleTemplate & tpl);

		size_t getNumSteeringGroups() const;
		void addSteeringGroup();

		struct WheelTemplate
		{
			int32		idxAxle_;
			int32		idxSteeringGroup_;
			real		radius_;
			real		mass_;
			bool		bMassIsRelativeToChassisMass_;
			String		gfxDescriptor_;
			String		gfxDescriptorType_;
			Vector3		position_;
			real		suspensionSpring_;
			real		suspensionDamping_;
		};
		size_t getNumWheels() const;
		const WheelTemplate& getWheel(const size_t index) const;
		void addWheel( const WheelTemplate & tpl);

		struct ChassisTemplate
		{
			String				name_;
			real				mass_;
			String				gfxDescriptor_;
			Vector3				position_;
			Vector3				dimensions_;
			GeomTemplateList	geoms_;
		};
		ChassisTemplate	chassisTpl_;
	private:
		GearTemplateList	mGears;
		
		typedef Vector<EngineTemplate> EngineTemplateList;
		EngineTemplateList	mEngines;

		typedef Vector<AxleTemplate> AxleTemplateList;
		AxleTemplateList	mAxles;

		typedef Vector<WheelTemplate> WheelTemplateList;
		WheelTemplateList	mWheels;

		uint32				mNumSteerGroups;
	};

}
}
}
}

#endif