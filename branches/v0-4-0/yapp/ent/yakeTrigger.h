/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#ifndef YAKE_ENT_TRIGGER_H
#define YAKE_ENT_TRIGGER_H

namespace yake {
namespace ent {

	class YAKE_ENT_API TriggerCondition
	{
		YAKE_DECLARE_REGISTRY_0( TriggerCondition, String );
	private:
	protected:
		virtual void onFire() {}
	public:
		virtual ~TriggerCondition();

		//void fire();
		// events:
		//	"onFire()"
		Event& getEvent_fire();

		bool isToBeExecutedOncePerFrame() const;
		bool isToBeExecutedOncePerTick() const;

		virtual void onTick() {}
		virtual void onFrame() {}
	};
	class YAKE_ENT_API TimedTriggerCondition : public TriggerCondition
	{
		YAKE_DECLARE_CONCRETE( TimedTriggerCondition, "condition.timed" );

	public:
		void setTargetSimTime( const simtime );
		void setTargetTime( const real );

		virtual void onTick();
		virtual void onFrame();

	private:
		real		mTime;
	};
	namespace geom {
		class Volume
		{
			Volume(const Volume&);
		protected:
			Volume();
		public:
			virtual ~Volume() {}
			virtual bool contains(const Point3&) const = 0;
			virtual void translate(const Vector3&) = 0;
		};
		class Sphere : public Volume
		{
		public:
			Sphere(const Point3& center, const real radius = real(1.));
			Sphere(const real radius = real(1.));
			Sphere(const Sphere&);
			Sphere& operator = (const Sphere&);

			virtual bool contains(const Point3&) const;
			virtual void translate(const Vector3&) = 0;

			void setRadius(const real radius);
			real radius() const;
			void setCenter(const Point3& center);
			Point3 center() const;
			void center(Point3& center) const;
		private:
			Point3	mCenter;
			real	mRadius;
		};
		class VolumeSet : public Volume
		{
		public:
			VolumeSet();
			virtual ~VolumeSet();
			VolumeSet(const VolumeSet&);
			VolumeSet& operator = (const VolumeSet&);

			virtual bool contains(const Point3&) const;
			virtual void translate(const Vector3&);

			void addVolume( Volume* );
			Volume* removeVolume( Volume* );
		private:
			void _removeAndDestroyAllVolumes();
		private:
			typedef Deque<Volume*> VolumePtrList;
			VolumePtrList	mVolumes;
		};
	} // namespace geom
	class YAKE_ENT_API SpatialTriggerCondition : public TriggerCondition
	{
		YAKE_DECLARE_CONCRETE( SpatialTriggerCondition, "condition.spatial" );
		// (additional) events:
		//	"onFire(Entity*)"
		//	"onEnterVolume(Entity*)"
		//	"onExitVolume(Entity*)"
	public:
		// PointEnter: good for static point and moving volume trigger.
		void addPointEnterCondition( geom::Volume*, const Point3& );
		void addPointExitCondition( geom::Volume*, const Point3& );

		// MoveableEnter: good for testing a movable object against a volume trigger.
		void addMovableEnterCondition( geom::Volume*, Movable* );
		void addMovableExitCondition( geom::Volume*, Movable* );
	private:
	};
	class YAKE_ENT_API Trigger : public Entity
	{
		YAKE_DECLARE_ENTITY(Trigger)
	public:
		OBJECT_PROPS_BEGIN(Trigger)
			OBJECT_PROP("position", Point3, Point3(0,0,0))
		OBJECT_PROPS_END()
		//events:
		//	"onFire(Entity*,TriggerCondition*)"

		SharedPtr<TriggerCondition> addCondition(SharedPtr<TriggerCondition>);
		SharedPtr<TriggerCondition> removeCondition(SharedPtr<TriggerCondition>);

		void addVolume( geom::Volume* );
		geom::Volume* removeVolume( geom::Volume* );
	protected:
		void onUpdateConditionsOncePerFrame();
		void onUpdateConditionsOncePerTick();
	protected:
		Trigger();
		virtual ~Trigger();
		virtual void onTick();
	private:
		typedef Deque<SharedPtr<TriggerCondition> > ConditionPtrList;
		ConditionPtrList	mConds;
		geom::VolumeSet		mVolume;
	};

} // namespace yake
} // namespace ent
#endif
