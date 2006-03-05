#ifndef YAKE_OBJECT_COMPONENT_H
#define YAKE_OBJECT_COMPONENT_H

#include "yake/ent/prerequisites.h"
#include "yake/prop/yakeProperty.h"

namespace yake {
namespace object {

	struct Co;

#define DECL_CO(CLASS,DESC) \
	DECL_CLASS_RTTI(CLASS,DESC) \
public: \
	static CLASS* cast(::yake::object::Co*);

	struct YAKE_ENT_API Co : public PropHolder<Co>
	{
		virtual ~Co() {}

		DECL_CO(Co,"Component base")
	public:
	};

} // namespace object
} // namespace yake

#endif
