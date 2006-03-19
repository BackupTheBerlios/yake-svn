/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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
#include <yapp/model/yakeModelMovableLink.h>

namespace yake {
namespace model {

	YAKE_REGISTER_CONCRETE( ModelMovableLink );

	ModelMovableLink::ModelMovableLink()
	{
	}
	SignalConnection ModelMovableLink::subscribeToPositionChanged( Movable* pMovable )
	{	
		const Vector3 srcPos = getSource()->getPosition();
		pMovable->setPosition( srcPos );

		return mPositionSignal.connect( Bind1( &Movable::setPosition, pMovable ) );
	}

	SignalConnection ModelMovableLink::subscribeToOrientationChanged( Movable* pMovable )
	{
		const Quaternion srcOrientation = getSource()->getOrientation();
		pMovable->setOrientation( srcOrientation );

		return mOrientationSignal.connect( Bind1( &Movable::setOrientation, pMovable ) );
	}

	void ModelMovableLink::update( real timeElapsed )
	{
		const Movable* pSource = getSource();
		YAKE_ASSERT( pSource ).debug("no update source -> no updates");
		if (!pSource)
			return;
		Vector3 position = pSource->getPosition();
		if (mLastPosition != position)
		{
			mPositionSignal( position );
			mLastPosition = position;
		}
		Quaternion orientation = pSource->getOrientation();
		if (mLastOrientation != orientation)
		{
			mOrientationSignal( orientation );
			mLastOrientation = orientation;
		}
	}

	YAKE_REGISTER_CONCRETE( ModelMovableWorldLink );
	ModelMovableWorldLink::ModelMovableWorldLink()
	{
	}
	SignalConnection ModelMovableWorldLink::subscribeToPositionChanged( Movable* pMovable )
	{	
		const Vector3 srcPos = getSource()->getDerivedPosition();
		pMovable->setPosition( srcPos );

		return mPositionSignal.connect( Bind1( &Movable::setPosition, pMovable ) );
	}

	SignalConnection ModelMovableWorldLink::subscribeToOrientationChanged( Movable* pMovable )
	{
		const Quaternion srcOrientation = getSource()->getDerivedOrientation();
		pMovable->setOrientation( srcOrientation );

		return mOrientationSignal.connect( Bind1( &Movable::setOrientation, pMovable ) );
	}

	void ModelMovableWorldLink::update( real timeElapsed )
	{
		const Movable* pSource = getSource();

		YAKE_ASSERT( pSource ).error("no update source -> no updates");

		if ( pSource == NULL )
			return;

		Vector3 position = pSource->getDerivedPosition();
		if (mLastPosition != position)
		{
			mPositionSignal( position );
			mLastPosition = position;
		}
		Quaternion orientation = pSource->getDerivedOrientation();
		if (mLastOrientation != orientation)
		{
			mOrientationSignal( orientation );
			mLastOrientation = orientation;
		}
	}
}
}

