#include "editor/property_list_base.h"

namespace yake
{
namespace gui 
{

// defaults
float property_list_base::defaults::size::height = 0.05f;
float property_list_base::defaults::size::width = 0.2f;
float property_list_base::defaults::position::x = 0.1f;
float property_list_base::defaults::position::y = 0.1f;

property_list_base::property_list_base(const info & this_info) 
{
	// if the user has set a pointer then bind user pointer to this
	if(this_info.m_bind_to_this) *const_cast<info &>(this_info).m_bind_to_this = this;

	// set properties
	m_impl.get_impl().setSize(CEGUI::Size(this_info.m_w, this_info.m_h));
	m_impl.get_impl().setPosition(CEGUI::Point(this_info.m_x, this_info.m_y));  
}

CEGUI::Window & property_list_base::getImpl() const
{ return m_impl.get_impl(); }

void property_list_base::add_item(
		const reflection::Property & prop, 
		const yake::gui::control_base & child, 
		unsigned int col, 
		unsigned int row)
{ m_impl.add_item(prop, child, col, row); }

void property_list_base::reset_list()
{ m_impl.reset_list(); }

} // namespace gui
} // namespace yake