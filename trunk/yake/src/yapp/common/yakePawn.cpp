#include <yapp/samples/base/vehicle/yakePCH.h>
#include <yapp/base/yapp.h>
#include <yapp/model/yakeModel.h>
#include <yapp/common/yakePawn.h>

namespace yake {
namespace app {
namespace model {

	Pawn::Pawn() : mComplex(0)
	{
	}
	Pawn::~Pawn()
	{
		YAKE_SAFE_DELETE( mComplex );
	}
	void Pawn::setModel( model::complex::Model* pComplex )
	{
		YAKE_ASSERT( pComplex );
		mComplex = pComplex;
	}
	void Pawn::setMovable( physics::IComplexObject* pMovable )
	{
		mMovable = pMovable;
	}
	void Pawn::onSense()
	{
		model::InputComponent* pInput = getInputComponent("defaultInput");
		if (!pInput)
			return;
		pInput->update();
	}
	void Pawn::onThink()
	{
	}
	void Pawn::onAct()
	{
		model::ActorControl* pControl = getControlComponent("defaultCtrl");
		if (!pControl)
			return;
		pControl->update();
	}
	void Pawn::onPostPhysics(const real timeElapsed)
	{
		updateControllers( timeElapsed );
	}
	void Pawn::updateControllers( const real timeElapsed )
	{
		YAKE_ASSERT( mComplex );
		mComplex->updateControllers( timeElapsed );
	}

}
}
}