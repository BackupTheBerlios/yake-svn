#ifndef _GUI_TEST_H_
#define _GUI_TEST_H_

#include <string>

#include <reflection.h>
#include <bind_lua/bind_lua.h>
#undef free // todo: find conflict with tnl and move to bind_network.h
//#include <bind_network/bind_network.h> // todo: remove

#include <Ogre.h>

#include "default_window.h"
#include "forward_buffer.h"
#include "editor/widget_editor.h"

namespace yake
{
using namespace gui;

button_base * get_fire_button();
multi_line_edit_box_base * get_cout_box();

void forward_cout_here(const std::string & data);
void forward_cout_here_lua(const std::string & data);

struct forward_cout
{
	forward_cout()
		: m_forward_buf(&forward_cout_here),
			m_temp_cout(*std::cout.rdbuf())
	{
		// forward cout
		std::cout.rdbuf(&m_forward_buf);
	}

	~forward_cout()
	{
    // reset cout		
		std::cout.rdbuf(&m_temp_cout);
	}

	forward_buf m_forward_buf;
	std::streambuf & m_temp_cout;
};

struct test_entity
{
	test_entity() {}
  virtual ~test_entity() {};

	CLASS(test_entity, reflection::NullClass);
	EVENT(public, birth, (const std::string &));
};

struct pawn : public reflection::Registered, public test_entity
{
	pawn() {}
	pawn(const char * name) : reflection::Registered(name) {}

	CLASS(pawn, reflection::Registered);

	METHOD(public, void, on_birth, (const std::string & cry))
	{	std::cout << ".on_birth( \"" << cry << "\" )" << std::endl; }

	METHOD(public, void, on_close, (const bool funky))
  {	std::cout << ".on_close( " << ( funky ? "true" : "false" ) << " )" << std::endl; }

	METHOD(public, void, on_click, (const int funky))
  {	std::cout << ".on_click( " << funky << " )" << std::endl; }

	METHOD(public, void, on_clicked, (const control_base & widget))
	{	std::cout << getName() << ".on_clicked( " << widget.getImpl().getName().c_str() << " ): C++ here I am!" << std::endl; }
};

static bool	handle_error_box(const CEGUI::EventArgs & e);
static void do_error_box(const char * text);

class gui_test
{
public:
	gui_test();

	bool on_clicked(const gui::control_base & widget);

	void handle_soft_wire(const gui::control_base & widget);
	void handle_quit(const gui::control_base & widget);
	void handle_events_window(const gui::control_base & widget);
	void handle_codegen_window(const gui::control_base & widget);
	void handle_editor_window(const gui::control_base & widget);
	void handle_console_window(const gui::control_base & widget);
	void handle_playground_window(const gui::control_base & widget);

	void handle_events_run_lua(const gui::control_base & widget);
	void handle_playground_run_lua(const gui::control_base & widget);

private:
	forward_cout m_forward_cout;

	default_window m_win_menu;
	default_window m_win_console;
	default_window m_win_events;
	default_window m_win_codegen;
	default_window m_win_editor;
	default_window m_win_editable;
	default_window m_win_playground;

	multi_line_edit_box_base * m_events_win_lua_code_widget;
	multi_line_edit_box_base * m_playground_win_lua_code_widget;

	pawn m_pawn;
	boost::shared_ptr<widget_editor> m_editor;
};

} // namespace yake

#endif // _GUI_TEST_H_
