#include <yapp/gui/editor/edit_widget.h>
#include <yapp/gui/editor/widget_editor.h>
#include <yapp/plugins/guiCEGUI/editor/edit_widget.h>

namespace plugin
{

float edit_widget::defaults::placement_step_size = 2.0f;
float edit_widget::defaults::frame_line_width = 2.0f;
float edit_widget::defaults::frame_corner_size = 6.0f;
std::string edit_widget::defaults::imageset_name = "TaharezLook";
std::string edit_widget::defaults::mouse_cursor_move = "MouseMoveCursor";
std::string edit_widget::defaults::mouse_cursor_resize_nw_se = "MouseNwSeCursor";
std::string edit_widget::defaults::mouse_cursor_resize_no_so = "MouseNoSoCursor";
std::string edit_widget::defaults::mouse_cursor_resize_ne_sw = "MouseNeSwCursor";
CEGUI::colour edit_widget::defaults::line_colour = CEGUI::colour(0.5f, 0.5f, 0.5f, 1.0f);
CEGUI::colour edit_widget::defaults::corner_normal_colour = CEGUI::colour(1.0f, 1.0f, 1.0f, 1.0f);
CEGUI::colour edit_widget::defaults::corner_highlight_colour = CEGUI::colour(0.5f, 0.5f, 0.5f, 1.0f);

edit_widget::edit_widget(const yake::gui::edit_widget & wrapper, const yake::gui::widget_editor & editor, CEGUI::Window & child)
	: CEGUI::Static("edit_widget_type", child.getName() + "__edit_widget"),
		m_wrapper(wrapper),
		m_editor(editor),
		m_child(child)
{
	// place edit_widget on top of the child window
	setPosition(CEGUI::Point(child.getXPosition()-0.01225f, child.getYPosition()-0.01225f));
	setSize(CEGUI::Size(child.getWidth()+0.02f, child.getHeight()+0.02f));
}

void edit_widget::onActivated(CEGUI::ActivationEventArgs & e)
{
	// todo: widget_editor should register to activated event of edit_widget wrapper
	CEGUI::Static::onActivated(e);
	m_editor.on_widget_changed(m_wrapper.get_child());
}

} // namespace plugin