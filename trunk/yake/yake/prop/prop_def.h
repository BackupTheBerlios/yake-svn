#ifndef YAKE_PROP_DEF_H
#define YAKE_PROP_DEF_H

#include "prerequisites.h"

namespace yake {

	struct Prop;
	struct PropDef
	{
		PropDef(const bool readOnly = false) : readOnly_(readOnly) {}
		PropDef(const PropDef& other) :
		name_(other.name_),
			description_(other.description_),
			type_(other.type_),
			defaultValue_(other.defaultValue_)
		{}
		PropDef& operator = (const PropDef& rhs)
		{
			name_ = rhs.name_;
			description_ = rhs.description_;
			type_ = rhs.type_;
			defaultValue_ = rhs.defaultValue_;
			return *this;
		}
		template<typename T>
		PropDef(const std::string& name, const std::string& desc, const T& defaultValue, const bool readOnly = false) :
		name_(name),
			description_(desc),
			defaultValue_(defaultValue),
			type_(typeid(T).name()),
			readOnly_(readOnly)
		{
		}
		std::string name() const
		{ return name_; }
		std::string description() const;
		std::string type() const
		{ return type_; }
		Prop* create() const;
		boost::any getDefaultValue() const
		{ return defaultValue_; }
	private:
		std::string		name_;
		std::string		description_;
		std::string		type_;
		boost::any		defaultValue_;
		bool				readOnly_;
	};
	inline std::ostream& operator << (std::ostream& out, const PropDef& propdef)
	{
		out << "PropDef(name='" << propdef.name() << "', type='" << propdef.type() << "')";
		return out;
	}
} // namespace yake

	#include "prop.h"
namespace yake {
	inline Prop* PropDef::create() const
	{ return new Prop(*this,defaultValue_,readOnly_); }
} // namespace yake

#endif