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
#include <yake/graphics/yakeGraphics.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace graphics {

//using namespace yake::base::templates;
using namespace yake::base::math;

/*
	class IRenderOperation
	{
	public:
		public ~IRenderOperation() {}

		virtual bool open( uint32 numVertices, uint32 numIndices, uint8 numTexSets ) = 0;
		//virtual bool resize( uint32 numVertices, uint32 numIndices, uint8 numTexSets ) = 0;
		virtual bool close() = 0;

		virtual void setMaterial( const String & material ) = 0;

		virtual bool render() = 0;

		virtual uint32 getNumVertices() const = 0;
		virtual uint32 getNumIndices() const = 0;
		virtual uint8 getNumTexSets() const = 0;

		virtual bool lockPositions( uint32 start, uint32 count, bool bRead = false ) = 0;
		virtual bool unlockPositions() = 0;
		virtual bool setPosition( uint32 index, const Vector3 & position ) = 0;

		virtual bool lockTexCoords( uint16 set, uint32 start, uint32 count, bool bRead = false ) = 0;
		virtual bool unlockTexCoords( uint16 set ) = 0;
		virtual bool setTexCoord( uint16 set, uint16 index, const Vector3 & uv ) = 0;

		virtual bool lockColours( uint32 start, uint32 count, bool bRead = false ) = 0;
		virtual bool unlockColours() = 0;
		virtual bool setColour( uint32 index, const Color & colour ) = 0;

		virtual bool lockIndices( uint32 start, uint32 count ) = 0;
		virtual bool setIndex( uint32 index, uint32 value ) = 0;
		virtual bool unlockIndices() = 0;
	};
*/
	typedef uint32 SubmeshId;
	class ISubmeshAccess
	{
	public:
		virtual ~ISubmeshAccess() {}

		virtual bool open() = 0;
		virtual bool open( uint32 numVertices, uint32 numIndices, uint8 numTexSets ) = 0;
		virtual bool close() = 0;

		virtual bool setRenderVertexCount( uint32 count ) = 0;
		virtual bool setRenderIndexCount( uint32 count ) = 0;

		virtual bool setMaterial( const base::String & material ) = 0;

		virtual bool lockPositions( uint32 start, uint32 count, bool bRead = false ) = 0;
		virtual bool unlockPositions() = 0;
		virtual bool setPosition( uint32 index, const Vector3 & position ) = 0;

		virtual bool lockNormals( uint32 start, uint32 count, bool bRead = false ) = 0;
		virtual bool unlockNormals() = 0;
		virtual bool setNormal( uint32 index, const Vector3 & normal ) = 0;

		virtual bool lockTexCoords( uint16 set, uint32 start, uint32 count, bool bRead = false ) = 0;
		virtual bool unlockTexCoords( uint16 set ) = 0;
		virtual bool setTexCoord( uint16 set, uint16 index, const Vector3 & uv ) = 0;

		virtual bool lockColours( uint32 start, uint32 count, bool bRead = false ) = 0;
		virtual bool unlockColours() = 0;
		virtual bool setColour( uint32 index, const Color & colour ) = 0;

		virtual bool lockIndices( uint32 start, uint32 count ) = 0;
		virtual bool setIndex( uint32 index, uint32 value ) = 0;
		virtual bool unlockIndices() = 0;
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

