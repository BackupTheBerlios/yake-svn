#include "property_list.h"

namespace yake
{
namespace gui
{
namespace cegui
{

CEGUI::Editbox * list_item_base::m_editbox = NULL;

list_item_base::list_item_base(const CEGUI::String & text, const CEGUI::MultiColumnList & parent) 
	: ListboxTextItem(text), 
		m_parent(parent)
{
	// create edit box one-time
	static struct set_editbox_parent
	{
		set_editbox_parent(CEGUI::MultiColumnList & parent)
		{
			// create
			m_editbox = static_cast<CEGUI::Editbox*>(
				CEGUI::WindowManager::getSingleton().createWindow(
					(CEGUI::utf8*)"TaharezLook/Editbox", (CEGUI::utf8*)"edit_box"));
			// conf
			m_editbox->setVisible(false);
			m_editbox->setAlwaysOnTop(true);
			m_editbox->setSize(CEGUI::Size(0.0f, 0.1f));
			// add
			parent.addChildWindow(m_editbox);
		}
	} editbox_init(const_cast<CEGUI::MultiColumnList&>(parent));

	// set brush
	setSelectionBrushImage((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MultiListSelectionBrush");
}


namespace // unnamed
{
	template <typename Value, typename Object>
	struct get_property_value_helper; // throw compile-time error

	template <typename Object>
	struct get_property_value_helper<bool, Object>
	{
		static std::string apply(const reflection::Property & prop, const Object & obj)
		{
			return (prop.get<bool>(&obj) ? "true" : "false"); 
		}
	};

	template <typename Object>
	struct get_property_value_helper<float, Object>
	{
		static std::string apply(const reflection::Property & prop, const Object & obj)
		{
			return boost::lexical_cast<std::string>(prop.get<float>(&obj)); 
		}
	};

	template <typename Object>
	struct get_property_value_helper<const char *, Object>
	{
		static std::string apply(const reflection::Property & prop, const Object & obj)
		{
			return std::string(prop.get<const char *>(&obj)); 
		}
	};

	template <typename Object>
	struct get_property_value_helper<const yake::gui::point &, Object>
	{
		static std::string apply(const reflection::Property & prop, const Object & obj)
		{
			const yake::gui::point & value = prop.get<const yake::gui::point &>(&obj);
			return boost::lexical_cast<std::string>(value.x) + POINT_PROPERTY_DELIMITER + boost::lexical_cast<std::string>(value.y); 
		}
	};

} // namespace unnamed

template <typename Value, typename Object>
std::string get_property_value(const reflection::Property & prop, const Object & obj)
{ return get_property_value_helper<Value, Object>::apply(prop, obj); }


list_item_base & create_list_item(
		const reflection::Property & prop, 
		const yake::gui::control_base & widget, 
		CEGUI::MultiColumnList & mclbox)
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

	// dispatch property type and create an concrete list item for it
	if(prop.getTypeInfo() == typeid(bool))
	{ 
		return *new list_item<bool>(get_property_value<bool>(prop, widget), mclbox, prop, widget); 
	}
	else if(prop.getTypeInfo() == typeid(float))
	{ 
		return *new list_item<float>(get_property_value<float>(prop, widget), mclbox, prop, widget); 
	}
	else if(prop.getTypeInfo() == typeid(const char *))
	{ 
		return *new list_item<const char *>(get_property_value<const char *>(prop, widget), mclbox, prop, widget); 
	}
	else if(prop.getTypeInfo() == typeid(const yake::gui::point &))
	{ 
		return *new list_item<const yake::gui::point &>(get_property_value<const yake::gui::point &>(prop, widget), mclbox, prop, widget);
	}

	// error: there was an unknown property - you should never arrive here at runtime, 
	// correct type lists used by the static assert above, which should intercept the 
	// error at compile time, and implement the missing property type if needed
	throw exception();
}

} // namespace cegui
} // namespace gui
} // namespace yake