#ifndef YAKE_ENT_COMPONENT_HOLDER_H
#define YAKE_ENT_COMPONENT_HOLDER_H

#include "prerequisites.h"

namespace yake {
namespace ent {

	typedef std::string CoTag;
	struct YAKE_ENT_API CoHolder
	{
		// operations
		void addCo(const CoTag&, Co*);
		Co* removeCo(const Co*);
		Co* removeCo(const CoTag&);
		Co* getCo(const CoTag&) const;
	private:
		typedef std::map<CoTag,Co*> TagCoMap;
		TagCoMap	components_;
	};

} // namespace ent
} // namespace yake

#endif
