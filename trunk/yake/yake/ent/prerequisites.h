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
