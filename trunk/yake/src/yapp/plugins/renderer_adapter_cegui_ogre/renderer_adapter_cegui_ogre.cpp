/**------------------------------------------------------------------------------------
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
*/
// ogre
#include <dependencies/ogrenew/OgreMain/include/OgreNoMemoryMacros.h>
#include <dependencies/ogrenew/OgreMain/include/Ogre.h>
// cegui
#include <dependencies/cegui_mk2/include/CEGUI.h>
#include <dependencies/cegui_mk2/include/renderers/OgreGUIRenderer/ogrerenderer.h>
// yake
#include <yapp/gui/renderer_adapter_base.h>

namespace yapp
{
namespace gui
{
namespace cegui_ogre
{

//---------------------------------------------------------
renderer_adapter_cegui_ogre::renderer_adapter_cegui_ogre()
{
	Ogre::RenderWindow * render_window = Ogre::Root::getSingleton().getAutoCreatedWindow();
	YAKE_ASSERT(render_window).error("Need a valid render window!");
	m_gui_renderer = new CEGUI::OgreCEGUIRenderer(
		render_window, 
		Ogre::RENDER_QUEUE_OVERLAY, 
		true, 
		3000,
		Ogre::ST_GENERIC);
	YAKE_ASSERT(m_gui_renderer).error("Need a valid renderer!");
}

//---------------------------------------------------------
renderer_adapter_cegui_ogre::~renderer_adapter_cegui_ogre()
{
	safe_delete(m_gui_renderer);
}

} // namespace cegui_ogre
} // namespace gui
} // namespace yapp