#ifndef YAKE_OBJECT_PREREQUISITES_H
#define YAKE_OBJECT_PREREQUISITES_H

#include "yake/prop/yakeProperty.h"
#include "yake/statemachine/fsm_core.h"
#include "yake/base/yakeTaggedListenerManager.h"
#include "yake/base/templates/yakeVector.h"

#if defined(YAKE_ENT_EXPORTS)
#	define YAKE_ENT_API DLLEXPORT
#else
#	define YAKE_ENT_API DLLIMPORT
#endif

namespace yake {
namespace object {

	typedef fsm::machine<std::string,std::string> object_fsm;

} // namespace object
} // namespace yake

#endif
