/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright (c) The YAKE Team
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
#include "yake/model/pch.h"
#include "yake/model/prerequisites.h"
#include "yake/model/model.h"

namespace yake {
namespace model {

	Physical::Physical()
	{
	}
	/*
	Physical::Physical(Model& owner) : ModelComponent(owner)
	{
	}
	*/
	Physical::~Physical()
	{
		for (TagActorMap::iterator itA = actors_.begin(); itA != actors_.end(); ++itA)
		{
			physics::IActor* obj = itA->second.p_;
			obj->getCreator()->destroyActor(obj);
		}
		for (TagJointMap::iterator itJ = joints_.begin(); itJ != joints_.end(); ++itJ)
		{
			physics::IJoint* obj = itJ->second.p_;
			obj->getCreator()->destroyJoint(obj);
		}
	}
	void Physical::addActor(physics::IActor* actor, const String& xpath)
	{
		YAKE_LOG(String("Physical::addActor(") + xpath + _T(")"));
		this->_add<physics::IActor>(actor,actors_,xpath);
	}
	void Physical::addJoint(physics::IJoint* joint, const String& xpath)
	{
		YAKE_LOG(String("Physical::addJoint(") + xpath + _T(")"));
		this->_add<physics::IJoint>(joint,joints_,xpath);
	}
	void Physical::addBody(physics::IBody* body, const String& xpath)
	{
		YAKE_LOG(String("Physical::addBody(") + xpath + _T(")"));
		this->_add<physics::IBody>(body,bodies_,xpath);
	}
	void Physical::addAffector(physics::IBodyAffector* affector, const String& xpath)
	{
		YAKE_LOG(String("Physical::addAffector(") + xpath + _T(")"));
		this->_add<physics::IBodyAffector>(affector,affectors_,xpath);
	}
	physics::IActor* Physical::getActor(const yake::String& xpath) const
	{
		return this->_get(actors_,xpath);
	}
	physics::IJoint* Physical::getJoint(const yake::String& xpath) const
	{
		return this->_get(joints_,xpath);
	}
	physics::IBody* Physical::getBody(const yake::String& xpath) const
	{
		return this->_get(bodies_,xpath);
	}
	physics::IBodyAffector* Physical::getAffector(const String& xpath) const
	{
		return this->_get(affectors_,xpath);
	}
	void Physical::addAffectorTargetBody(physics::IBodyAffector* affector,physics::IBody* target)
	{
		//@todo test for existence of affector in affectorTargetMap_.
		YAKE_ASSERT( affector );
		YAKE_ASSERT( target );
		affectorTargetMap_[affector] += target;
	}
	void Physical::addAffectorTargetBody(physics::IBodyAffector* affector,const String& bodyXPath)
	{
		//@todo test for existence of affector in affectorTargetMap_.
		YAKE_ASSERT( affector );
		YAKE_ASSERT( !bodyXPath.empty() );
		physics::IBodyPtr body = this->getBody( bodyXPath );
		YAKE_ASSERT( body );
		this->addAffectorTargetBody(affector,body);
	}
	void Physical::addAffectorTargetActor(physics::IBodyAffector* affector,const String& actorXPath)
	{
		//@todo test for existence of affector in affectorTargetMap_.
		YAKE_ASSERT( affector );
		YAKE_ASSERT( !actorXPath.empty() );
		physics::IActorPtr actor = this->getActor(actorXPath);
		YAKE_ASSERT( actor );
		physics::IBodyPtr body = actor->getBodyPtr();
		YAKE_ASSERT( body );
		this->addAffectorTargetBody(affector,body);
	}
	void Physical::updateAffectors(const real dt)
	{
		AffectorTargetMap::iterator itEnd = affectorTargetMap_.end();
		for (AffectorTargetMap::iterator it = affectorTargetMap_.begin(); it != itEnd; ++it)
		{
			it->first->applyTo(it->second,dt);
		}
	}

} // namespace model
} // namespace yake
