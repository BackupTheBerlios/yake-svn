#ifndef _YAKE_APPLICATION_H_
#define _YAKE_APPLICATION_H_

namespace yake
{
namespace samples
{

struct library_manager
{
  typedef vector< shared_ptr<base::library> > library_list;

  void load_library(const char * name)
  { ... }

  library_list m_library_list;
};



template <class System>
struct manual_initialization_system_holder
{
   void set_system(shared_ptr<System> system)
   { m_system.reset(system); }

   shared_ptr<System> get_system()
   { return m_system; }

   shared_ptr<System> m_system;
};

template <class Systems>
struct manual_initialization : inherit_linear<Systems, lambda< manual_initialization_system_holder<_> >::type
{
  template <class System>
  System & get_system()
  { static_cast<holder<System>&>(*this)->get_system(); }

  template <class System>
  void load_system(const char * id)
  { get_system<System>().set_system(create<System>(id)); }
};

template <class System>
struct default_initialization_system_holder
{
   default_initialization_system_holder()
     : m_system(create<System>())
   {}

   shared_ptr<System> get_system()
   { return m_system; }

   shared_ptr<System> m_system;
};


// note: you cannot use gui without a renderer (todo: for win32 gui fake renderer needed?)
template <>
struct default_initialization_system_holder<gui_system>
{
   // the user can add an adapter for a gui/renderer combination without changing the code
   default_initialization_system_holder()
     : m_system(create<gui_system>()),
       m_gui_renderer_adapter(create<gui_renderer_adapter>(
         static_cast<default_initialization_system_holder<graphics_system>*>(this)->get_system()->get_type_info(),
         m_system->get_type_info()))
   {}

   shared_ptr<gui_system> get_system()
   { m_system.reset(system); }

   shared_ptr<gui_system> m_system;
   shared_ptr<gui_renderer_adapter> m_gui_renderer_adapter;
};



// todo: config should not is-a library-manager ... hmm?
// config: has two load the libraries within the constructor and needs to define a type list of systems
struct default_config : library_manager
{
  typedef list<graphics_system, gui_system, input_system, physics_system> systems;

  default_config()
  {
    load_library("ogre3d_plugin");   
  }

};




template <class Config = default_config>
struct default_initialization : typename inherit_linear<typename Config::systems, lambda< default_initialization_system_holder<_> >::type
{
  template <class System>
  System & get_system()
  { static_cast<holder<System>&>(*this)->get_system(); }
};



template <class Initialization>
struct application : Initialization
{

};


class my_app : application< default_initialization<> >
{};



struct minimum_config : library_manager
{
  typedef list<graphics_system, input_system> systems;

  minimum_config()
  {
    load_library("ogre3d_plugin");   
  }

};

class my_app_mini : application< default_initialization<minimum_config> >
{};


// todo: useful?
class my_app_manual : library_manager, application< manual_initialization< list<graphics_system, gui_system, input_system, physics_system> > >
{
   my_app_manual()
   {
     load_library("ogre3d_plugin");
     load_system<graphics_system>("yake.graphics.ogre3d");
     load_library("cegui_plugin");
     load_system<graphics_system>("yake.gui.cegui");
     m_gui_renderer_adapter.reset(create<gui_renderer_adapter>(get_system<graphics_system>()->get_type_info(), get_system<gui_system>()->get_type_info()));
   };

   shared_ptr<gui_renderer_adapter> m_gui_renderer_adapter;
};

} // namespace samples
} // namespace yake

#endif // _YAKE_APPLICATION_H_
