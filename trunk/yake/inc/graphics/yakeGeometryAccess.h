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
#ifndef YAKE_GRAPHICS_GEOMETRYACCESS_H
#define YAKE_GRAPHICS_GEOMETRYACCESS_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <inc/graphics/yakeGraphics.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
using namespace yake::base::mpl;
using namespace yake::base::templates;
using namespace yake::base::math;

namespace yake {
namespace graphics {

	typedef uint32 SubmeshId;
	class ISubmeshAccess
	{
	public:
		virtual ~ISubmeshAccess() {}

		virtual bool open() = 0;
		virtual bool open( uint32 numVertices, uint8 numTexSets ) = 0;
		virtual bool close() = 0;

		virtual bool lockPositions( uint32 start, uint32 count, bool bRead = false ) = 0;
		virtual bool unlockPositions() = 0;
		virtual bool setPosition( uint32 index, const Vector3 & position ) = 0;

		virtual bool lockTexCoords( uint16 set, uint32 start, uint32 count, bool bRead = false ) = 0;
		virtual bool unlockTexCoords( uint16 set );
		virtual bool setTexCoord( uint16 set, uint16 index, const Vector3 & uv ) = 0;

		virtual bool lockColours( uint32 start, uint32 count, bool bRead = false ) = 0;
		virtual bool unlockColours() = 0;
		virtual bool setColour( uint32 index, const Color & colour ) = 0;
	};

	class IMeshGeometryAccess
	{
	public:
		virtual ~IMeshGeometryAccess() {}

		virtual SubmeshId createSubmesh() = 0;
		virtual ISubmeshAccess* getSubmesh( const SubmeshId id ) = 0;
	};

} // graphics
} // yake

#endif // YAKE_GRAPHICS_GEOMETRYACCESS_H

