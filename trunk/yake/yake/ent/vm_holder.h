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
