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
#include <yapp/base/yappPCH.h>
#include <yapp/base/yapp.h>
#include <yapp/model/yakeModelLink.h>
#include <yapp/model/yakeModelMovableLink.h>

namespace yake {
namespace model {
namespace complex {

	//-----------------------------------------------------
	Model::Model() : mParent( 0 )
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
	void Model::addChildModel( SharedPtr<Model>& rModel )
	{
		YAKE_ASSERT( rModel.get() );
		mChildren.push_back( rModel );
	}
	
	//-----------------------------------------------------
	SharedPtr<Model> Model::getChildModelByName( const String& rName ) const
	{
		return SharedPtr<Model>();
	}
	
	//-----------------------------------------------------
	SharedPtr<Model> Model::removeChildModel( const String& rName )
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
	void Model::addPhysical( const SharedPtr<Physical>& pPhysical, const String& rName )
	{
		YAKE_ASSERT( pPhysical.get() );
		String name = rName;
		if (name != "")
		{
			PhysicalMap::const_iterator itFind = mPhysicals.find( name );
			YAKE_ASSERT( itFind == mPhysicals.end() ).debug( "submodel needs a unique name! name already taken!" );
		}
		else
			name = uniqueName::create( "auto_" );
		mPhysicals.insert( std::make_pair(name,pPhysical) );
	}

	//-----------------------------------------------------
	void Model::addGraphical( const SharedPtr<Graphical>& pGraphical, const String& rName )
	{
		YAKE_ASSERT( pGraphical.get() );
		String name = rName;
		if ( name != "" )
		{
			GraphicalMap::const_iterator itFind = mGraphicals.find( name );
			YAKE_ASSERT( itFind == mGraphicals.end() ).debug( "submodel needs a unique name! name already taken!" );
		}
		else
			name = uniqueName::create("auto_");
		mGraphicals.insert( std::make_pair(name,pGraphical) );
	}

	//-----------------------------------------------------
	Physical* Model::getPhysicalByName( const String& rName ) const
	{
		PhysicalMap::const_iterator itFind = mPhysicals.find( rName );
		
		if (itFind == mPhysicals.end())
			return 0;
		
		return itFind->second.get();
	}

	//-----------------------------------------------------
	Graphical* Model::getGraphicalByName( const String& rName ) const
	{
		GraphicalMap::const_iterator itFind = mGraphicals.find( rName );
		
		if ( itFind == mGraphicals.end() )
			return 0;
		
		return itFind->second.get();
	}

	//-----------------------------------------------------
	void Model::addGraphicsController(const SharedPtr<IObjectController>& pController, const String& rName)
	{
		YAKE_ASSERT( 0 == 1 ).warning( "IS THIS REALLY A GRAPHICS CONTROLLER?" );
		YAKE_ASSERT( pController.get() );
		YAKE_ASSERT( rName.length() == 0 ).warning( "tagging not implemented!" );
		
		mControllers.push_back( pController );
	}

	//-----------------------------------------------------
	void Model::addLink( const SharedPtr<ModelLink>& pModelLink, const String& rName )
	{
		YAKE_ASSERT( pModelLink.get() );
		YAKE_ASSERT( rName.length() == 0 ).warning( "tagging not implemented!" );
		
		mControllers.push_back( pModelLink );
	}

	//-----------------------------------------------------
	ModelMovableLink* Model::addLink( Movable* pSource, Movable* pTarget )
	{
		YAKE_ASSERT( pSource );
		YAKE_ASSERT( pTarget );
		if (!pSource || !pTarget)
			return 0;
		ModelMovableLink* pLink = 0;
		MovableLinkMap::iterator itFind = mMovableLinkMap.find( pSource );
		if (itFind == mMovableLinkMap.end())
		{
			pLink = new ModelMovableLink();
			pLink->setSource( pSource );
			mMovableLinkMap.insert( std::make_pair(pSource,pLink) );
			mControllers.push_back( SharedPtr<IObjectController>(pLink) );
		}
		else
			pLink = itFind->second;
		
		pLink->subscribeToPositionChanged( pTarget );
		pLink->subscribeToOrientationChanged( pTarget );

		return pLink;
	}

	//-----------------------------------------------------
	void Model::updatePhysics( real timeElapsed )
	{
	}
	
	//-----------------------------------------------------
	void Model::updateGraphics( real timeElapsed )
	{
		for (CtrlrList::const_iterator it = mControllers.begin(); it != mControllers.end(); ++it)
		{
			(*it)->update( timeElapsed );
		}
	}

} // ns complex
} // ns model
} // ns yake
