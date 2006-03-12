#ifndef YAKE_ENT_OBJECT_COMPONENT_H
#define YAKE_ENT_OBJECT_COMPONENT_H

#include "yake/ent/prerequisites.h"
#include "yake/prop/yakeProperty.h"

namespace yake {
namespace ent {

	struct Co;

#define DECL_CO(CLASS,DESC) \
	DECL_CLASS_RTTI(CLASS,DESC) \
public: \
	static CLASS* cast(::yake::ent::Co*);

#define IMPL_CO(CLASS) \
	IMPL_CLASS_RTTI(CLASS)

#define IMPL_CO_1(CLASS,PARENT1) \
	IMPL_CLASS_RTTI_1(CLASS,PARENT1)

	struct YAKE_ENT_API Co : public PropHolder<Co>
	{
		virtual ~Co() {}

		DECL_CO(Co,"Component base")
	public:
	};

} // namespace ent
} // namespace yake

#endif
