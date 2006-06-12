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

	Graphical::Graphical()
	{
	}
	/*
	Graphical::Graphical(Model& owner) : ModelComponent(owner)
	{
	}
	*/
	Graphical::~Graphical()
	{
		/*
		for (TagNodeMap::iterator itN = nodes_.begin(); itN != nodes_.end(); ++itN)
		{
			if (itN->second.owned_)
				delete itN->second.p_;
		}

		for (TagEntityMap::iterator itE = entities_.begin(); itE != entities_.end(); ++itE)
		{
			if (itE->second.owned_)
				delete itE->second.p_;
		}

		for (TagLightMap::iterator itL = lights_.begin(); itL != lights_.end(); ++itL)
		{
			if (itL->second.owned_)
				delete itL->second.p_;
		}
		*/
	}
	/*
	ModelComponent* Graphical::clone() const
	{
		Graphical* cloned = new Graphical();

		// nodes
		ConstVectorIterator<TagNodeMap> itN( nodes_ );
		while (itN.hasMoreElements())
		{
			const TagNodeMap::value_type& vt = itN.getNextRef();
			graphics::ISceneNode* clonedNode = vt.second->deepClone();
			cloned->addSceneNode( clonedNode, vt.first );
		}

		//@todo lights etc

		return cloned;
	}
	*/
	void Graphical::addSceneNode(graphics::ISceneNode* node, const String& xpath, const bool owned)
	{
		YAKE_LOG(String("Graphical::addSceneNode(") + xpath + _T(")"));
		this->_add(node,nodes_,xpath,owned);
	}
	void Graphical::addEntity(graphics::IEntity* ent, const String& xpath, const bool owned)
	{
		YAKE_LOG(String("Graphical::addEntity(") + xpath + _T(")"));
		this->_add(ent,entities_,xpath,owned);
	}
	void Graphical::addLight(graphics::ILight* light, const String& xpath, const bool owned)
	{
		YAKE_LOG(String("Graphical::addLight(") + xpath + _T(")"));
		this->_add(light,lights_,xpath,owned);
	}
	graphics::ISceneNode* Graphical::getSceneNode(const String& xpath) const
	{
		return this->_get(nodes_,xpath);
	}
	graphics::IEntity* Graphical::getEntity(const String& xpath) const
	{
		return this->_get(entities_,xpath);
	}
	graphics::ILight* Graphical::getLight(const String& xpath) const
	{
		return this->_get(lights_,xpath);
	}
	void Graphical::translate(const Vector3& d)
	{
		ConstVectorIterator<TagNodeMap> itN( nodes_ );
		while (itN.hasMoreElements())
		{
			const TagNodeMap::value_type& vt = itN.getNextRef();
			//if (vt.second.owned_)
			if (!vt.second.p_->getParent())
				vt.second.p_->translate( d );
		}
	}

} // namespace model
} // namespace yake
