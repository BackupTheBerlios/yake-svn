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
#ifndef YAKE_BASE_TEMPLATES_OBJECTLINK_H
#define YAKE_BASE_TEMPLATES_OBJECTLINK_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#	include "../yakePrerequisites.h"
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace templates
{
/* Example of usage:
		typedef ObjectLinkUpdater<Movable,Movable,const real> TimeBasedMovableUpdater;
		//typedef templates::ObjectLink<Movable,Movable> MovableLink;
		//MovableLink link(pSceneNode,pPhysical);
		TimeBasedMovableUpdater t( pPhysical, pSceneNode );
		t.update( timeElapsed );
*/
/** Represents a link between two objects.
	This link can be modified, for example, using an ObjectLinkUpdater.
	\see ObjectLinkUpdater
*/
template < typename T_ONE, typename T_TWO >
class ObjectLink
{
protected:
	typedef T_ONE OneType;
	typedef T_TWO TwoType;

	OneType	* mOne;
	TwoType	* mTwo;
public:
	ObjectLink( T_ONE* pFirst, T_TWO* pSecond ) : mOne( pFirst ), mTwo( pSecond )
	{
	}

	T_ONE* getFirst() const
	{
		return mOne;
	}

	T_TWO* getSecond() const
	{
		return mTwo;
	}

	bool isValid() const
	{
		return ( mTwo != 0 && mOne != 0 && mTwo != mOne );
	}
};

/** Base class for updater objects which update one object from another in some
	kind of way.
	\see ObjectLink
*/
template < typename SRC, typename TGT, typename UPD >
class ObjectLinkUpdater
{
protected:
	typedef ObjectLink< SRC, TGT > LinkType;
	LinkType	mLink;
public:
	ObjectLinkUpdater( SRC* pSource, TGT* pTarget ) : mLink( pSource, pTarget )
	{
	}

	LinkType* getLink() const
	{
		return &mLink;
	}
	virtual void update( UPD ) = 0;
};

// physical-audible
// physical-graphical

/** Updates a graphical object from a physical object.
	When update is called the graphical object is updated from the
	physical object.
	\see ObjectLinkUpdater
	\see ObjectLink
*/
/*
class PhysicalGraphicalUpdater : public ObjectLinkUpdater< Physical, Graphical, FrameUpdate >
{
public:
	void update( const FrameUpdate & upd )
	{
		if (mLink->isValid())
		{
			mLink->getTarget()->setPosition( mLink->getSource()->getPosition() );
			mLink->getTarget()->setOrientation( mLink->getSource()->getOrientation() );
		}
	}
};*/

} // templates
} // base
} // yake

#endif // YAKE_BASE_TEMPLATES
