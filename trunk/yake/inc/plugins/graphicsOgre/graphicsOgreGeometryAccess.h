/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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
#ifndef YAKE_GRAPHICS_OGRE3D_GEOMETRYACCESS_H
#define YAKE_GRAPHICS_OGRE3D_GEOMETRYACCESS_H

using namespace yake::base::mpl;
using namespace yake::base::templates;
using namespace yake::base::math;

namespace yake {
namespace graphics {
namespace ogre3d {

	const uint8 MAX_TEX_COORDS = 8;

	class OgreSubmeshAccess : public ISubmeshAccess
	{
	private:
		Ogre::SubMesh*		mSubmesh;
		Ogre::Mesh*			mMesh;
		uint8				mNumTexCoordSets;

		real*				mpLockedPositions;
		real*				mpLockedTexCoords[MAX_TEX_COORDS];
		Ogre::HardwareVertexBufferSharedPtr	mhwvbPositions;
	public:
		OgreSubmeshAccess( Ogre::Mesh* parentMesh, Ogre::SubMesh* subMesh = 0 );
		virtual ~OgreSubmeshAccess();

		virtual bool open();
		virtual bool open( uint32 numVertices, uint8 numTexSets );
		virtual bool close();

		virtual bool lockPositions( uint32 start, uint32 count, bool bRead = false );
		virtual bool unlockPositions();
		virtual bool setPosition( uint32 index, const Vector3 & position );

		virtual bool lockTexCoords( uint16 set, uint32 start, uint32 count, bool bRead = false );
		virtual bool unlockTexCoords( uint16 set );
		virtual bool setTexCoord( uint16 set, uint16 index, const Vector3 & uv );

		virtual bool lockColours( uint32 start, uint32 count, bool bRead = false );
		virtual bool unlockColours();
		virtual bool setColour( uint32 index, const Color & colour );
	};

	class OgreMeshGeometryAccess : public IMeshGeometryAccess
	{
	public:
		OgreMeshGeometryAccess();
		virtual ~OgreMeshGeometryAccess();

		virtual SubmeshId createSubmesh();
		virtual ISubmeshAccess* getSubmesh( const SubmeshId id );
	};

} // ogre3d
} // graphics
} // yake

#endif