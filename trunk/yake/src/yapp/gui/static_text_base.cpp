#include <yapp/gui/static_text_base.h>

namespace yake
{
namespace gui
{

// defaults
float static_text_base::defaults::size::height = 0.05f;
float static_text_base::defaults::size::width = 0.2f;
float static_text_base::defaults::position::x = 0.1f;
float static_text_base::defaults::position::y = 0.1f;
const char * static_text_base::defaults::text = typeid(static_text_base).name();

GUI_IMPLEMENT_CLASS(static_text_base)

} // namespace gui
} // namespace yake