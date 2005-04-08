#ifndef _EVENT_ADAPTOR_H_
#define _EVENT_ADAPTOR_H_

#include <yake/reflection/reflection.h>
#include <yake/reflection/events.h>
#include <yake/reflection/bind.h>

#include <CEGUI.h>

// todo: move into cegui plugin
namespace yake
{
namespace gui
{

using namespace std;

class control_base;

template<typename Args_policy>
struct ce_event_adaptor : public reflection::event<typename Args_policy::parameter1>
{
	typedef reflection::event<typename Args_policy::parameter1> base;

	// attach this event adaptor as cegui handler and forward the calls
	ce_event_adaptor(const CEGUI::EventSet & event_set, const char * name, const control_base & parent)
		: m_parent(parent)
	{
		const_cast<CEGUI::EventSet&>(event_set).
			subscribeEvent(name, boost::bind(&ce_event_adaptor::on_event, this, _1));
	}

	// convert the event args and forward the event fired from cegui
	bool on_event(const CEGUI::EventArgs & args)
	{
		// extract parameter and fire adapted event
		base::fire(typename Args_policy::extract(m_parent, args));
		return true;
	}

	void attach_handlers(const handlers & handlers)
	{
		for(handlers::const_iterator iter(handlers.begin());
			iter != handlers.end(); iter++)
		{	attach_handler(**iter); }
	}

#ifndef YAKE_GUI_DISABLE_LUA
	void attach_handlers(const lua_functor_list & handlers)
	{
		for(lua_functor_list::const_iterator iter(handlers.begin());
			iter != handlers.end(); iter++)
		{	attach_handler(*iter); }
	}
#endif // YAKE_GUI_DISABLE_LUA

	const control_base & m_parent;
};

struct ce_clicked_args_policy
{
	typedef const control_base & parameter1;
	// todo return tuple with data from event args
	static parameter1 extract(const control_base & parent, const CEGUI::EventArgs & args)
	{
		// todo: convert cegui args and define more event_args_policies
		return parent; //static_cast<const CEGUI::WindowEventArgs&>(args).window;
	};  
};

typedef ce_event_adaptor<ce_clicked_args_policy> ce_clicked_event_adaptor;

} // namespace gui
} // namespace yake

#endif _EVENT_ADAPTOR_H_