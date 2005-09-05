/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#ifndef _YAPP_GUI_RENDERER_ADAPTER_BASE_H_
#define _YAPP_GUI_RENDERER_ADAPTER_BASE_H_

// yake
#include <yake/base/yake.h>
// yapp
#include <yapp/gui/config.h>

#if defined(YAPP_GUI_RENDERER_ADAPTER_BASE_EXPORTS)
#	define YAPP_GUI_ADAPTER_API DLLEXPORT
#else
#	define YAPP_GUI_ADAPTER_API DLLIMPORT
#endif

namespace yake
{
namespace gui
{

// TODO: is this useful?! we would need to load multiple dlls ... but the application template class or the user
// selects/loads specific plugin e.g. ogre3d/cegui and could load the specific adapter manually ...?!
// => we load "renderer_adapter.dll" which has a bunch of implementations and the system uses the right one,
// so the client cannot use a wrong adapter for a given combination of renderer/gui

// the adapter for a given renderer and gui backend
struct YAPP_GUI_ADAPTER_API renderer_adapter_base
{
	virtual ~renderer_adapter_base() {}

	// the identifier is a combination of the type_infos of the renderer (e.g. ogre3d) and the gui (e.g. cegui)
	typedef std::pair<base::type_info, base::type_info> identifier;
	YAKE_DECLARE_REGISTRY_0(renderer_adapter_base, identifier)
};

} // namespace gui
} // namespace yake

#endif // _YAPP_GUI_RENDERER_ADAPTER_BASE_H_