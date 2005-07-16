#include "gui_test.h"
#include "editor/code_generator.h"
#include "bind_gui_to_lua.h"

#include "button_base.h"
#include "static_text_base.h"
#include "multi_line_edit_box_base.h"

namespace yake
{

namespace // unnamed
{
	button_base * g_fire_button = NULL;
	multi_line_edit_box_base * g_cout = NULL;
}

static button_base * get_fire_button() { return g_fire_button; }
static multi_line_edit_box_base * get_cout_box() { return g_cout; }

static void forward_cout_here(const std::string & data)
{
	// cout content
  static std::string content;
	// out
	if(g_cout) g_cout->set_text(content += data);
}

static void forward_cout_here_lua(const std::string & data)
{
	// add new line if needed
	if(*data.rbegin() != '\n') const_cast<std::string &>(data) += '\n';
	// out
	forward_cout_here("lua: " + data);  
}

static bool	handle_error_box(const CEGUI::EventArgs & e)
{
	CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"ErrorBox")->hide();
	return true;
}

static void do_error_box(const char * text)
{
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* root = winMgr.getWindow((CEGUI::utf8*)"root_wnd");

	CEGUI::FrameWindow* errbox;

	try
	{
		errbox = (CEGUI::FrameWindow*)winMgr.getWindow((CEGUI::utf8*)"ErrorBox");
	}
	catch(CEGUI::UnknownObjectException x)
	{
		// create frame window for box
		CEGUI::FrameWindow* fwnd = (CEGUI::FrameWindow*)winMgr.createWindow((CEGUI::utf8*)"TaharezLook/FrameWindow", (CEGUI::utf8*)"ErrorBox");
		root->addChildWindow(fwnd);
		fwnd->setPosition(CEGUI::Point(0.1, 0.25f));
		fwnd->setMaximumSize(CEGUI::Size(1.0f, 1.0f));
		fwnd->setSize(CEGUI::Size(0.7f, 0.5f));
		fwnd->setText((CEGUI::utf8*)"CEGUI Demo - Info!");
		fwnd->setDragMovingEnabled(false);
		fwnd->setSizingEnabled(false);
		fwnd->setAlwaysOnTop(true);
		fwnd->setCloseButtonEnabled(false);

		// create error text message
		CEGUI::StaticText* msg = (CEGUI::StaticText*)winMgr.createWindow((CEGUI::utf8*)"TaharezLook/StaticText", (CEGUI::utf8*)"ErrorBox/Message");
		fwnd->addChildWindow(msg);
		msg->setPosition(CEGUI::Point(0.1f, 0.1f));
		msg->setSize(CEGUI::Size(0.9f, 0.5f));
		msg->setVerticalFormatting(CEGUI::StaticText::VertCentred);
		msg->setHorizontalFormatting(CEGUI::StaticText::HorzCentred);
		msg->setBackgroundEnabled(false);
		msg->setFrameEnabled(false);

		// create ok button
		CEGUI::PushButton* btn = (CEGUI::PushButton*)winMgr.createWindow((CEGUI::utf8*)"TaharezLook/Button", (CEGUI::utf8*)"ErrorBox/OkButton");
		fwnd->addChildWindow(btn);
		btn->setPosition(CEGUI::Point(0.3f, 0.80f));
		btn->setSize(CEGUI::Size(0.4f, 0.1f));
		btn->setText((CEGUI::utf8*)"Okay!");

		// subscribe event
		btn->subscribeEvent(CEGUI::PushButton::EventClicked, boost::bind(&handle_error_box, _1));

		errbox = fwnd;
	}

	errbox->getChild((CEGUI::utf8*)"ErrorBox/Message")->setText((CEGUI::utf8*)text);
	errbox->show();
	errbox->activate();
}


