#ifndef YAKE_CAMERACONTROLLERS_H
#define YAKE_CAMERACONTROLLERS_H

namespace yake {

	class TopDownCameraController
	{
	public:
		TopDownCameraController();

		void setCamera(graphics::ICamera*);

		void setTarget( const Point3& );
		void setOffset( const Vector3& );
		Vector3 getOffset() const;

		void update(const real dt);
	private:
		Point3				mTargetPos;
		graphics::ICamera*	mCam;
		Vector3				mCamOffset;
	};
	/** @todo move into .inl file! */
	TopDownCameraController::TopDownCameraController() :
		mCam(0),
		mTargetPos(Point3::kZero),
		mCamOffset(Vector3(2,0,2))
	{
	}
	void TopDownCameraController::setCamera(graphics::ICamera* cam)
	{
		mCam = cam;
	}
	void TopDownCameraController::setTarget( const Point3& pt )
	{
		mTargetPos = pt;
	}
	void TopDownCameraController::setOffset( const Vector3& offset )
	{
		mCamOffset = offset;
	}
	Vector3 TopDownCameraController::getOffset() const
	{
		return mCamOffset;
	}
	void TopDownCameraController::update(const real dt)
	{
		if (!mCam)
			return;
		const Point3 targetCamPos = mTargetPos + mCamOffset;
		mCam->translate( 0.5 * (targetCamPos - mCam->getPosition()) );
		mCam->lookAt( mTargetPos );
	}

} // namespace yake

#endif
