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
#ifndef YAKE_GRAPHICSSYSTEM_H
#define YAKE_GRAPHICSSYSTEM_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/graphics/yakeGraphics.h>
#include <yake/graphics/yakeGraphicalWorld.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base::mpl;
using namespace yake::base::templates;

namespace yake {
namespace graphics {

/** The graphics system interface.
*/
struct YAKE_GRAPHICS_INTERFACE_API IGraphicsSystem 
	//: public AbstractFactory< sequences::list< IGraphicalWorld > >
{
	// Destructor.
	virtual ~IGraphicsSystem();

	virtual SharedPtr<IGraphicalWorld> createWorld() = 0;

YAKE_DECLARE_REGISTRY_0( IGraphicsSystem, yake::base::String )
};

} // graphics
} // yake

#endif // YAKE_GRAPHICSSYSTEM_H

