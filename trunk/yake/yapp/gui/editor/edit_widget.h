#ifndef _YAPP_GUI_EDITOR_EDIT_WIDGET_H_
#define _YAPP_GUI_EDITOR_EDIT_WIDGET_H_

#include <yapp/gui/control_base.h>
#include <yapp/plugins/gui_cegui/editor/edit_widget.h> // todo remove

namespace yake
{
namespace gui
{

class edit_widget : public control_base
{
public:
	edit_widget(const widget_editor & editor, const control_base & child)
		: m_impl(*this, editor, child.getImpl()),
			m_child(child)
	{}

public: // accessors  
  virtual CEGUI::Window & getImpl() const
	{ return m_impl; }

	const control_base & get_child() const
	{ return m_child; }

private: // data
	mutable plugin::edit_widget m_impl; // todo: plugin should derive from edit_widget_base
	const control_base & m_child;
};

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_EDITOR_EDIT_WIDGET_H_