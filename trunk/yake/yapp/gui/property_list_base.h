#ifndef _YAPP_GUI_PROPERTY_LIST_BASE_H_
#define _YAPP_GUI_PROPERTY_LIST_BASE_H_

#include "plugin/property_list.h" // todo remove

namespace yake
{
namespace gui 
{

class property_list_base : public control_base
{
public: // types
  struct info : info_base<info, property_list_base> {};

	static struct defaults
	{
		static struct size { static float height, width; };
		static struct position { static float x, y; };
	};

public: // constructors
	property_list_base()
	{}

  property_list_base(const info & this_info);

public: // accessors  
  virtual CEGUI::Window & getImpl() const;

public: // mutators
	virtual void add_item(
		const reflection::Property & prop, 
		const yake::gui::control_base & child, 
		unsigned int col, 
		unsigned int row) /*= 0*/;

	virtual void reset_list() /*= 0*/;

private: // data
	mutable cegui::property_list m_impl; // todo: should derive from property_list_base and using abstract factory
};

typedef property_list_base::info property_list;

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_PROPERTY_LIST_BASE_H_