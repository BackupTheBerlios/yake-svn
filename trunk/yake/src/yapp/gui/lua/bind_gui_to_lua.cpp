#include <yapp/gui/lua/bind_gui_to_lua.h>

#include <yapp/gui/default_window.h>
#include <yapp/gui/button_base.h>
#include <yapp/gui/static_text_base.h>

#include <luabind/return_reference_to_policy.hpp>
#include <luabind/iterator_policy.hpp>

namespace yake
{
namespace gui
{

// todo: needed for soft wiring
template <typename ReturnType>
static ReturnType & get_registered(const char * name)
{	
	Registered * reg_obj = Registered::instances_map[name];
	if(!reg_obj) throw exception(); // todo: debug info
	return static_cast<ReturnType&>(*reg_obj); 
}

template <typename Base, typename Final>
static Final * dynamic_cast_to(const Base * base)
{	return dynamic_cast<Final *>(const_cast<Base *>(base)); }

template <typename Base, typename Final>
static Final * safe_dynamic_cast_to(const Base * base)
{	
	Final * final = dynamic_cast<Final *>(const_cast<Base *>(base));
	if(!final) throw exception(); // todo: debug info
	return final;
}

void bind_gui_to_lua()
{
	using namespace luabind;

	// widget infos
	module(L)
	[
		class_<button>("button")
			.def(constructor<>())
			.def(constructor<const char *>())
			.def("caption", &button::caption, return_reference_to(self)) // todo: return_reference_to(_1) luabind 7 rc3 problem: http://sourceforge.net/mailarchive/message.php?msg_id=7990099
			.def("pos", (button & (button::*)(float, float)) &button::pos, return_reference_to(self))
			//.def("absolute_position", (button & (button::*)(float, float)) &button::absolute_position, return_reference_to(self))
			.def("clicked", &button::clicked, return_reference_to(self)),
		class_<static_text>("static_text")
			.def(constructor<>())
			.def(constructor<const char *>())
	];

	// concrete widgets
	module(L)
	[
		class_<control_base>("control_base"), 
		class_<container_control_base>("container_control_base")
			.def("add", &container_control_base::add),
		class_<window_base>("window_base")
			.def("run", &window_base::run)
			.def("get_client_area", &window_base::get_client_area)
			.def("add", (window_base & (window_base::*)(const button &)) &window_base::add<button>, return_reference_to(self)) //todo: typelist of concrete widgets we want to bind => centralizing widget registration ...
			.def("add", (window_base & (window_base::*)(const static_text &)) &window_base::add<static_text>, return_reference_to(self))
			.property("childs", &window_base::get_childs, return_stl_iterator)
			.property("title", &window_base::get_title, &window_base::set_title),
		class_<default_window, window_base>("default_window")
			.def(constructor<const char *>())
	];

	// utility
	module(L)
	[
		def("safe_dynamic_cast_to_button", (button_base * (*)(const gui::control_base *)) &safe_dynamic_cast_to<control_base, button_base>),
		def("dynamic_cast_to_button", (button_base * (*)(const gui::control_base *)) &dynamic_cast_to<control_base, button_base>),
		//def("cast_to_button", (button_base & (*)(ControlBase &)) &cast_to<ControlBase, button_base>), // todo
		def("get_windows", &get_windows, return_stl_iterator),
		//def("get_button", &get_registered<Button>), todo: needed for soft wiring
		class_<point>("point")
			.def(constructor<float, float>())
			.property("x", &point::get_x, &point::set_x)
			.property("y", &point::get_y, &point::set_y)
	];
}

} // namespace gui
} // namespace yake