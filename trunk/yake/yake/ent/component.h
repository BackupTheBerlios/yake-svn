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
#ifndef YAKE_ENT_OBJECT_COMPONENT_H
#define YAKE_ENT_OBJECT_COMPONENT_H

#include "yake/ent/prerequisites.h"
#include "yake/prop/yakeProperty.h"

namespace yake {
namespace ent {

#define DECL_CO(CLASS,DESC) \
	DECL_CLASS_RTTI(CLASS,DESC) \
public: \
	static CLASS* cast(::yake::ent::Component*);

#define IMPL_CO(CLASS) \
	IMPL_CLASS_RTTI(CLASS)

#define IMPL_CO_1(CLASS,PARENT1) \
	IMPL_CLASS_RTTI_1(CLASS,PARENT1)

	struct YAKE_ENT_API Component : public PropHolder<Component>
	{
		virtual ~Component() {}

		DECL_CO(Component,"Component base")
	public:
	};

} // namespace ent
} // namespace yake

#endif
