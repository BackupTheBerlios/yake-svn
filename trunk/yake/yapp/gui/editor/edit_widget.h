#ifndef _EDIT_WIDGET_H_
#define _EDIT_WIDGET_H_

#include "control_base.h"
#include "plugin/edit_widget.h" // todo remove

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

#endif // _EDIT_WIDGET_H_