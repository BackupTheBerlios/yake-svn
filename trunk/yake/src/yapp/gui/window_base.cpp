#include "window_base.h"

namespace yake
{
namespace gui
{

window_base::window_list window_base::windows = window_base::window_list(); 

const window_base::window_list & get_windows()
{ return window_base::windows; }

// defaults
float window_base::defaults::size::height = 0.5f;
float window_base::defaults::size::width = 0.5f;
float window_base::defaults::position::x = 0.1f;
float window_base::defaults::position::y = 0.1f;
const char * window_base::defaults::title = typeid(window_base).name();

} // namespace gui
} // namespace yake