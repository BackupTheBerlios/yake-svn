#ifndef YAKE_PROP_H
#define YAKE_PROP_H

#include "prerequisites.h"

namespace yake {

	struct Prop
	{
	public:
		friend struct PropDef;

		template<typename T>
		bool setValue(const T& v)
		{
			try {
				boost::any_cast<T>(value_); // <- test for type mismatch!
				value_ = v;
				return true;
			} 
			catch (boost::bad_any_cast&)
			{
				std::cout << "Prop::setValue() ERROR: type mismatch. '" << def_.type() << "' expected, got '" << typeid(v).name() << "'\n";
				return false;
			}
		}
		template<typename T>
		bool getValue(T& v) const
		{
			try {
				v = boost::any_cast<T>(value_);
				return true;
			} 
			catch (boost::bad_any_cast&)
			{
				return false;
			}
		}
		void setValue(const boost::any& value) // @todo Unsafe! Fix this somehow!
		{ value_ = value; }
		const PropDef& def() const
		{ return def_; }
	private:
		Prop();
		Prop(const PropDef& def, const boost::any& defaultValue) :
		value_(defaultValue), def_(def)
		{}
	private:
		boost::any				value_;
		const PropDef&			def_;
	};
	typedef std::vector<Prop> PropList;


	typedef std::map<std::string,Prop*> StringPropPtrMap;
	inline std::ostream& operator << (std::ostream& out, const StringPropPtrMap& props)
	{
		out << "properties=(\n";
		for (StringPropPtrMap::const_iterator it = props.begin(); it != props.end(); ++it)
		{
			out << "      '" << it->first << "' : '" << it->second->def().type() << "' = ";
			const std::string type = it->second->def().type();
			/*if (type == "struct vector3")
			{
				vector3 v;
				it->second->getValue<vector3>(v);
				out << v;
			}
			else*/ if (type == "int")
			{
				int v;
				it->second->getValue<int>(v);
				out << v;
			}
			else
				out << "(unhandled value type)";
			out << "\n";
		}
		out << "   )\n";
		return out;
	}


} // namespace yake

#endif
