#include "yake/ent/pch.h"
#include "yake/ent/ent.h"

namespace yake {
namespace ent {

	void CoHolder::addCo(const CoTag& tag, Co* co)
	{
		if (getCo(tag))
			return; //@todo throw instead?
		components_.insert( std::make_pair(tag, co) );
	}
	Co* CoHolder::getCo(const CoTag& tag) const
	{
		TagCoMap::const_iterator itFind = components_.find(tag);
		return ((itFind == components_.end()) ? 0 : itFind->second);
	}

} // namespace object
} // namespace yake

