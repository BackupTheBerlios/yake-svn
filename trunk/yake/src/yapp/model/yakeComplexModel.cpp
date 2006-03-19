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
	void Model::addPhysical( Physical* pPhysical, const String& rName )
	{
		addPhysical( SharedPtr<Physical>( pPhysical ), rName );
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
	void Model::addGraphical( Graphical* pGraphical, const String & rName /*= ""*/ )
	{
		addGraphical( SharedPtr<Graphical>( pGraphical ), rName );
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
	void Model::addGraphicsController( IObjectController* pController, const String & rName /*= ""*/ )
	{
		addGraphicsController( SharedPtr<IObjectController>( pController ), rName );
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

		ModelMovableLink* pLink = NULL;
		MovableLinkMap::iterator itFind = mMovableLinkMap.find( pSource );
		
		if (itFind != mMovableLinkMap.end())
		{
			YAKE_LOG_INFORMATION( "It seems that link already exists. Trying to determine..." );
			pLink = dynamic_cast<ModelMovableLink*>( itFind->second );
		}
		
		if ( pLink == NULL )
		{
			pLink = new ModelMovableLink();
			pLink->setSource( pSource );
			mMovableLinkMap.insert( std::make_pair(pSource,pLink) );
			mControllers.push_back( SharedPtr<IObjectController>(pLink) );
		}
		
		pLink->subscribeToPositionChanged( pTarget );
		pLink->subscribeToOrientationChanged( pTarget );

		return pLink;
	}

	//-----------------------------------------------------
	ModelMovableWorldLink* Model::addWorldLink( Movable* pSource, Movable* pTarget )
	{
		YAKE_ASSERT( pSource != NULL ).warning( "Source movable doesn't exist" );
		YAKE_ASSERT( pTarget != NULL ).warning( "Target movable doesn't exist" );

		if (!pSource || !pTarget)
			return 0;

		ModelMovableWorldLink* pLink = NULL;
		MovableLinkMap::iterator itFind = mMovableLinkMap.find( pSource );
		
		if ( itFind != mMovableLinkMap.end() )
		{
			YAKE_LOG_INFORMATION( "It seems that link already exists. Trying to determine..." );
			pLink = dynamic_cast<ModelMovableWorldLink*>( itFind->second );
		}
		
		if ( pLink == NULL )
		{
			YAKE_LOG_INFORMATION( "This is new link. Creating." );

			pLink = new ModelMovableWorldLink();
			pLink->setSource( pSource );
			mMovableLinkMap.insert( std::make_pair(pSource,pLink) );
			mControllers.push_back( SharedPtr<IObjectController>(pLink) );

			YAKE_LOG_INFORMATION( "Controller created." );
		}
		
		YAKE_LOG_INFORMATION( "Subscribing to controller signals..." );

		pLink->subscribeToPositionChanged( pTarget );
		pLink->subscribeToOrientationChanged( pTarget );

		YAKE_LOG_INFORMATION( "Done. Link setup." );
		return pLink;
	}

	//-----------------------------------------------------
	void Model::updatePhysics( real timeElapsed )
	{
	    // updating all body affectors applied to all physicals...
	    PhysicalMap::iterator ph_end = mPhysicals.end();
	    for( PhysicalMap::iterator ph = mPhysicals.begin(); ph != ph_end; ++ph )
	    {
		ph->second->updateAffectors( timeElapsed );
	    }
	}
	
	//-----------------------------------------------------
	void Model::updateGraphics( real timeElapsed )
	{
		// FIXME this assumes model does have only graphics controllers!
		// So should there be some separate updateControllers method?
		//
		for (CtrlrList::const_iterator it = mControllers.begin(); it != mControllers.end(); ++it)
		{
			(*it)->update( timeElapsed );
		}
	}

} // ns complex
} // ns model
} // ns yake

