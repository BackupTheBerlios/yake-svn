#ifndef YAKE_OBJECT_COMPONENT_HOLDER_H
#define YAKE_OBJECT_COMPONENT_HOLDER_H

#include "prerequisites.h"

namespace yake {
namespace object {

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

} // namespace object
} // namespace yake

#endif
