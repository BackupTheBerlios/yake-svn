#ifndef _YAPP_GUI_CONTROL_BASE_H_
#define _YAPP_GUI_CONTROL_BASE_H_

#include <CEGUI.h>
#include <yake/reflection/reflection.h>

namespace yake
{
namespace gui
{

// todo: generic ... not bound to cegui
class control_base
{
public: // types
  template<typename ConcreteInfo, typename ConcreteControl> 
	struct info_base 
	{
		info_base() 
			: m_x(ConcreteControl::defaults::position::x),
				m_y(ConcreteControl::defaults::position::y),
				m_w(ConcreteControl::defaults::size::width),
				m_h(ConcreteControl::defaults::size::height),
				m_bind_to_this(NULL),
				m_abs_pos(false),
				m_visible(true)
		{}

		ConcreteInfo & pos(float x, float y) 
		{ m_x=x; m_y=y; return static_cast<ConcreteInfo&>(*this); }

		ConcreteInfo & absolute_position(float x, float y) 
		{ m_x=x; m_y=y; m_abs_pos=true; return static_cast<ConcreteInfo&>(*this); }

		ConcreteInfo & size(float width, float height) 
		{ m_w=width; m_h=height; return static_cast<ConcreteInfo&>(*this); }

		ConcreteInfo & visible(bool visible) 
		{ m_visible=visible; return static_cast<ConcreteInfo&>(*this); }
	
		ConcreteInfo & bind(ConcreteControl *& bind_to_this) 
		{ m_bind_to_this = &bind_to_this; return static_cast<ConcreteInfo&>(*this); }

		float m_x, m_y, m_w, m_h;
		bool m_abs_pos, m_visible;
		ConcreteControl ** m_bind_to_this;
	};

public: // construction and destruction
  virtual ~control_base() {}

public: // implementation-specific accessors  
  virtual CEGUI::Window & getImpl() const = 0; // todo: remove

	CLASS(control_base, reflection::NullClass);
};

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_CONTROL_BASE_H_