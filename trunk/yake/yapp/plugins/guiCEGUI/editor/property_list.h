#ifndef _PROPERTY_LIST_H
#define _PROPERTY_LIST_H

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <CEGUI.h>
#include <reflection.h>

#include "../control_base.h"
#include "../point.h"
#include "../config.h"
#include "../compare_sequences.h"

namespace yake
{
namespace gui
{
namespace cegui
{

static const char POINT_PROPERTY_DELIMITER = ';';

class list_item_base : public CEGUI::ListboxTextItem
{
public:
	list_item_base(const CEGUI::String & text, const CEGUI::MultiColumnList & parent);

	void on_selected(const CEGUI::Point & position)
	{
		// place text box over the actual list entry
		m_editbox->setWidth(m_parent.getColumnHeaderWidth(1) - 0.001f);
		m_editbox->setVisible(true);
    m_editbox->setPosition(position);
		m_editbox->setText(getText());
	}

	virtual void on_focus_lost() = 0;

	const CEGUI::MultiColumnList & m_parent;
	static CEGUI::Editbox * m_editbox;
};

namespace // unnamed
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

	template <typename Value, typename Object>
	struct set_property_value_helper; // throw compile-time error

	template <typename Object>
	struct set_property_value_helper<bool, Object>
	{
		static void apply(const reflection::Property & prop, Object * obj, const char * value)
		{ prop.set(obj, (std::string(value) == "true" || std::string(value) == "1")); }
	};

	template <typename Object>
	struct set_property_value_helper<float, Object>
	{
		static void apply(const reflection::Property & prop, Object * obj, const char * value)
		{ prop.set(obj, boost::lexical_cast<float>(value)); }
	};

	template <typename Object>
	struct set_property_value_helper<const char *, Object>
	{
		static void apply(const reflection::Property & prop, Object * obj, const char * value)
		{ prop.set(obj, value); }
	};

	template <typename Object>
	struct set_property_value_helper<const yake::gui::point &, Object>
	{
		static void apply(const reflection::Property & prop, Object * obj, const char * value)
		{	
			// todo: regex check

			// convert coordinates to float
			std::string coors(value);
			float x = boost::lexical_cast<float>(coors.substr(0, coors.find(POINT_PROPERTY_DELIMITER)));
			float y = boost::lexical_cast<float>(coors.substr(coors.find(POINT_PROPERTY_DELIMITER) + 1, 
				coors.length() - coors.find(POINT_PROPERTY_DELIMITER) + 1));
			// set new coordinates
			prop.set_ref(obj, yake::gui::point(x, y)); 
		}
	};

} // namespace unnamed

template <typename Value, typename Object>
void set_property_value(const reflection::Property & prop, Object * obj, const char * value)
{ set_property_value_helper<Value, Object>::apply(prop, obj, value); }

template<class ValueType>
class list_item : public list_item_base
{
public:
	list_item(
		const CEGUI::String & text, 
		const CEGUI::MultiColumnList & parent, 
		const reflection::Property & prop, 
		const yake::gui::control_base & widget)
	: list_item_base(text, parent), 
		m_property(prop),
		m_widget(widget)
	{}

	virtual void on_focus_lost()
	{
		// we don't need to show the edit box anymore
		m_editbox->setVisible(false);

		// equalize the text of the edit box and with the text of the list item
		const char * value = m_editbox->getText().c_str();
    this->setText(value);

		// the reflected property should have the same value type
		assert(m_property.getTypeInfo() == typeid(ValueType));

		// update reflected property
		set_property_value<ValueType>(m_property, &m_widget, value);
	}

	const reflection::Property & m_property;
	const yake::gui::control_base & m_widget;
};


list_item_base & create_list_item(
		const reflection::Property & prop, 
		const yake::gui::control_base & widget, 
		CEGUI::MultiColumnList & mclbox);

// todo: create new widget for this: editable_multi_column_list_box
// property list should use that widget
class property_list //: public property_list_base, TaherzLook::MultiColumnList
{
public:
	property_list()
		: m_multibox(static_cast<CEGUI::MultiColumnList&>(
				*CEGUI::WindowManager::getSingleton().createWindow(
					(CEGUI::utf8*)"TaharezLook/MultiColumnList", (CEGUI::utf8*)"property_list")))
	{
		// todo: clean up
		m_multibox.setMaximumSize(CEGUI::Size(1.0f, 1.0f));
		m_multibox.setPosition(CEGUI::Point(0.01f, 0.1f));
		m_multibox.setSize(CEGUI::Size(0.9f, 0.38f));
		m_multibox.setSelectionMode(CEGUI::MultiColumnList::RowSingle);
		m_multibox.addColumn((CEGUI::utf8*)"Property", 0, 0.5f);
		m_multibox.addColumn((CEGUI::utf8*)"Value", 1, 0.5f);

		m_multibox.subscribeEvent(CEGUI::MultiColumnList::EventSelectionChanged, 
			boost::bind(&property_list::on_selection_changed, this, _1));
	}

	bool on_selection_changed(const CEGUI::EventArgs & args)
	{
		static list_item_base * last_item = NULL;

		// get selected item
		list_item_base * item = static_cast<list_item_base*>(
			m_multibox.getNextSelected(m_multibox.getFirstSelectedItem()));

		// checks
		if(last_item && last_item != item)	{ last_item->on_focus_lost(); }
		if(!item)	{	return true; }

		// calculate position of this item and fire
		item->on_selected(CEGUI::Point(m_multibox.getColumnHeaderWidth(0), 
				m_multibox.getItemGridReference(item).row * 0.1f + 0.15f));//m_multibox.getHighestRowItemHeight(0))); // todo: inherit from multicolbox and use functions to get the height of the row
		// todo: does not work anymore

		// store
		last_item = item;		

		return true;
	}

  CEGUI::MultiColumnList & get_impl() const
	{ return m_multibox; }

public: // mutators
	virtual void add_item(
		const reflection::Property & prop, 
		const yake::gui::control_base & child, 
		unsigned int col, 
		unsigned int row)
	{ m_multibox.setItem(&create_list_item(prop, child, m_multibox), col, row); }

	virtual void reset_list()
	{ m_multibox.resetList(); }

private:
	CEGUI::MultiColumnList & m_multibox;
};

} // namespace cegui
} // namespace gui
} // namespace yake

#endif // _PROPERTY_LIST_H