#ifndef YAKE_PROP_HOLDER_H
#define YAKE_PROP_HOLDER_H

#include "prerequisites.h"
#include <iostream> // std::cout for warnings

namespace yake {

	struct RttiClass;
	template<typename T>
	struct PropHolder
	{
		PropHolder()
		{
			//if (!ms_cls)
			//	ms_cls = new RttiClass();
		}
		virtual ~PropHolder() {}

		virtual RttiClass* isA() const = 0;

		template<typename T>
		bool getValue(const std::string& name, T& retValue) const
		{
			StringPropPtrMap::const_iterator it = props_.find(name);
			if (it == props_.end())
				return false;
			assert( it->second );
			return it->second->getValue<T>(retValue);
		}
		template<typename T>
		bool setValue(const std::string& name, const T& value)
		{
			StringPropPtrMap::iterator it = props_.find(name);
			if (it == props_.end())
				return false;
			assert( it->second );
			return it->second->setValue<T>(value);
		}

		std::ostream& dump(std::ostream& out) const
		{
			out << "object(type='" << isA()->name() << "')\n";
			out << "   " << props_;
			return out;
		}

		void init()
		{
			initProps( *isA(), props_ );
		}
	private:
		void initProps(const RttiClass& cls, StringPropPtrMap& props)
		{
			const RttiClass::ParentClassList& parentClsList = cls.getParents();
			RttiClass::ParentClassList::const_iterator itClsEnd = parentClsList.end();
			for (RttiClass::ParentClassList::const_iterator itCls = parentClsList.begin(); itCls != itClsEnd; ++itCls)
				initProps(**itCls, props);

			const PropDefMap& defs = cls.getPropDefs();
			PropDefMap::const_iterator itDefEnd = defs.end();
			for (PropDefMap::const_iterator itDef = defs.begin(); itDef != itDefEnd; ++itDef)
			{
				const PropDef& def = itDef->second;
				if (def.name().empty())
				{
					std::cout << "warning: empty property name in class '" << cls.name() << "'. skipping.\n";
					continue;
				}
				StringPropPtrMap::const_iterator itFind = props.find(def.name());
				if (itFind == props.end())
				{
					props[ def.name() ] = def.create();
				}
				else
				{
					const PropDef& baseDef = itFind->second->def();
					std::cout << "warning: class '" << cls.name() << "' attempts to override inherited property '" << def.name() << "'\n";
					if (def.type() == baseDef.type())
					{
						std::cout << "  => override of default value accepted.\n";
						itFind->second->setValue( def.getDefaultValue() );
					}
					else
					{
						std::cout << "  => type mismatch. could not override default value.\n";
					}
				}
			}
		}

		//static const RttiClass* getRttiClass();
	private:
		//static RttiClass* ms_cls; // @todo make it auto_ptr
		StringPropPtrMap		props_;
	};
	//template<typename T> RttiClass PropHolder<T>::ms_cls = 0;

} // namespace yake

#endif
