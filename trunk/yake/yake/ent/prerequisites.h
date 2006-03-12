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
#ifndef YAKE_OBJECT_PREREQUISITES_H
#define YAKE_OBJECT_PREREQUISITES_H

#include "yake/prop/yakeProperty.h"
#include "yake/statemachine/fsm_core.h"
#include "yake/base/yakeTaggedListenerManager.h"
#include "yake/base/templates/yakeVector.h"
#include "yake/object/yakeObjects.h"

#if defined(YAKE_ENT_EXPORTS)
#	define YAKE_ENT_API DLLEXPORT
#else
#	define YAKE_ENT_API DLLIMPORT
#endif

namespace yake {
namespace ent {

	typedef fsm::machine<std::string,std::string> object_fsm;
	typedef object::ObjectId<object::default_objectid_traits> ObjectId;
	typedef ObjectId::ClassId ClassId;

	struct ClsEntry
	{
		RttiClass* cls;
	};
	typedef object::ClassAndObjectIdManager<ClsEntry> ClassAndObjectIdManager;

} // namespace ent
} // namespace yake

#endif
