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
#ifndef YAKE_GRAPHICALWORLD_H
#define YAKE_GRAPHICALWORLD_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <inc/base/yakePCH.h>
#include <inc/graphics/yakeGraphicsSystem.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base::mpl;

namespace yake 
{
namespace graphics
{

	struct IGraphicalWorld {};

/** A graphics world.
*/
//struct YAKE_GRAPHICS_INTERFACE_API IGraphicalWorld
//{ //: public AbstractFactory< list< ISceneNode, ILight, ICamera > >
//public:
	//virtual ~IGraphicalWorld() {}

	// todo
	//virtual IEntity* createEntity( const base::String & mesh ) = 0;

	/** Creates viewport and assigns camera *pCamera to it.
			Viewport start offset and dimensions are in normalized window-relative coords [0..1].
	*/
	//virtual IViewport* createViewport( ICamera * pCamera ) = 0;

	/** Globally activates the default shadowing method, often stencil shadows.	*/
	//virtual void setShadowsEnabled( bool enabled ) = 0;

	/** Render a single frame. */
	//virtual void render( real timeElapsed ) = 0;
//};

} // graphics
} // yake

#endif // YAKE_GRAPHICALWORLD_H