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
namespace complex {

	//-----------------------------------------------------
	Model::Model() : mParent(0)
	{
	}
	//-----------------------------------------------------
	Model::~Model()
	{
	}
	//-----------------------------------------------------
	void Model::setParentModel( Model* pModel )
	{
		mParent = pModel;
	}
	//-----------------------------------------------------
	Model* Model::getParentModel() const
	{
		return mParent;
	}
	//-----------------------------------------------------
	void Model::addChildModel( SharedPtr<Model> & rModel )
	{
		YAKE_ASSERT( rModel.get() );
		mChildren.push_back( rModel );
	}
	//-----------------------------------------------------
	SharedPtr<Model> Model::getChildModelByName( const String & rName ) const
	{
		return SharedPtr<Model>();
	}
	//-----------------------------------------------------
	SharedPtr<Model> Model::removeChildModel( const String & rName )
	{
		return SharedPtr<Model>();
	}
	//-----------------------------------------------------
	SharedPtr<Model> Model::removeChildModel( const SharedPtr<Model>& rModel )
	{
		YAKE_ASSERT( rModel.get() );
		Model* pModel = rModel.get();
		for (ModelList::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
		{
			YAKE_ASSERT( (*it).get() );
			if ((*it) == rModel)
			{
				mChildren.erase( it );
				return rModel;
			}
		}
		return SharedPtr<Model>();
	}

	//-----------------------------------------------------
	void Model::addPhysical(SharedPtr<Physical> & pPhysical, const String & rName)
	{
		YAKE_ASSERT( pPhysical.get() );
		String name = rName;
		if (name != "")
		{
			PhysicalMap::const_iterator itFind = mPhysicals.find( name );
			if (itFind == mPhysicals.end())
				name = yake::base::uniqueName::create("auto_","_"+rName);
		}
		else
			name = yake::base::uniqueName::create("auto_");
		mPhysicals.insert( std::make_pair(name,pPhysical) );
	}

	//-----------------------------------------------------
	void Model::addGraphical(SharedPtr<Graphical> & pGraphical, const String & rName)
	{
		YAKE_ASSERT( pGraphical.get() );
		String name = rName;
		if (name != "")
		{
			GraphicalMap::const_iterator itFind = mGraphicals.find( name );
			if (itFind == mGraphicals.end())
				name = yake::base::uniqueName::create("auto_","_"+rName);
		}
		else
			name = yake::base::uniqueName::create("auto_");
		mGraphicals.insert( std::make_pair(name,pGraphical) );
	}

	//-----------------------------------------------------
	Physical* Model::getPhysicalByName( const String & rName ) const
	{
		PhysicalMap::const_iterator itFind = mPhysicals.find( rName );
		if (itFind == mPhysicals.end())
			return 0;
		return itFind->second.get();
	}

	//-----------------------------------------------------
	Graphical* Model::getGraphicalByName( const String & rName ) const
	{
		GraphicalMap::const_iterator itFind = mGraphicals.find( rName );
		if (itFind == mGraphicals.end())
			return 0;
		return itFind->second.get();
	}

	//-----------------------------------------------------
	void Model::addController(SharedPtr<IObjectController> & pController, const String & rName)
	{
		YAKE_ASSERT( pController.get() );
		mControllers.push_back( pController );
	}

	//-----------------------------------------------------
	void Model::updatePhysics( real timeElapsed )
	{
		for (PhysicalMap::iterator it = mPhysicals.begin();
			it != mPhysicals.end(); ++it)
		{
			SharedPtr<Physical> pPhysical = (it->second);
			Physical::ComplexList coList = pPhysical->getComplexObjects();
			VectorIterator< Physical::ComplexList > itCO( coList.begin(), coList.end() );
			while (itCO.hasMoreElements())
			{
				SharedPtr<physics::IComplexObject> pCO = itCO.getNext();
				physics::IBody* pBody = pCO->getBody();

				real kVel = -.2 * timeElapsed;
				real kTorque = kVel;

				Vector3 vel = pBody->getLinearVelocity();
				vel *= kVel;
				pBody->addForce( vel );

				vel = pBody->getAngularVelocity();
				vel *= kTorque;
				pBody->addTorque( vel );

			}
		}
	}
	//-----------------------------------------------------
	void Model::updateControllers( real timeElapsed )
	{
		for (CtrlrList::const_iterator it = mControllers.begin(); it != mControllers.end(); ++it)
		{
			(*it)->update( timeElapsed );
		}
	}

}
}
}
}
