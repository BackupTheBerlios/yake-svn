#include <yapp/gui/editor/edit_window_visitor.h>
#include <yapp/gui/editor/widget_editor.h>

namespace yake
{
namespace gui
{

edit_window_visitor::edit_window_visitor(const widget_editor & editor, container_control_base & controls_container)
	: m_controls_container(controls_container)
{
	// duplicate container, otherwise the iteration would not work, since we are adding edit_widget controls
	container_control_base::child_list childs = controls_container.get_childs();

	// add edit_widget to each widget
	for(container_control_base::child_list::iterator iter = childs.begin();
		iter != childs.end(); iter++)
	{	controls_container.add(*new edit_widget(editor, **iter)); }
}

} // namespace gui
} // namespace yake