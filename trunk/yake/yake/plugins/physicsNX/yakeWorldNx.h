#ifndef YAKE_NX_WORLD_H
#define YAKE_NX_WORLD_H

namespace yake {
namespace physics {

	class WorldNx : public IWorld
	{
	public:
		WorldNx();
		virtual ~WorldNx();

		virtual SharedPtr<IJoint> createJoint( const IJoint::DescBase & rkJointDesc );
		virtual SharedPtr<IActor> createActor( const IActor::Desc & rkActorDesc );

		virtual Deque<ShapeType> getSupportedShapes(bool bStatic = true, bool bDynamic = true) const;
		virtual Deque<JointType> getSupportedJoints() const;
		virtual Deque<String> getSupportedSolvers() const;
		virtual bool useSolver( const String & rkSolver );
		virtual String getCurrentSolver() const;
		virtual const PropertyNameList& getCurrentSolverParams() const;
		virtual void setCurrentSolverParam( const String & rkName, const boost::any & rkValue );

		virtual void step(const real timeElapsed);

		bool init_();
	private:
		String		mCurrentSolver;
		NxScene*	mpScene;
	};

}
}


#endif