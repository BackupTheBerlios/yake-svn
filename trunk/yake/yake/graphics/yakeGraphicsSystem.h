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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
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
	//: public AbstractFactory< sequences::list< IWorld > >
{
	YAKE_DECLARE_REGISTRY_0( IGraphicsSystem, yake::base::String )
	
	// Destructor.
	virtual ~IGraphicsSystem();

	virtual SharedPtr<IWorld> createWorld() = 0;

	typedef Signal0<void> ShutdownSignal;
	virtual void subscribeToShutdownSignal( const ShutdownSignal::slot_type& rSlot ) = 0;

	virtual const std::type_info & get_type_info() = 0;
};

} // graphics
} // yake

#endif // YAKE_GRAPHICSSYSTEM_H

