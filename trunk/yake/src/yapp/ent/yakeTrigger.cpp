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
#include <yapp/ent/yakePCH.h>
#include <yapp/ent/yakeCommon.h>
#include <yapp/ent/yakeEvent.h>
#include <yapp/ent/yakeMessaging.h>
#include <yapp/ent/yakeObject.h>
#include <yapp/ent/yakeEntityMachine.h>
#include <yapp/ent/yakeEntityComponent.h>
#include <yapp/ent/yakeEntity.h>
#include <yapp/ent/yakeTrigger.h>
#include <yapp/ent/yakeSim.h>

namespace yake {
namespace ent {
	namespace geom {
		Volume::Volume()
		{}
		VolumeSet::VolumeSet()
		{}
		VolumeSet::~VolumeSet()
		{
			_removeAndDestroyAllVolumes();
		}
		void VolumeSet::_removeAndDestroyAllVolumes()
		{
			ConstVectorIterator< VolumePtrList > itV( mVolumes );
			while (itV.hasMoreElements())
				delete itV.getNext();
			mVolumes.clear();
		}
		void VolumeSet::addVolume( Volume* pV )
		{
			YAKE_ASSERT( pV ).warning("Tried to add 0 ptr volume.");
			if (pV)
				mVolumes.push_back( pV );
		}
		Volume* VolumeSet::removeVolume( Volume* pV )
		{
			VolumePtrList::iterator itFind = std::find( mVolumes.begin(), mVolumes.end(), pV );
			YAKE_ASSERT( itFind != mVolumes.end() );
			if (itFind == mVolumes.end())
				return pV;
			mVolumes.erase( itFind );
		}
		void VolumeSet::translate(const Vector3&)
		{
			YAKE_ASSERT( 1==0 ).debug("NOT IMPLEMENTED!");
		}
		bool VolumeSet::contains(const Point3&) const
		{
			YAKE_ASSERT( 1==0 ).debug("NOT IMPLEMENTED!");
			return false;
		}
	} // namespace  geom
	

	YAKE_IMPLEMENT_REGISTRY( TriggerCondition );

	TriggerCondition::~TriggerCondition()
	{
	}

	YAKE_REGISTER_CONCRETE( SpatialTriggerCondition );

	YAKE_REGISTER_CONCRETE( TimedTriggerCondition );
	void TimedTriggerCondition::onFrame()
	{
		YAKE_ASSERT( 1==0 ).debug("NOT IMPLEMENTED!");
	}
	void TimedTriggerCondition::onTick()
	{
		YAKE_ASSERT( 1==0 ).debug("NOT IMPLEMENTED!");
	}

	YAKE_DEFINE_ENTITY_1( Trigger, Entity )

	Trigger::Trigger()
	{
	}
	Trigger::~Trigger()
	{
		mConds.clear();
	}
	SharedPtr<TriggerCondition> Trigger::addCondition(SharedPtr<TriggerCondition> pCond)
	{
		YAKE_ASSERT( pCond.get() ).warning("Cannot add a null condition!");

		if (pCond.get())
			mConds.push_back(pCond);
	
		return pCond;
	}
	SharedPtr<TriggerCondition> Trigger::removeCondition(SharedPtr<TriggerCondition> pCond)
	{
		ConditionPtrList::iterator itFind = std::find( mConds.begin(), mConds.end(), pCond );
		if (itFind != mConds.end())
			mConds.erase( itFind );
		return pCond;
	}
	void Trigger::addVolume( geom::Volume* pVolume )
	{
		mVolume.addVolume( pVolume );
	}
	geom::Volume* Trigger::removeVolume( geom::Volume* pVolume )
	{
		return mVolume.removeVolume( pVolume );
	}
	void Trigger::onTick()
	{
		Entity::onTick();
		onUpdateConditionsOncePerTick();
	}
	void Trigger::onUpdateConditionsOncePerFrame()
	{
		ConstVectorIterator< ConditionPtrList > itC( mConds );
		while (itC.hasMoreElements())
			itC.getNext()->onFrame();
	}
	void Trigger::onUpdateConditionsOncePerTick()
	{
		ConstVectorIterator< ConditionPtrList > itC( mConds );
		while (itC.hasMoreElements())
			itC.getNext()->onTick();
	}

} // namespace yake
} // namespace ent
