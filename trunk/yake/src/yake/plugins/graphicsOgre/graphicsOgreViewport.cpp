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
#include <yake/plugins/graphicsOgre/yakePCH.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCore.h>
#include <yake/plugins/graphicsOgre/graphicsOgreCamera.h>
#include <yake/plugins/graphicsOgre/graphicsOgreViewport.h>

namespace yake {
namespace graphics {
namespace ogre3d {

	//------------------------------------------------------
	OgreViewport::OgreViewport( OgreCore* pCore, 
								OgreCamera* pCam ) : mCore( pCore ), 
													mCamera( pCam ),
													mViewport( 0 )
	{
		YAKE_ASSERT( mCore ).debug("need a core!");
		YAKE_ASSERT( mCamera ).debug("need a camera to render to!");

		Ogre::RenderWindow* pWnd = mCore->getRenderWindow();
		YAKE_ASSERT( pWnd ).debug("render window absent!");

		mViewport = pWnd->addViewport( mCamera->getCamera_(), 100, 0., 0., 1., 1. );
		YAKE_ASSERT( mViewport ).error("viewport creation failed!");

		mViewport->setBackgroundColour( Ogre::ColourValue(0.2,0.2,0.3) );
	}

	//------------------------------------------------------
	OgreViewport::~OgreViewport()
	{
		//destroy viewport
		killViewport();
	}

	//------------------------------------------------------
	void OgreViewport::setDimensions( real left, real top, real width, real height )
	{
		YAKE_ASSERT( mViewport ).debug("need a viewport");
		mViewport->setDimensions( left, top, width, height );
	}

	//------------------------------------------------------
	void OgreViewport::setZ( int z )
	{
		YAKE_ASSERT( mViewport ).debug("need a viewport");

		real left = mViewport->getLeft();
		real top = mViewport->getTop();
		real width = mViewport->getWidth();
		real height = mViewport->getHeight();

		Ogre::RenderWindow* pWnd = mCore->getRenderWindow();

		killViewport();

		try
		{
			mViewport = pWnd->addViewport( mCamera->getCamera_(), z, left, top, width, height );
			mViewport->setBackgroundColour( Ogre::ColourValue(0.2,0.2,0.3) );
		}
		catch ( Ogre::Exception& exc )
		{
			if ( z++ <= 100 )
				setZ( z );
			else
				//FIXME: decide what to do. throw exception? just fail, log error & return?
				throw;
		}
	}

	//------------------------------------------------------
	void OgreViewport::killViewport()
	{
		Ogre::RenderWindow* pWnd = mCore->getRenderWindow();
		pWnd->removeViewport( mViewport->getZOrder() );
		mViewport = NULL;
 	}

	//------------------------------------------------------
	void OgreViewport::attachCamera( ICamera* pCam )
	{
		YAKE_ASSERT(1==0);
	}

	//------------------------------------------------------
	ICamera* OgreViewport::getAttachedCamera() const
	{
		return mCamera;
	}

} // ogre3d
} // graphics
} // yake