gui_test::gui_test()
{
	yake::gui::bind_gui_to_lua();

	using namespace luabind;

	// forward print()
	module(L)[ def("forward_cout_here_lua", &forward_cout_here_lua) ];
	dostring(L, "function print(text) forward_cout_here_lua(text) end"); 
	// todo: add call to original print: "call(%print, text)"

	// menu
	m_win_menu = create<window_base>("menu")
		.pos(0.525f, 0.69f)
		.size(0.2f, 0.3f)
		.sizing_enabled(false);

	m_win_menu 
		 << button("events")
			   .pos(0.1f, 0.12f)
		     .size(0.8f, 0.1f)
			   .clicked(handle_events_window, this)
	   << button("code gen")
				 .pos(0.1f, 0.27f)
				 .size(0.8f, 0.1f)
				 .clicked(handle_codegen_window, this)
		 << button("editor")
				 .pos(0.1f, 0.42f)
				 .size(0.8f, 0.1f)
				 .clicked(handle_editor_window, this)
     << button("console")
				 .pos(0.1f, 0.57f)
				 .size(0.8f, 0.1f)
				 .clicked(handle_console_window, this)
     << button("playground")
				 .pos(0.1f, 0.72f)
				 .size(0.8f, 0.1f)
				 .clicked(handle_playground_window, this)
		 << button("quit")
				 .pos(0.1f, 0.87f)
				 .size(0.8f, 0.1f)
				 .clicked(handle_quit, this);

	// cout
	m_win_console = create<window_base>("cout")
		.pos(0.01f, 0.69f)
		.size(0.5f, 0.3f)
		.sizing_enabled(false);

	m_win_console
		 << multi_line_edit_box("redirected cout output goes here")
				 .pos(0.01f, 0.1f)
				 .size(0.98f, 0.9f)
				 .bind(g_cout);

	// events
	m_win_events = create<window_base>("events")
		.pos(0.4f, 0.15f)
		.size(0.5f, 0.5f)
		.sizing_enabled(false)
		.visible(false);

	m_win_events 
		 << static_text("lua:")
				 .bg_enabled(false)
				 .frame_enabled(false)
				 .pos(0.02f, 0.05f)
				 .size(0.1f, 0.1f)
		 << multi_line_edit_box()
				 .bind(m_events_win_lua_code_widget)
				 .text("function get_window_by_title(title)\n"
							 " for win in get_windows() do\n"
							 "  if win.title == title then return win end\n"
							 " end\n"
							 "end\n"
							 "\n"
					
							"function get_button_by_caption(window, caption)\n"
							" for child in window.childs do\n"
							"  but = dynamic_cast_to_button(child)\n"
							"  if (but ~= nil) and (but.caption == caption) then return but end\n"
							" end\n"
							"end\n"
							"\n"

							"function get_button(window_title, button_caption)\n"
							" return get_button_by_caption(get_window_by_title(window_title), button_caption)\n"
							"end\n"
							"\n"
							"function lua_clicked_handler(widget)\n"
							" print('lua_clicked_handler(): LUA here I am!')\n"
							"end\n"
							"\n"
							"get_button('events', 'fire events').clicked:attach_handler(lua_clicked_handler)\n"
							"print('attached handlers, click fire event!')\n"
							"\n"

							"function lua_clicked_second_handler(widget)\n"
							" print('lua_clicked_second_handler(): LUA here I am!')\n"
							" but = safe_dynamic_cast_to_button(widget) -- or assert(dynamic_cast_to_button(widget)) where assert returns the ptr\n"
							" --but.absolute_position = point(250, 100)\n"
							" but.pos.y = but.pos.y + 0.05\n"
							"end\n"
							"\n"
							"function create_win(id)\n"
							"  return default_window('win: ' .. id)\n"
							"    :add(button('but' .. id):caption('but: ' .. id))\n"
							"    :add(button('fire'):pos(0.4, 0.1):clicked(lua_clicked_second_handler))\n"
							"    :run()\n"
							"end\n"
							"win = create_win('this_id')")
				 .pos(0.02f, 0.13f)
				 .size(0.96f, 0.8f)
		 << button("run lua")
				 .pos(0.02f, 0.94f)
				 .size(0.2f, 0.05f)
				 .clicked(handle_events_run_lua, this)
		 << button("run c++")
				 .pos(0.25f, 0.94f)
				 .size(0.2f, 0.05f)
				 .clicked(handle_soft_wire, this)
		 << button("fire events")
				 .pos(0.48f, 0.94f)
				 .size(0.2f, 0.05f)
				 .bind(g_fire_button);

	// code gen
	m_win_codegen = create<window_base>("code gen")
		.pos(0.4f, 0.15f)
		.size(0.5f, 0.5f)
		.sizing_enabled(false)
		.visible(false);

	m_win_codegen
		 << static_text("todo: dropdown box for windows")
				 .pos(0.02f, 0.08f)
				 .size(0.2f, 0.05f)
		 << button("generate")
				 .pos(0.25f, 0.08f)
				 .size(0.2f, 0.05f)
		 << button("run lua")
				 .pos(0.48f, 0.08f)
				 .size(0.2f, 0.05f)
		 << multi_line_edit_box()
				 .text(widget_serializer<lua_code>::generate(m_win_events) + "\nwin = events()\nwin:run()")
				 .pos(0.02f, 0.15f)
				 .size(0.96f, 0.80f);

	// editor
	m_win_editor = create<window_base>("editor")
		.pos(0.1f, 0.1f)
		.size(0.2f, 0.5f)
		.sizing_enabled(false)
		.visible(false);

	property_list_base * prop_list = NULL;
	m_win_editor
		 << property_list()
				 .pos(0.06f, 0.08f)
				 .size(0.85f, 0.8f)
				 .bind(prop_list);

	// editable window
	m_win_editable = create<window_base>("editable window")
		.pos(0.45f, 0.1f)
		.size(0.5f, 0.5f)
		.sizing_enabled(false)
		.visible(false);

	m_win_editable 
		 << button("drag me")
				 .pos(0.02f, 0.6f)
				 .size(0.2f, 0.05f)
		 << static_text()
				 .text("static text, or me!")
				 .pos(0.1f, 0.1f)
				 .size(0.4f, 0.4f)
		 << multi_line_edit_box()
				 .text("multi line edit box, drag me!")
				 .pos(0.6f, 0.6f)
				 .size(0.3f, 0.3f);

	// playground
	m_win_playground = create<window_base>("playground")
		.pos(0.4f, 0.15f)
		.size(0.5f, 0.5f)
		.sizing_enabled(false)
		.visible(false);

	m_win_playground
		 << static_text("lua:")
				 .bg_enabled(false)
				 .frame_enabled(false)
				 .pos(0.02f, 0.05f)
				 .size(0.1f, 0.1f)
		 << multi_line_edit_box()
				 .text("function print_window_names()\n"
				       " nr = 0\n"
							 " for win in get_windows() do\n"
							 "  nr = nr + 1\n"
							 "  print('window ' .. nr .. ' : ' .. win.title)\n"
							 " end\n"
							 "end\n"
							 "print_window_names()")
				 .pos(0.02f, 0.13f)
				 .size(0.96f, 0.8f)
				 .bind(m_playground_win_lua_code_widget)
		 << button("run lua")
				 .pos(0.02f, 0.94f)
				 .size(0.2f, 0.05f)
				 .clicked(handle_playground_run_lua, this);

	// init windows
	m_win_menu.run();
	m_win_console.run();
	m_win_events.run();
	m_win_codegen.run();
	m_win_editor.run();
	m_win_editable.run();
	m_win_playground.run();

	// init editor
	m_editor.reset(new widget_editor(*prop_list));
	m_editor->edit(m_win_editable.get_client_area());
}

