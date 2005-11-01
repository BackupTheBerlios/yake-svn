#include "multi_line_edit_box_base.h"
#include <typeinfo>

namespace yake
{
namespace gui
{

// defaults
float multi_line_edit_box_base::defaults::size::height = 0.05f;
float multi_line_edit_box_base::defaults::size::width = 0.2f;
float multi_line_edit_box_base::defaults::position::x = 0.1f;
float multi_line_edit_box_base::defaults::position::y = 0.1f;
const char * multi_line_edit_box_base::defaults::text = typeid(multi_line_edit_box_base).name();

GUI_IMPLEMENT_CLASS(multi_line_edit_box_base)

} // namespace gui
} // namespace yake