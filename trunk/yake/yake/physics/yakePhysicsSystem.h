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
#ifndef YAKE_PHYSICS_PHYSICSSYSTEM_H
#define YAKE_PHYSICS_PHYSICSSYSTEM_H

#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#	include <yake/physics/yakePhysicsPrerequisites.h>
#endif

#ifndef YAKE_PHYSICS_WORLD_H
#	include <yake/physics/yakePhysicsWorld.h>
#endif

	/** The description structs are used for creating elements.

		Why is this necessary?
			Physics engines differ in the way they define and create physics objects.
			Often the order of setting parameters is crucial, e.g. in ODE as soon as a fixed joint
			is attached to two objects, the anchor point cannot be set again (workaround is destroying
			the joint, recreating it, setting the new anchor point and reattaching the objects).

			Furthermore we give the concrete implementation of yake::physics the freedom to
			use certain optimizations and at least avoid sub-optimal creation paths.

			Normally, most objects won't be massively modified once created, therefore this approach
			should work fine. Nevertheless modification is still possible. Depending on the restrictions
			of the underlying physics system modifications may be sub-optimal in certain situations (see
			the example of fixed joints in ODE above).
	*/

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace physics {

	class YAKE_PHYSICS_API IPhysicsSystem
	{
		YAKE_DECLARE_REGISTRY_0( IPhysicsSystem, base::String );
	public:
		virtual ~IPhysicsSystem() {}
		virtual SharedPtr<IWorld> createWorld() = 0;
	};

}
}

#endif

