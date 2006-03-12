#include "yake/ent/pch.h"
#include "yake/ent/ent.h"

namespace yake {
namespace ent {

	VMHolder::~VMHolder()
	{
		YAKE_ASSERT( vms_.empty() );
		/*
		ConstVectorIterator<VmMap> it(vms_);
		while (it.hasMoreElements())
		{
			VmMap::value_type& v = it.getNext();
			if (v.second)
				delete v.second;
		}
		*/
		vms_.clear();
	}
	bool VMHolder::attachVM(scripting::IVM* vm,const std::string& tag)
	{
		YAKE_ASSERT(vm)(tag).debug("null vm");
		if (!vm)
			return false;
		YAKE_ASSERT( !tag.empty() )(tag).debug("invalid tag");
		if (tag.empty())
			return false;
		VmMap::const_iterator it = vms_.find(tag);
		YAKE_ASSERT( it == vms_.end() )(tag).debug("tag already used.");
		if (vms_.end() != it)
			return false;
		vms_.insert( VmMap::value_type(tag,vm) );
		return true;
	}
	scripting::IVM* VMHolder::detachVM(const std::string &tag)
	{
		YAKE_ASSERT( !tag.empty() )(tag).debug("invalid tag");
		if (tag.empty())
			return 0;
		VmMap::iterator it = vms_.find(tag);
		YAKE_ASSERT( it != vms_.end() )(tag).debug("tag not found.");
		if (vms_.end() == it)
			return 0;
		scripting::IVM* vm = it->second;
		vms_.erase(tag);
		return vm;
	}
	scripting::IVM* VMHolder::getVM(const std::string &tag) const
	{
		YAKE_ASSERT( !tag.empty() )(tag).debug("invalid tag");
		if (tag.empty())
			return 0;
		VmMap::const_iterator it = vms_.find(tag);
		YAKE_ASSERT( it != vms_.end() )(tag).debug("tag not found.");
		if (vms_.end() == it)
			return 0;
		return it->second;
	}

}
}