/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#include <yapp/plugins/ceguiOgreRendererAdapter/pch.h>
#include "OgreNoMemoryMacros.h"
#include "CEGUI.h"
#include <Ogre.h>
#include "OgreCEGUIRenderer.h"
#include <yapp/common/yakeCEGUIRendererAdapter.h>
#include <yapp/plugins/ceguiOgreRendererAdapter/plugin.h>


//---------------------------------------------------------
// This is an example of an exported function.
extern "C"
{
	yake::base::Plugin* dynlibStartPlugin(void)
	{
		return new ThePlugin();
	}
}

//---------------------------------------------------------
ThePlugin::ThePlugin()
{
}

//---------------------------------------------------------
ThePlugin::~ThePlugin()
{
}

//---------------------------------------------------------
yake::String ThePlugin::getName() const
{
	return yake::String("yake.misc.ceguiadapter");
}

//---------------------------------------------------------
yake::Version ThePlugin::getVersion() const
{
	return yake::Version( 0, 1, 0 );
}

//---------------------------------------------------------
bool ThePlugin::initialise()
{
	try {
		//HACK FIXME getAutoCreatedWindow assumes window was autocreated!
		Ogre::RenderWindow* pWin = Ogre::Root::getSingleton().getAutoCreatedWindow();
		YAKE_ASSERT( pWin ).error("Need a valid render window!");
		mGUIRenderer = new CEGUI::OgreCEGUIRenderer(
			pWin, 
			Ogre::RENDER_QUEUE_OVERLAY, 
			true, 
			3000,
			Ogre::ST_GENERIC );
			//Ogre::Root::getSingleton()._getCurrentSceneManager());
	} catch (Ogre::Exception& e)
	{
		YAKE_EXCEPT("Failed to initialise CEGUI adapter!\n" + yake::String(e.getFullDescription().c_str()), "initialise");
	}
	YAKE_ASSERT( mGUIRenderer );
	return (mGUIRenderer != 0);
}

//---------------------------------------------------------
bool ThePlugin::shutdown()
{
	YAKE_SAFE_DELETE(mGUIRenderer);
	return true;
}

//---------------------------------------------------------
CEGUI::Renderer* ThePlugin::getRenderer()
{
	return mGUIRenderer;
}
