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
#ifndef YAPP_MODEL_PHYSICAL_H
#define YAPP_MODEL_PHYSICAL_H

#include <yapp/base/yappPrerequisites.h>
#include <yake/physics/yakePhysics.h>
#include <yapp/model/yakeModel.h>

namespace yake {
	using namespace base::templates;
namespace app {
namespace model {

	/** A physical container model.
	*/
	class YAPP_BASE_API Physical : public Submodel
	{
	public:
		Physical()
		{}
		virtual ~Physical();

		/** Adds a physical actor and registers it with a name for easy lookup.
		*/
		void addActor( SharedPtr<physics::IActor> & pActor, const String & rName );

		/** Retrieve a physical actor by the name it was registered within this model::Physical object.
		*/
		SharedPtr<physics::IActor> getActorByName( const String & rName ) const;

		/** Adds a body affector. */
		void addAffector( SharedPtr<physics::IBodyAffector> & pAffector );

		/** Adds a joint. */
		void addJoint( SharedPtr<physics::IJoint> & pJoint );
		//void addJointGroup( SharedPtr<physics::IJointGroup> & pJointGroup );

		/** Adds a body. */
		void addBody( SharedPtr<physics::IBody> & pBody, const String & rName );

		/** Translates this model::Physical object by translating all contained actors. */
		void translate( const Vector3 & d );

		typedef Vector< SharedPtr<physics::IActor> > ComplexList;
		/** @todo optimize! */
		ComplexList getComplexObjects() const;
	private:
		typedef AssocVector< String, SharedPtr<physics::IActor> > ActorMap;
		ActorMap		mActors;

		typedef Vector< SharedPtr<physics::IJoint> > JointList;
		JointList		mJoints;
	};

} // model
} // app
} // yake

#endif
