#ifndef _YAPP_GUI_RENDERER_ADAPTER_BASE_H_
#define _YAPP_GUI_RENDERER_ADAPTER_BASE_H_

#include "config.h"

namespace yake
{
namespace gui
{

struct YAPP_GUI_API renderer_adapter_base
{
	virtual renderer_adapter_base() {}

	// the identifier is a combination of the type_infos of the renderer (e.g. ogre3d) and the gui (e.g. cegui)
	YAKE_DECLARE_REGISTRY_0(renderer_adapter_base, std::pair<const std::type_info &, const std::type_info &>)
};

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_RENDERER_ADAPTER_BASE_H_