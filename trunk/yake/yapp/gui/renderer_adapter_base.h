#ifndef _YAPP_GUI_RENDERER_ADAPTER_BASE_H_
#define _YAPP_GUI_RENDERER_ADAPTER_BASE_H_

#include <yake/base/type_info.h>

#include "config.h"

namespace yake
{
namespace gui
{

// TODO: is this useful?! we would need to load multiple dlls ... but the application template class or the user
// selects/loads specific plugin e.g. ogre3d/cegui and could load the specific adapter manually ...?!
// => we load "renderer_adapter.dll" which has a bunch of implementations and the system uses the right one,
// so the client cannot use a wrong adapter for a given combination of renderer/gui

// the adapter for a given renderer and gui backend
struct YAPP_GUI_API renderer_adapter_base
{
	virtual ~renderer_adapter_base() {}

	// the identifier is a combination of the type_infos of the renderer (e.g. ogre3d) and the gui (e.g. cegui)
	typedef std::pair<base::type_info, base::type_info> identifier;
	YAKE_DECLARE_REGISTRY_0(renderer_adapter_base, identifier)
};

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_RENDERER_ADAPTER_BASE_H_