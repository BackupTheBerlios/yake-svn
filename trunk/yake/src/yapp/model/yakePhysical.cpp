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

namespace yake {
namespace app {
namespace model {

	//-----------------------------------------------------
	Physical::~Physical()
	{
		mActors.clear();
		mJoints.clear();
	}
	//-----------------------------------------------------
	Physical::ComplexList Physical::getComplexObjects() const
	{
		Physical::ComplexList c;
		ConstVectorIterator<Physical::ActorMap> itActor( mActors.begin(), mActors.end() );
		while (itActor.hasMoreElements())
		{
			c.push_back( itActor.getNext().second );
		}
		return c;
	}
	//-----------------------------------------------------
	void Physical::translate( const Vector3 & d )
	{
		VectorIterator< ActorMap > itActor( mActors.begin(), mActors.end() );
		while (itActor.hasMoreElements())
		{
			physics::IMovableActor* pMovableActor = dynamic_cast<physics::IMovableActor*>( itActor.getNext().second.get() );
			if (pMovableActor)
				pMovableActor->translate( d );
		}
	}
	//-----------------------------------------------------
	void Physical::addActor( SharedPtr<physics::IActor> & pActor, const String & rName )
	{
		YAKE_ASSERT( pActor.get() );
		if (!pActor.get())
			return;
		String name = (rName.length() > 0) ? rName : (base::uniqueName::create("physical_actor_"));
		mActors.insert( std::make_pair(name,pActor) );
	}
	//-----------------------------------------------------
	void Physical::addJoint( SharedPtr<physics::IJoint> & pJoint )
	{
		YAKE_ASSERT( pJoint.get() );
		if (!pJoint.get())
			return;
		mJoints.push_back( pJoint );
	}
	//-----------------------------------------------------
	SharedPtr<physics::IActor> Physical::getActorByName( const String & rName ) const
	{
		ActorMap::const_iterator itFind = mActors.find( rName );
		YAKE_ASSERT( itFind != mActors.end() )( rName ).warning("couldn't find the object!");
		if (itFind == mActors.end())
			return SharedPtr<physics::IActor>();
		else
			return itFind->second;
	}

}
}
}
