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

	ModelManager::ModelManager()
	{
	}
	ModelManager::~ModelManager()
	{
	}
	namespace detail {
		void extractParams(const String& params, StringMap& out)
		{
			StringVector singles = split<String>(params,";");
			ConstVectorIterator<StringVector> it( singles );
			while (it.hasMoreElements())
			{
				const String s = it.getNext();
				StringVector keyValuePair = split<String>( s, "=" );
				out[ keyValuePair.front() ] = (keyValuePair.size() == 1) ? "" : keyValuePair.back();
			}
		}
	} // namespace detail
	/// graphics/dotScene:name=gfx;file=bla.scene|physics/dotXODE...
	Model* ModelManager::createModel(const String& def)
	{
		Vector<String> defComponents = split<String>(def, "|");
		YAKE_ASSERT(!defComponents.empty());
		if (defComponents.empty())
			return 0;
		Model* m = new Model();
		models_.push_back( SharedPtr<Model>(m) );
		ConstVectorIterator<Vector<String> > itDef( defComponents );
		while (itDef.hasMoreElements())
		{
			const String& defC = itDef.getNextRef();

			// Split e.g. "gfx=graphics/dotScene:file=blah.scene" into "gfx" and "file=graphics/dotScene:blah.scene"
			Vector<String> defTypeRest = split<String>( defC, ":" );
			YAKE_ASSERT( defTypeRest.size() == 2 );

			const String type = defTypeRest.front(); // e.g. "graphics/dotScene"

			StringMap params;
			detail::extractParams( defTypeRest.back(), params );

			StringMap::const_iterator itParam = params.find("name");
			YAKE_ASSERT( itParam != params.end() ).debug("No property 'name' for component!");
			if (itParam == params.end())
				continue;
			const String name = itParam->second;

			YAKE_ASSERT( m->getComponentByTag(name) == 0 ).debug("Components with duplicate tags are not allowed within the same model!");

			m->addComponent( creatorMgr_.create( type, ctx_, params ), name );
		}
		return m;
	}
	void ModelManager::setCreationContext_GraphicalWorld(graphics::IWorld* w)
	{
		ctx_.gworld_ = w;
	}
	void ModelManager::setCreationContext_PhysicalWorld(physics::IWorld* w)
	{
		ctx_.pworld_ = w;
	}

} // namespace model
} // namespace yake
