#ifndef _CODE_GENERATOR_H_
#define _CODE_GENERATOR_H_

#include "../config.h"
#include "../compare_sequences.h"

/*
class 'chat_window' (default_window)

function chat_window:__init() super()
	self.my_added_var = 100
	-- generate: window 'chat window'
	self.title = 'chat window'
	-- generate: button 'hello'
	self.add(button():caption('hello'))
end

function chat_window:my_fnc()
end
*/

namespace yake
{
namespace gui
{

// todo: are there functions/properties the widget serializer could share between python or lua code generation?!
// e.g. dispatch_widget_type etc.
struct lua_code {};


template <class Language>
struct widget_serializer
{
	static std::string generate(const window_base & window)
	{
		// construct class name
		std::string class_name = window.get_title();
		for(int pos = class_name.find(' '); pos != std::string::npos; 
			pos = class_name.find(' ', pos + 1))
		{ class_name[pos] = '_'; }

		// [prolog] window class and creator function code
		std::string creator_function =
			"class '" + class_name + "' (default_window)\n" +
			"\n" +
			"function " + class_name + ":__init() super('" + window.get_title() + "')\n";/* +
			"  self.property = value\n";*/ // todo: add window properties using reflected properties
		
		// childs
		for(container_control_base::child_list::const_iterator widget_iter = window.get_childs().begin();
			widget_iter != window.get_childs().end(); widget_iter++)
		{
			// get widget type info
			const type_info & widget_type_info = (*widget_iter)->getClass().get_type_info();
			std::string widget_type_string = dispatch_widget_type(widget_type_info);
      // add widget info
			creator_function += "  self:add(" + widget_type_string + "()";
			// add widget properties
			for(reflection::Class::PropertyList::const_iterator prop_iter = (*widget_iter)->getClass().getProperties().begin();
				prop_iter != (*widget_iter)->getClass().getProperties().end(); prop_iter++)
			{ creator_function += std::string(":") + prop_iter->getName() + '(' + dispatch_property_value(*prop_iter, **widget_iter) + ')'; }
			// end
			creator_function += ")\n";
		}

		// [epilog]
		creator_function += "end";
		return creator_function;
	}

private:
	static std::string dispatch_widget_type(const std::type_info & type_info)
	{
		// check whether all widget types are handled or not at compile time
		// todo: use macro, see config.h
		BOOST_STATIC_ASSERT((
			compare_sequences
			< 
				implemented_widgets, // conditional types
				boost::mpl::list // handled/implemented (by this function) types
				<
					button_base, 
					static_text_base,
					multi_line_edit_box_base
				>
			>::type::value));

    // returning the name of the widget info type
		if(type_info == typeid(button_base)) return "button";
		else if(type_info == typeid(static_text_base)) return "static_text";
		else if(type_info == typeid(multi_line_edit_box_base)) return "multi_line_edit_box";

		// error: there was an unknown widget - you should never arrive here at runtime, 
		// correct type lists used by the static assert above, which should intercept the 
		// error at compile time, and implement the missing widget type if needed
		throw exception();
	}

	static std::string dispatch_property_value(const reflection::Property & prop, const control_base & widget)
	{
		// check whether all property types are handled or not at compile time
		BOOST_STATIC_ASSERT((
			compare_sequences
			< 
				implemented_properties, // conditional types
				boost::mpl::list // handled/implemented (by this function) types
				<
					bool,
					float,
					const char *,
					const point &	
				> 
			>::type::value));

		// returning the formated value of the property
		const std::type_info & type_info = prop.getTypeInfo();
		if(type_info == typeid(const char *))
		{
			return std::string("'") + prop.get<const char *>(&widget) + "'";
		}
		else if(type_info == typeid(float))
		{
			return boost::lexical_cast<std::string>(prop.get<float>(&widget));
		}
		else if(type_info == typeid(const point &))
		{
			return boost::lexical_cast<std::string>(prop.get<const point &>(&widget).x) + ',' 
				+ boost::lexical_cast<std::string>(prop.get<const point &>(&widget).y);
		}
		else if(type_info == typeid(bool))
		{
			return (prop.get<bool>(&widget) ? "true" : "false");
		}

		// error: there was an unknown property - you should never arrive here at runtime, 
		// correct type lists used by the static assert above, which should intercept the 
		// error at compile time, and implement the missing property type if needed
		throw exception(); 
	}
};

} // namespace gui
} // namespace yake

#endif // _CODE_GENERATOR_H_