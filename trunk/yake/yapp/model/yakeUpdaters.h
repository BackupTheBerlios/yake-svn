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
#ifndef YAPP_MODEL_UPDATERS_H
#define YAPP_MODEL_UPDATERS_H

#include <yapp/base/yappPrerequisites.h>

	/** Example: (updating a graphics node from a physically simulated body)

	MovableUpdateController //mChassisUpdateController;
									mChassisUpdCtrlr;

	// 1. source
	MovableUpdateController.setSource( mChassis->getBody() );

	// 2. subscribe to signals

	// GENERAL CASE:
	//mScopedSignals += mChassisUpdCtrlr.subscribeToPositionChanged( Bind(&mChassisNode, &ISceneNode::setPosition) );
	//mScopedSignals += mChassisUpdCtrlr.subscribeToOrientationChanged( Bind(&mChassisNode, &ISceneNode::setOrientation) );

	// MOVABLE :
	mScopedSignals += mChassisUpdCtrlr.subscribeToPositionChanged( &mChassisNode );
	mScopedSignals += mChassisUpdCtrlr.subscribeToOrientationChanged( &mChassisNode );

	// 3. update controller somehow, e.g.
	GlobalUpdateThingy.subscribeToFrameUpdate( &mChassisUpdCtrlr, MovableUpdateController::update );

	// 4. per frame/tick/whatever: update all update controllers
	GlobalUpdateThingy.update( timeElapsed );
	*/

namespace yake {
namespace app {
namespace model {

using namespace ::yake::base;
using namespace ::yake::base::math;
using namespace ::yake::base::templates;

		//-----------------------------------------------------
		typedef Deque<real> RealVector;

		/** An interpolator that uses a cache.
			The size of the cache and the weights of each entry can be specified.
			@todo merge with ValueInterpolator etc from YAKE.
		*/
		template< class T >
		class CachedInterpolator
		{
		public:
			typedef T ValueType;
			typedef Deque< T > ValueVector;

			ValueType get(const ValueType & defaultValue) const
			{
				ValueType ret = defaultValue;
				uint8 numValues = mValues.size();
				if (numValues == 0)
					return ret;
				for (uint8 i = 0; i<numValues; ++i)
				{
					ret += /*mWeights[i] * */ mValues[i];
				}
				ret /= numValues;
				return ret;
			}
			CachedInterpolator( uint8 size ) : mSize(size)
			{
				if (mSize < 1)
					mSize = 1;
				mWeights.resize( mSize );
			}
			void setWeight( uint8 index, real weight )
			{
				mWeights[index] = weight;
			}
			void pushValue( const T & value )
			{
				while (mValues.size() >= mSize-1)
					mValues.pop_back();
				mValues.push_front( value );
			}
		private:
			RealVector		mWeights;
			ValueVector		mValues;
			uint8			mSize;
		};

		/** Base class for controllers. Controllers are a very general concept. Nothing
			much to say about them. Everything that needs to be updated regularly and
			has some kind of job of updating or controlling objects may be an
			object controller.
		*/
		class YAPP_BASE_API IObjectController
		{
		public:
			virtual ~IObjectController() {}
			virtual void update( real timeElapsed ) = 0;
		};

		/** A controller that updates objects. The source object type and the
			instance can be specified.
			@see MovableUpdateController
		*/
		template< class SourceType >
		class ObjectUpdateController : public IObjectController
		{
		private:
			SourceType	* mSource;

		public:
			virtual void setUpdateSource( SourceType * source )
			{ mSource = source; }

			SourceType* getUpdateSource() const
			{ return mSource; }
		};

		/** A MovableUpdateController is a controller that forwards position and/or orientation
			to other objects. Simplified methods for other Movable objects are provided.
			Forwarding is achieved using signals.
			@Remarks This concept is used in the 'dotLink' concept. See ModelLinks and MovableLinkCreators
				and similar classes for more details.
		*/
		class YAPP_BASE_API MovableUpdateController : public ObjectUpdateController< Movable >
		{
		protected:
			typedef Signal1< void(const Vector3 &) > PositionSignal;
			PositionSignal		mPositionSignal;

			typedef Signal1< void(const Quaternion &) > OrientationSignal;
			OrientationSignal	mOrientationSignal;

			Vector3				mLastPosition;
			Quaternion			mLastOrientation;

		public:
			MovableUpdateController() {}
			SignalConnection subscribeToPositionChanged( const PositionSignal::slot_type & slot );
			SignalConnection subscribeToOrientationChanged( const OrientationSignal::slot_type & slot );

			SignalConnection subscribeToPositionChanged( Movable* pMovable )
			{
				return mPositionSignal.connect( Bind1( &Movable::setPosition, pMovable ) );
			}
			SignalConnection subscribeToOrientationChanged( Movable* pMovable )
			{
				return mOrientationSignal.connect( Bind1( &Movable::setOrientation, pMovable ) );
			}


			virtual void update( real timeElapsed )
			{
				Movable * pSource = getUpdateSource();
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
		};

}
}
}

#endif
