#include <yakePhysics/inc/pch.h>
#include <yakeBase/inc/yake.h>
#include <yakePhysics/inc/yakePhysicsSystem.h>
#include <yakePhysics/inc/yakePhysical.h>

namespace yake {
namespace physics {

	Physical::Physical() : mBaseMovable(0)
	{
	}

	Physical::~Physical()
	{
		for (PhysicalList::const_iterator it = mChildren.begin(); it != mChildren.end(); ++it)
			delete (*it);
		for (ComplexList::const_iterator it = mComplex.begin(); it != mComplex.end(); ++it)
			delete (*it);
	}

	void Physical::setPosition( const Vector3 & position )
	{
		if (mBaseMovable)
		{
			mBaseMovable->setPosition( position );
			for (PhysicalList::const_iterator it = mChildren.begin(); it != mChildren.end(); ++it)
				(*it)->setPosition( position );
		}
	}

	void Physical::setOrientation( const Quaternion & orientation )
	{
		if (mBaseMovable)
		{
			mBaseMovable->setOrientation( orientation );
			for (PhysicalList::const_iterator it = mChildren.begin(); it != mChildren.end(); ++it)
				(*it)->setOrientation( orientation );
		}
	}

	Vector3 Physical::getPosition() const
	{
		if (mBaseMovable)
			return mBaseMovable->getPosition();
		return Vector3::kZero;
	}

	Quaternion Physical::getOrientation() const
	{
		if (mBaseMovable)
			return mBaseMovable->getOrientation();
		return Quaternion::kIdentity;
	}

	void Physical::setBaseMovable( Movable & pBaseObj )
	{
		mBaseMovable = &pBaseObj;
	}

	void Physical::removeBaseMovable()
	{
		mBaseMovable = 0;
	}

	void Physical::addChild( Physical* pPhysical )
	{
		mChildren.push_back( pPhysical );
	}

	void Physical::addComplex(physics::IComplexObject* pComplex )
	{
		mComplex.push_back( pComplex );
	}

	void Physical::associate(const base::String & name, physics::IComplexObject* pComplex)
	{
		mComplexNames[ name ] = pComplex;
	}

	physics::IComplexObject* Physical::getComplex( const base::String & name )
	{
		return mComplexNames[name];
	}


}
}