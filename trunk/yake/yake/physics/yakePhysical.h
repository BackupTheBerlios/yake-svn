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
*/
#ifndef YAKE_PHYSICAL_H
#define YAKE_PHYSICAL_H

#include <yake/physics/yakePhysics.h>

#ifndef YAKE_BASE_MOVABLE_H
#include <yake/base/yakeMovable.h>
#endif

#ifndef YAKE_PHYSICSSYSTEM_H
#include <yake/physics/yakePhysicsSystem.h>
#endif

namespace yake {
namespace physics {

	/** High level graphical object in a hierarchy. Can hold any number of graphics entities.
		Graphics entities can be associated with names which are valid within
		a single Physical object.
		\see Complex
		\see RefPtr
	*/
	class YAKE_PHYSICS_API Physical : public base::Movable
	{
	public:
		Physical();
		virtual ~Physical();

		virtual void setPosition( const base::math::Vector3 & position );
		virtual void setOrientation( const base::math::Quaternion & orientation );
		virtual Vector3 getPosition() const;
		virtual Quaternion getOrientation() const;

		virtual void setBaseMovable( base::Movable & pBaseObj );
		virtual void removeBaseMovable();

		/** Add a Physical child object.
			\param pPhysical a weak pointer to a Physical object.
		*/
		virtual void addChild( Physical* pPhysical );
		virtual void addComplex( physics::IComplexObject* pComplex );
		virtual void associate( const base::String & name, physics::IComplexObject* pComplex );
		virtual physics::IComplexObject* getComplex( const base::String & name );

	protected:
		typedef std::list< physics::IComplexObject* > ComplexList;
		ComplexList				mComplex;

		typedef base::templates::AssocVector< base::String, physics::IComplexObject* > ComplexNameMap;
		ComplexNameMap			mComplexNames;

		typedef std::list< Physical* > PhysicalList;
		PhysicalList			mChildren;

		Movable*				mBaseMovable;
	};

}
}

#endif

