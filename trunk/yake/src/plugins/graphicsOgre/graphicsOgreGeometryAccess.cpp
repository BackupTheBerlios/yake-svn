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
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <inc/plugins/graphicsOgre/yakePCH.h>
#include <inc/graphics/yakeGeometryAccess.h>
#include <inc/plugins/graphicsOgre/yakeGraphicsSystem.h>
#include <inc/plugins/graphicsOgre/yakeGraphicsWorld.h>
#include <inc/plugins/graphicsOgre/graphicsOgreEntity.h>
#include <inc/plugins/graphicsOgre/graphicsOgreGeometryAccess.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace graphics {
namespace ogre3d {

	//-----------------------------------------------------
	OgreSubmeshAccess::OgreSubmeshAccess( Ogre::Mesh* parentMesh, Ogre::SubMesh* subMesh )
			: mMesh(parentMesh), mSubmesh( subMesh ),
				mpLockedPositions(0)
	{
		YAKE_ASSERT( mMesh );
		for (int i=0; i<MAX_TEX_COORDS; ++i)
			mpLockedTexCoords[i] = 0;
	}

	//-----------------------------------------------------
	OgreSubmeshAccess::~OgreSubmeshAccess()
	{
		close();
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::open()
	{
		YAKE_ASSERT( mMesh );
		YAKE_ASSERT( mSubmesh ); // open existing one
		return (mSubmesh != 0);
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::open(uint32 numVertices, uint8 numTexSets)
	{
		YAKE_ASSERT( mMesh );
		YAKE_ASSERT( mSubmesh == 0 ); // create a new one!

		// setup submesh
		mSubmesh = mMesh->createSubMesh();
		YAKE_ASSERT( mSubmesh ).debug("graphics.ogre: Could not create mesh.");

		mNumTexCoordSets = numTexSets;

		mSubmesh->vertexData = new Ogre::VertexData;
		mSubmesh->indexData = new Ogre::IndexData;

		mSubmesh->useSharedVertices = false;
		mSubmesh->setMaterialName("test");

		// index data
		YAKE_ASSERT( mSubmesh->indexData );
		Ogre::IndexData& indexData = *mSubmesh->indexData;
		indexData.indexStart = 0;
		indexData.indexCount = numVertices; // mode: tri list
		indexData.indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
				(indexData.indexCount < 65536)
					? Ogre::HardwareIndexBuffer::IndexType::IT_16BIT 
					: Ogre::HardwareIndexBuffer::IndexType::IT_32BIT,
				indexData.indexCount,
				Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,
				false // shadow buffer
			);
		YAKE_ASSERT( false == indexData.indexBuffer.isNull() );

		// vertex data
		YAKE_ASSERT( mSubmesh->vertexData );
		Ogre::VertexData& vertexData = *mSubmesh->vertexData;
		vertexData.vertexStart = 0;
		vertexData.vertexCount = numVertices;

		// vertex decl
		vertexData.vertexDeclaration = new Ogre::VertexDeclaration();
		YAKE_ASSERT( vertexData.vertexDeclaration );
		uint16 source = 0;
		vertexData.vertexDeclaration->addElement( source++, 0 /*offset*/,
				Ogre::VET_FLOAT3,
				Ogre::VES_POSITION
			);
		vertexData.vertexDeclaration->addElement( source++, 0 /*offset*/,
				Ogre::VET_COLOUR,
				Ogre::VES_DIFFUSE
			);
		for (int i=0; i<mNumTexCoordSets; ++i)
			vertexData.vertexDeclaration->addElement( source++, 0 /*offset*/,
					Ogre::VET_FLOAT2,
					Ogre::VES_TEXTURE_COORDINATES,
					i
				);

		// vertex buffers + binding
		source = 0;

		// positions
		Ogre::HardwareVertexBufferSharedPtr hwvbVertices = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
				vertexData.vertexDeclaration->getVertexSize( source ),
				vertexData.vertexCount,
				Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,
				false // shadow buffer
			);
		vertexData.vertexBufferBinding->setBinding(source++, hwvbVertices );

		// colours
		Ogre::HardwareVertexBufferSharedPtr hwvbColours = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
				vertexData.vertexDeclaration->getVertexSize( source ),
				vertexData.vertexCount,
				Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,
				false // shadow buffer
			);
		vertexData.vertexBufferBinding->setBinding(source++, hwvbColours );

		// tex coord sets
		for (int i=0; i<mNumTexCoordSets; ++i)
		{
			Ogre::HardwareVertexBufferSharedPtr hwvbTexCoordSet = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
					vertexData.vertexDeclaration->getVertexSize( source ),
					vertexData.vertexCount,
					Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,
					false // shadow buffer
				);
			vertexData.vertexBufferBinding->setBinding(source++, hwvbTexCoordSet );
		}

		// we should be done now...

		return (mSubmesh != 0);
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::close()
	{
		YAKE_ASSERT( mMesh );
		YAKE_ASSERT( mSubmesh );

		unlockPositions();
		for (int i=0; i<mNumTexCoordSets; ++i)
			unlockTexCoords( i );
		unlockColours();

		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::lockPositions(uint32 start, uint32 count, bool bRead /* = false  */)
	{
		YAKE_ASSERT( mpLockedPositions == 0 ).warning("should be unlocked!");
		if (mpLockedPositions)
		{
			if (!unlockPositions())
				return false;
		}
		YAKE_ASSERT( mMesh );
		YAKE_ASSERT( mSubmesh );

		YAKE_ASSERT( mSubmesh->vertexData );
		Ogre::VertexData& vertexData = *mSubmesh->vertexData;

		YAKE_ASSERT( vertexData.vertexBufferBinding );
		mhwvbPositions = vertexData.vertexBufferBinding->getBuffer( 0 ); // 0 = positions
		YAKE_ASSERT( false == mhwvbPositions.isNull() );

		mpLockedPositions = static_cast<real*>(mhwvbPositions->lock( Ogre::HardwareBuffer::HBL_DISCARD ));
		YAKE_ASSERT( mpLockedPositions );

		return (mpLockedPositions!=0);
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::unlockPositions()
	{
		YAKE_ASSERT( mpLockedPositions != 0 ).warning("should be locked!");
		YAKE_ASSERT( mhwvbPositions.isNull() == false ).warning("need a hwvb!");
		if (mhwvbPositions.isNull())
			return false;

		mhwvbPositions->unlock();

		return true;
	}

}
}
}