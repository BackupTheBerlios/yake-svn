#ifndef _YAPP_GUI_EDITOR_EDIT_WINDOW_VISITOR_H_
#define _YAPP_GUI_EDITOR_EDIT_WINDOW_VISITOR_H_

#include <boost/shared_ptr.hpp>

#include "edit_widget.h"
#include "container_control_base.h"

namespace yake
{
namespace gui
{

class widget_editor;

class edit_window_visitor
{
public:
	typedef std::vector< boost::shared_ptr<edit_widget> > edit_widget_list;

	edit_window_visitor(const widget_editor & editor, container_control_base & controls_container);

	~edit_window_visitor()
	{
		for(edit_widget_list::iterator iter = m_edit_widgets.begin();
			iter != m_edit_widgets.end(); iter++)
		{ m_controls_container.remove(*iter->get()); }
	}

	const container_control_base & get_container() const
	{ return m_controls_container; }

private:
	container_control_base & m_controls_container;
	edit_widget_list m_edit_widgets;
};

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_EDITOR_EDIT_WINDOW_VISITOR_H_