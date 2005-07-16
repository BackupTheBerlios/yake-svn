/*
   ------------------------------------------------------------------------------------
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
#ifndef YAKE_NX_WORLD_H
#define YAKE_NX_WORLD_H

namespace yake {
namespace physics {
	/*
	template< class Base_, class ConcreteProduct_ >
	class YakeNxFactoryUnit : public Base_
	{
		typedef typename Base_::ProductList BaseProductList;
	    
	protected:
		typedef typename mpl::sequences::pop_front< BaseProductList >::type ProductList;
	    
	public:
		typedef typename mpl::sequences::front< BaseProductList >::type AbstractProduct;

		ConcreteProduct_* doCreate( mpl::functions::identity< AbstractProduct > )
		{
			return new ConcreteProduct_(this,this,true);
		}
	};*/

	class ActorNx;
	class MaterialNx;
	class JointNx;
	class AvatarNx;
	class WorldNx : 
		//public mpl::ConcreteFactory< IWorld, YakeNxFactoryUnit, mpl::sequences::list<ActorNx,AvatarNx> >
		public IWorld,
		public NxUserContactReport
	{
	public:
		WorldNx();
		virtual ~WorldNx();

		virtual IJointPtr createJoint( const IJoint::DescBase & rkJointDesc );
		virtual IActorPtr createActor( const IActor::Desc& rActorDesc = IActor::Desc(ACTOR_MOVABLE) );
		virtual ActorNx* _createActor(const IActor::Desc & rkActorDesc);
		virtual IAvatarPtr createAvatar( const IAvatar::Desc & rkAvatarDesc );
		virtual IMaterialPtr createMaterial( const IMaterial::Desc & rkMatDesc );
		virtual void destroyJoint( IJointPtr pJoint );
		virtual void destroyActor( IActorPtr pActor );
		virtual void destroyAvatar( IAvatarPtr pAvatar );
		virtual void destroyMaterial( IMaterialPtr pMaterial );

		virtual TriangleMeshId createTriangleMesh( const TriangleMeshDesc & rkTrimeshDesc );

		virtual Deque<ShapeType> getSupportedShapes(bool bStatic = true, bool bDynamic = true) const;
		virtual Deque<JointType> getSupportedJoints() const;
		virtual Deque<String> getSupportedSolvers() const;
		virtual bool useSolver( const String & rkSolver );
		virtual String getCurrentSolver() const;
		virtual const StringVector getCurrentSolverParams() const;
		virtual void setCurrentSolverParam( const String & rkName, const boost::any & rkValue );

		virtual void setGlobalGravity( const Vector3& g );
		virtual Vector3 getGlobalGravity() const;

		virtual void step(const real timeElapsed);

		YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, void, PreStep )
		YAKE_MEMBERSIGNAL_FIRE_FN0( public, PreStep )
		YAKE_MEMBERSIGNAL_VIRTUALIMPL( public, void, PostStep )
		YAKE_MEMBERSIGNAL_FIRE_FN0( public, PostStep )

		//-- NxUserContactReport interface
		virtual NxU32 onPairCreated(NxActor& s1, NxActor& s2);
		virtual void onContactNotify (NxContactPair &pair, NxU32 events);

		//-- helpers
		bool init_();
		void regActor_(NxActor* pNxActor, ActorNx* pActor);
		void unregActor_(NxActor* pNxActor);

		YAKE_MEMBERSIGNAL( public, void(const real), PreStepInternal )
		YAKE_MEMBERSIGNAL_FIRE_FN1( public, PreStepInternal, const real dt, dt )

	private:
		String					mCurrentSolver;
		NxScene*				mpScene;

		typedef AssocVector< NxActor*, ActorNx* > ContactActorMap;
		ContactActorMap			mContactActors;

		template<class VectorContainerType>
		struct Deleter
		{
			void operator()(typename VectorContainerType::value_type pObj)
			{
				if (pObj)
					delete pObj;
			}
		};

		typedef std::list<ActorNx*> ActorNxVector;
		ActorNxVector			mActors;

		typedef std::list<JointNx*> JointNxVector;
		JointNxVector			mJoints;

		typedef std::list<MaterialNx*> MaterialNxVector;
		MaterialNxVector		mMaterials;

		typedef std::list<AvatarNx*> AvatarNxVector;
		AvatarNxVector			mAvatars;
	};

}
}


#endif