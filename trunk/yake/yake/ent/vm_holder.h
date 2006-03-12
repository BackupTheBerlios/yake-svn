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
#ifndef YAKE_ENT_VMHOLDER_H
#define YAKE_ENT_VMHOLDER_H

#include "yake/ent/prerequisites.h"
#include "yake/ent/object_listener.h"

namespace yake {
namespace scripting {
	class IVM;
	class IScriptingSystem;
}
namespace ent {

	struct YAKE_ENT_API VMHolder
	{
		VMHolder() {}
		virtual ~VMHolder();

		// operations

		/** Warning: Currently ownership is not transferred! */
		bool attachVM(scripting::IVM*,const std::string& tag);
		scripting::IVM* detachVM(const std::string& tag);
		scripting::IVM* getVM(const std::string& tag) const;

		// signals
		typedef boost::signal<void(const std::string&,scripting::IVM*)> VmAttachedSignal;
		boost::signals::connection subsribeToVmAttached(const VmAttachedSignal::slot_type&);
		typedef boost::signal<void(const std::string&,scripting::IVM*)> VmDetachedSignal;
		boost::signals::connection subscribeToVmDetached(const VmDetachedSignal::slot_type&);
	private:
		VmAttachedSignal					sigAttached_;
		VmDetachedSignal					sigDetached_;
		typedef std::map<std::string,scripting::IVM*> VmMap;
		VmMap									vms_;
	};


} // namespace ent
} // namespace yake

#endif
