#ifndef YAPP_MODEL_PAWN_H
#define YAPP_MODEL_PAWN_H

namespace yake {
namespace app {
namespace model {

	/** Simple entity/actor that has a physical representation.
	*/
	class Pawn : public model::Actor
	{
	public:
		Pawn();
		virtual ~Pawn();
		void setModel( model::complex::Model* pComplex );
		void setMovable( physics::IComplexObject* pMovable );

		//@{ name model::Actor interface
		virtual void onSense();
		virtual void onThink();
		virtual void onAct();
		//@}

		void onPostPhysics(const real timeElapsed);
	private:
		void updateControllers( const real timeElapsed );
	private:
		model::complex::Model*		mComplex;
		physics::IComplexObject*	mMovable;
	};

}
}
}

#endif