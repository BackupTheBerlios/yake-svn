#ifndef _YAPP_GUI_CONTAINER_CONTROL_BASE_H_
#define _YAPP_GUI_CONTAINER_CONTROL_BASE_H_

#include "config.h"
#include "control_base.h"

#include <yapp/gui/gui_system_base.h>

namespace yake
{
namespace gui
{

class container_control_base : public control_base
{
public: // types
	typedef std::list<const control_base *> child_list;

  template<typename ConcreteInfo, typename ConcreteControl>
  struct info_base : control_base::info_base<ConcreteInfo, ConcreteControl>
  {
    template<typename ConcreteInfoRHS, typename ConcreteControlRHS>
    info_base & operator<<(const control_base::info_base<ConcreteInfoRHS, ConcreteControlRHS> & info)
    {
      ConcreteControlRHS * control = new ConcreteControlRHS(static_cast<const ConcreteInfoRHS&>(info));
      m_controls.push_back(control);
      return *this;
    }

    template<typename ConcreteInfoRHS, typename ConcreteControlRHS>
    info_base & operator<(const control_base::info_base<ConcreteInfoRHS, ConcreteControlRHS> & info) 
		{ return *this << info; }

    template<typename ConcreteInfoRHS, typename ConcreteControlRHS>
    info_base & operator^(const control_base::info_base<ConcreteInfoRHS, ConcreteControlRHS> & info) 
	  { return *this << info; }
    
		child_list m_controls;
  };

public: // construction
  template<typename ConcreteInfo, typename ConcreteControl>
  void add_for_info(const info_base<ConcreteInfo, ConcreteControl> & info)      
  {
		for( child_list::const_iterator iter(info.m_controls.begin()); 
			iter != info.m_controls.end(); ++iter)
		{ add(**iter); }
  }
 
public: // mutator
  virtual void add(const control_base & child) = 0;

public: // accessor
	virtual const child_list & get_childs() const = 0;
	virtual void remove(const control_base & child) = 0;
};

template<typename ConcreteInfo, typename ControlBase>
container_control_base & operator<<(container_control_base & container, 
		const control_base::info_base<ConcreteInfo, ControlBase> & info)
{
  //ConcreteControl * control = new ConcreteControl(static_cast<const ConcreteInfo &>(info));
	ControlBase * control = get_gui_system()->create<ControlBase>(static_cast<const ConcreteInfo &>(info));
  container.add(*control);
  return container;
}

template<typename ConcreteInfo, typename ControlBase>
container_control_base & operator<(container_control_base & container, 
		const control_base::info_base<ConcreteInfo, ControlBase> & info) 
{	return container << info; }

template<typename ConcreteInfo, typename ControlBase>
container_control_base & operator^(container_control_base & container, 
		const control_base::info_base<ConcreteInfo, ControlBase> & info) 
{	return container << info; }

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_CONTAINER_CONTROL_BASE_H_