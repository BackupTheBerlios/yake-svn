#ifndef _YAPP_GUI_EDITOR_WIDGET_EDITOR_H
#define _YAPP_GUI_EDITOR_WIDGET_EDITOR_H

#include <boost/shared_ptr.hpp>
#include <CEGUI.h>

#include "edit_window_visitor.h"

#include "../property_list_base.h"

namespace yake
{
namespace gui
{

class widget_editor
{
public:
	widget_editor(property_list_base & list_box)
		: m_property_list(list_box)
	{}

	void edit(yake::gui::container_control_base & controls_container)
	{ m_edit_window_visitor.reset(new edit_window_visitor(*this, controls_container)); }

	void cancel()
	{ m_edit_window_visitor.reset(); }

public: // todo: protected
	void on_widget_changed(const yake::gui::control_base & child) const
	{
		// clean property list
		m_property_list.reset_list();
    
		// add reflected properties to the property list
		int row = 0;
		for(reflection::Class::PropertyList::const_iterator iter = child.getClass().getProperties().begin();
			iter != child.getClass().getProperties().end(); iter++, row++)
		{
			// todo: use wrapper
			// new row
			CEGUI::MultiColumnList & mclbox = static_cast<CEGUI::MultiColumnList &>(m_property_list.getImpl());
			mclbox.addRow();

			// add values
			const reflection::Property & prop = *iter;
			mclbox.setItem(new CEGUI::ListboxTextItem(prop.getName()), 0, row);
			m_property_list.add_item(prop, child, 1, row);
		}
	}

private:
	boost::shared_ptr<edit_window_visitor> m_edit_window_visitor;
	mutable property_list_base & m_property_list;
};

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_EDITOR_WIDGET_EDITOR_H