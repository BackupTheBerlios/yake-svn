#ifndef ODE_MATERIAL_H
#define ODE_MATERIAL_H

namespace yake
{
namespace physics
{
	class OdeMaterial : public IMaterial
	{
	public:
		OdeMaterial();
		virtual void setRollingFriction( const real friction );
		virtual void setRestitution( const real restitution );
		virtual void setStaticFriction( const real friction );
		virtual void setStaticFrictionV( const real friction );
		virtual void setStaticFrictionVEnabled( bool enabled );

		real					mSoftness;
		real					mFriction;
		real					mFriction2;
		bool					mLateralSlip;
//		ISlipNormalSource*		mSlipNormalSource;
		Vector3					mSlipNormal;
		real					mSlipLinearCoeff;
		real					mSlipAngularCoeff;
	};

} // physics
} // yake

#endif
