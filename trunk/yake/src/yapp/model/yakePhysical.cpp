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
	Physical::ComplexList Physical::getComplexObjects() const
	{
		Physical::ComplexList c;
		ConstVectorIterator<Physical::ComplexMap> it( mComplexObjects.begin(), mComplexObjects.end() );
		while (it.hasMoreElements())
		{
			c.push_back( it.getNext().second );
		}
		return c;
	}
	//-----------------------------------------------------
	void Physical::translate( const Vector3 & d )
	{
		VectorIterator< ComplexMap > itComplex( mComplexObjects.begin(), mComplexObjects.end() );
		while (itComplex.hasMoreElements())
		{
			itComplex.getNext().second->translate( d );
		}
	}
	//-----------------------------------------------------
	void Physical::addComplex( SharedPtr<physics::IComplexObject> & pComplex, const String & rName )
	{
		YAKE_ASSERT( pComplex.get() );
		if (!pComplex.get())
			return;
		String name = (rName.length() > 0) ? rName : (base::uniqueName::create("physical_complex_"));
		mComplexObjects.insert( std::make_pair(name,pComplex) );
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
	SharedPtr<physics::IBody> Physical::getBodyByName( const String & rName ) const
	{
		YAKE_ASSERT( 1==0 && "not implemented!" );
		return SharedPtr<physics::IBody>();
	}
	//-----------------------------------------------------
	SharedPtr<physics::IComplexObject> Physical::getComplexByName( const String & rName ) const
	{
		ComplexMap::const_iterator itFind = mComplexObjects.find( rName );
		YAKE_ASSERT( itFind != mComplexObjects.end() )( rName ).warning("couldn't find the object!");
		if (itFind == mComplexObjects.end())
			return SharedPtr<physics::IComplexObject>();
		else
			return itFind->second;
	}

}
}
}
