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
#	include "yakePrerequisites.h"
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

/** Represents a link between two objects.
	This link can be modified, for example, using an ObjectLinkUpdater.
	\see ObjectLinkUpdater
*/
template < typename TSRC, typename TTARGET >
class ObjectLink
{
protected:
	typedef TSRC SourceType;
	typedef TTARGET TargetType;

	SourceType	* mSource;
	TargetType	* mTarget;
public:
	ObjectLink( TSRC* pSource, TTARGET* pTarget ) : mSource( pSource ), mTarget( pTarget )
	{
	}

	TSRC* getSource() const
	{
		return mSource;
	}

	TTARGET* getTarget() const
	{
		return mTarget;
	}

	bool isValid() const
	{
		return ( mTarget != 0 && mSource != 0 );
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
	ObjectLink< SRC, TGT >	mLink;
public:
	ObjectLinkUpdater( SRC* pSource, TGT* pTarget ) : mLink( pSource, pTarget )
	{
	}

	ObjectLink< SRC, TGT > * getLink() const
	{
		return &mLink;
	}
	virtual void update( const UPD & upd ) = 0;
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
