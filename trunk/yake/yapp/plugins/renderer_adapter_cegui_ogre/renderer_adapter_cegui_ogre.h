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
#ifndef _YAPP_PLUGINS_RENDERER_ADAPTER_CEGUI_OGRE_RENDERER_ADAPTER_CEGUI_OGRE_H_
#define _YAPP_PLUGINS_RENDERER_ADAPTER_CEGUI_OGRE_RENDERER_ADAPTER_CEGUI_OGRE_H_

// yake
#include <yake/base/yake.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsSystem.h>
// yapp
#include <yapp/gui_adapter/renderer_adapter_base.h>
#include <yapp/plugins/gui_cegui/gui_system.h>

#ifdef YAPP_CONCRETE_GUI_ADAPTER_EXPORTS
#	define YAPP_CONCRETE_GUI_ADAPTER_API DLLEXPORT
#else
#	define YAPP_CONCRETE_GUI_ADAPTER_API DLLIMPORT
#endif

namespace yake
{
namespace gui
{
namespace cegui_ogre
{

struct YAPP_CONCRETE_GUI_ADAPTER_API renderer_adapter_cegui_ogre : renderer_adapter_base
{
	renderer_adapter_cegui_ogre();
	virtual ~renderer_adapter_cegui_ogre();

	CEGUI::OgreRenderer * m_gui_renderer;

	YAKE_DECLARE_CONCRETE(renderer_adapter_cegui_ogre, identifier(typeid(yake::graphics::ogre3d::GraphicsSystem), typeid(yake::gui::cegui::gui_system)))
};

} // namespace cegui_ogre
} // namespace gui
} // namespace yapp

#endif // _YAPP_PLUGINS_RENDERER_ADAPTER_CEGUI_OGRE_RENDERER_ADAPTER_CEGUI_OGRE_H_