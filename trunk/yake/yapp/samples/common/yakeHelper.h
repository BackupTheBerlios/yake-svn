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
#ifndef YAPP_SAMPLES_HELPER_H
#define YAPP_SAMPLES_HELPER_H

using namespace yake;
using namespace yake::base;
using namespace yake::base::math;
using namespace yake::graphics;

namespace yapp {

	class CameraFollowController
	{
	private:
		Movable*		mCamMovable;
		Movable*		mTarget;
	public:
		CameraFollowController( Movable* pCamMovable, Movable* pTarget ) :
		mTarget(pTarget), mCamMovable(pCamMovable)
		{}

		void update( real timeElapsed )
		{
			YAKE_ASSERT( mCamMovable );
			YAKE_ASSERT( mTarget );

			Vector3 targetDir = mTarget->getOrientation() * (Vector3::kUnitZ);
			//mCamMovable->setPosition( mTarget->getPosition() + Vector3(0,7,14) );
			mCamMovable->setPosition( mTarget->getPosition() + Vector3(0,1.7,7) );
			//mCamMovable->setOrientation( mTarget->getOrientation() );
		}
	};

	model::MovableUpdateController* createMovableUpdateController(
				Movable* pSrc, Movable* pTgt, bool position, bool orientation )
	{
		YAKE_ASSERT( pSrc );
		YAKE_ASSERT( pTgt );
		YAKE_ASSERT( position || orientation );
		model::MovableUpdateController* pMUC = new model::MovableUpdateController();
		YAKE_ASSERT( pMUC );
		pMUC->setUpdateSource( pSrc );
		if (position)
			pMUC->subscribeToPositionChanged( pTgt );
		if (orientation)
			pMUC->subscribeToOrientationChanged( pTgt );
		return pMUC;
	}

	typedef Vector<SharedPtr<graphics::ISceneNode> > SharedSceneNodeList;
	typedef Vector<SharedPtr<model::MovableUpdateController> > SharedMUCList;
	typedef Vector<physics::ICollisionGeometry*> CollisionGeomList;

	void createDebugGeometryForComplexPhysical(
				const physics::IComplexObject* pComplex,
				graphics::IGraphicalWorld* pWorld,
				Vector<graphics::ISceneNode*> & nodes,
				SharedMUCList & ctrlrs,
				const String & overrideMesh = "",
				real overrideMeshScaleFactor = 1. )
	{
		YAKE_ASSERT( pComplex );
		YAKE_ASSERT( pWorld );
		const CollisionGeomList geoms = pComplex->getCollisionGeometryVector();
		if (geoms.size() == 0)
			return;
		ConstVectorIterator< const CollisionGeomList > it( geoms.begin(), geoms.end() );
		while (it.hasMoreElements())
		{
			physics::ICollisionGeometry* pGeom = it.getNext();
			YAKE_ASSERT( pGeom );

			graphics::ISceneNode* pSN = pWorld->createSceneNode();
			YAKE_ASSERT( pSN );
			graphics::IEntity* pE = 0;

			switch (pGeom->getType())
			{
			case physics::ICollisionGeometry::CGT_BOX:
				{
				pE = pWorld->createEntity( (overrideMesh == "") ? "box_1x1x1.mesh" : overrideMesh );
				pE->setCastsShadow( true );
				Vector3 dim = pGeom->boxGetDimensions();
				pSN->setScale( dim );
				}
				break;
			case physics::ICollisionGeometry::CGT_SPHERE:
				{
				pE = pWorld->createEntity( (overrideMesh == "") ? "sphere_d1.mesh": overrideMesh);
				pE->setCastsShadow( true  );
				real scale = overrideMeshScaleFactor * 2 /* x2 because diameter is 1, therefore radius is 0.5 by default*/ * pGeom->sphereGetRadius();
				if (scale < 0.0001)
					scale = 1.;
				pSN->setScale( Vector3(scale,scale,scale) );
				}
				break;
			default:
				YAKE_ASSERT( 1==0 ).error("unhandled debug geometry.");
				break;
			}

			YAKE_ASSERT( pE );
			pSN->attachEntity( pE );
			nodes.push_back( pSN );

			model::MovableUpdateController* pMUC = new model::MovableUpdateController();
			YAKE_ASSERT( pMUC );
			pMUC->setUpdateSource( pGeom );
			pMUC->subscribeToPositionChanged( pSN );
			pMUC->subscribeToOrientationChanged( pSN );
			ctrlrs.push_back( SharedPtr<model::MovableUpdateController>(pMUC) );
		}
	}

	void createDebugGeometryForComplexPhysical(
				const physics::IComplexObject* pComplex,
				graphics::IGraphicalWorld* pWorld,
				SharedSceneNodeList & nodes,
				SharedMUCList & ctrlrs,
				const String & overrideMesh = "",
				real overrideMeshScaleFactor = 1.)
	{
		Vector<graphics::ISceneNode*> newNodes;
		createDebugGeometryForComplexPhysical( pComplex, pWorld, newNodes, ctrlrs, overrideMesh, overrideMeshScaleFactor );
		VectorIterator< Vector<graphics::ISceneNode*> > it( newNodes.begin(), newNodes.end() );
		while (it.hasMoreElements())
		{
			graphics::ISceneNode* pSN = it.getNext();
			nodes.push_back( SharedPtr<graphics::ISceneNode>( pSN ) );
		}
	}

}

#endif