#include "button_base.h"

namespace yake
{
namespace gui
{

// defaults
float button_base::defaults::size::height = 0.05f;
float button_base::defaults::size::width = 0.2f;
float button_base::defaults::position::x = 0.1f;
float button_base::defaults::position::y = 0.1f;
const char * button_base::defaults::caption = typeid(button_base).name();

GUI_IMPLEMENT_CLASS(button_base)

} // namespace gui
} // namespace yake