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
		sigAttached_(tag,vm);
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
		sigAttached_(tag,vm);
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
	boost::signals::connection VMHolder::subsribeToVmAttached(const VmAttachedSignal::slot_type& slot)
	{
		return sigAttached_.connect(slot);
	}
	boost::signals::connection VMHolder::subscribeToVmDetached(const VmDetachedSignal::slot_type& slot)
	{
		return sigDetached_.connect(slot);
	}

}
}