bool gui_test::on_clicked(const gui::control_base & widget)
{
	do_error_box( CEGUI::String( "CEGUIOgre_TestDriver1::on_clicked( " + widget.getImpl().getName() + " )" ).c_str() );
	return true;
}

void gui_test::handle_soft_wire(const gui::control_base & widget)
{
	// todo: validation
	/*if( !traits_registry_.validate( "PushButton::Clicked", "pawn::on_clicked" ) )
	{
		doErrorBox( "Signatures do not match." );
		return true;
	}
	// soft wiring
	object_registry_.soft_wire( "Demo7/Window1/YakeClick.Clicked", "my_pawn.on_clicked" );
	object_registry_.soft_wire( "Demo7/Window1/YakeClick.Clicked", "testdriver1.on_clicked" );*/

	// todo: soft wiring
	/*Registered * obj_handler = Registered::instances_map["my_pawn"];
	const Method & handler = obj_handler->getClass().getMethod("on_clicked");

	Registered * obj_event = Registered::instances_map["Demo7/Window1/YakeClick"];
	obj_event->getClass().getEvent("clicked").attach_handler(obj_event, handler, obj_handler);*/

	g_fire_button->clicked.attach_handler(&pawn::on_clicked, &m_pawn);

	do_error_box( "Signatures do match and are soft-wired now." );
}

void gui_test::handle_quit(const gui::control_base & widget)
{
	Ogre::Root::getSingleton().queueEndRendering();
}

void gui_test::handle_events_window(const gui::control_base & widget)
{
	m_win_events.set_visible(!m_win_events.get_visible());
}

void gui_test::handle_codegen_window(const gui::control_base & widget)
{
	m_win_codegen.set_visible(!m_win_codegen.get_visible());
}

void gui_test::handle_editor_window(const gui::control_base & widget)
{
	m_win_editor.set_visible(!m_win_editor.get_visible());
	m_win_editable.set_visible(!m_win_editable.get_visible());
}

void gui_test::handle_console_window(const gui::control_base & widget)
{
	m_win_console.set_visible(!m_win_console.get_visible());
}

void gui_test::handle_playground_window(const gui::control_base & widget)
{
	m_win_playground.set_visible(!m_win_playground.get_visible());
}

void gui_test::handle_events_run_lua(const gui::control_base & widget)
{
	std::cout << "executing lua code...\n";
	dostring(L, static_cast<const multi_line_edit_box_base &>(*m_events_win_lua_code_widget).get_text());
}

void gui_test::handle_playground_run_lua(const gui::control_base & widget)
{
	std::cout << "executing lua code...\n";
	dostring(L, static_cast<const multi_line_edit_box_base &>(*m_playground_win_lua_code_widget).get_text());
}

} // namespace yake