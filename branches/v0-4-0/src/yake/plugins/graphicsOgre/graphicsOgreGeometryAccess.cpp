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
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/plugins/graphicsOgre/yakePCH.h>
#include <yake/graphics/yakeGeometryAccess.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsSystem.h>
#include <yake/plugins/graphicsOgre/yakeGraphicsWorld.h>
#include <yake/plugins/graphicsOgre/graphicsOgreEntity.h>
#include <yake/plugins/graphicsOgre/graphicsOgreGeometryAccess.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {
namespace graphics {
namespace ogre3d {

	//-----------------------------------------------------
	OgreMeshGeometryAccess::OgreMeshGeometryAccess(Ogre::MeshPtr& mesh) : mMesh(mesh), mLastId(0)
	{
	}

	//-----------------------------------------------------
	OgreMeshGeometryAccess::~OgreMeshGeometryAccess()
	{
	}

	//-----------------------------------------------------
	SubmeshId OgreMeshGeometryAccess::createSubmesh()
	{
		YAKE_ASSERT( !mMesh.isNull() ).error("need a mesh!");
		Ogre::SubMesh* pSM = mMesh->createSubMesh();
		YAKE_ASSERT( pSM );
		SubmeshId id = mLastId++;
		mSubmeshes.insert( std::make_pair( id, pSM ) );
		return id;
	}

	//-----------------------------------------------------
	ISubmeshAccess* OgreMeshGeometryAccess::getSubmesh( const SubmeshId id )
	{
		YAKE_ASSERT( !mMesh.isNull() ).error("need a mesh!");
		SubMeshMap::const_iterator itFind = mSubmeshes.find( id );
		if (itFind == mSubmeshes.end())
			return 0;
		return new OgreSubmeshAccess( mMesh, itFind->second );
	}

	//-----------------------------------------------------
	OgreSubmeshAccess::OgreSubmeshAccess( Ogre::MeshPtr& parentMesh, Ogre::SubMesh* subMesh )
			: 	mSubmesh( subMesh ),
				mMesh(parentMesh),
				mpLockedPositions(0),
				mpLockedNormals(0),
				mpLockedColours(0),
				mpLockedIndices(0)
	{
		YAKE_ASSERT( !mMesh.isNull() );
		for (int i=0; i<MAX_TEX_COORDS; ++i)
			mpLockedTexCoords[i] = 0;
	}

	//-----------------------------------------------------
	OgreSubmeshAccess::~OgreSubmeshAccess()
	{
		close();
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::setRenderVertexCount( uint32 count )
	{
		YAKE_ASSERT( mSubmesh );
		YAKE_ASSERT( mSubmesh->vertexData );
		mSubmesh->vertexData->vertexCount = count;
		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::setRenderIndexCount( uint32 count )
	{
		YAKE_ASSERT( mSubmesh );
		YAKE_ASSERT( mSubmesh->indexData );
		mSubmesh->indexData->indexCount = count;
		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::open()
	{
		YAKE_ASSERT( !mMesh.isNull() );
		YAKE_ASSERT( mSubmesh ); // open existing one
		mNumTexCoordSets = 0;
		Ogre::VertexDeclaration::VertexElementList list = mSubmesh->vertexData->vertexDeclaration->getElements();
		for (Ogre::VertexDeclaration::VertexElementList::const_iterator it = list.begin(); it != list.end(); ++it)
		{
			const Ogre::VertexElement & elem = (*it);
			if (elem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES)
				mNumTexCoordSets++;
		}
		return (mSubmesh != 0);
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::open(uint32 numVertices, uint32 numIndices, uint8 numTexSets)
	{
		YAKE_ASSERT( !mMesh.isNull() );
		YAKE_ASSERT( mSubmesh );

		//FIXME:
		YAKE_ASSERT( numVertices < 65536 ).debug("temporary restriction. code is mostly 32bit ready.");

		// setup submesh
		mNumTexCoordSets = numTexSets;

		mSubmesh->useSharedVertices = false;
		mSubmesh->setMaterialName("sphere");
		mSubmesh->operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
		YAKE_ASSERT(mSubmesh->parent);
		mSubmesh->parent->_setBoundingSphereRadius( 200 ); // FIXME
		mBox = mSubmesh->parent->getBounds();

		// index data
		if (numIndices > 0)
		{
			if (!mSubmesh->indexData)
				mSubmesh->indexData = new Ogre::IndexData;
			YAKE_ASSERT( mSubmesh->indexData );
			Ogre::IndexData& indexData = *mSubmesh->indexData;
			indexData.indexStart = 0;
			indexData.indexCount = numIndices; // mode: tri list
			indexData.indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
					/*(indexData.indexCount < 65536)
						? Ogre::HardwareIndexBuffer::IndexType::IT_16BIT 
						: Ogre::HardwareIndexBuffer::IndexType::IT_32BIT,*/
						Ogre::HardwareIndexBuffer::IT_32BIT,
					indexData.indexCount,
					mMesh->getIndexBufferUsage(),
					mMesh->isIndexBufferShadowed()
				);
			YAKE_ASSERT( false == indexData.indexBuffer.isNull() );
		}

		// vertex data
		mSubmesh->vertexData = new Ogre::VertexData;
		YAKE_ASSERT( mSubmesh->vertexData );
		Ogre::VertexData& vertexData = *mSubmesh->vertexData;
		vertexData.vertexStart = 0;
		vertexData.vertexCount = numVertices;

		// vertex decl
		YAKE_ASSERT( vertexData.vertexDeclaration );
		uint16 source = 0;
		vertexData.vertexDeclaration->addElement( source++, 0,
				Ogre::VET_FLOAT3,
				Ogre::VES_POSITION
			);

		vertexData.vertexDeclaration->addElement( source++, 0,
				Ogre::VET_COLOUR,
				Ogre::VES_DIFFUSE
			);

		vertexData.vertexDeclaration->addElement( source++, 0,
				Ogre::VET_FLOAT3,
				Ogre::VES_NORMAL
			);

		for (int i=0; i<mNumTexCoordSets; ++i)
			vertexData.vertexDeclaration->addElement( source++, 0,
					Ogre::VET_FLOAT2,
					Ogre::VES_TEXTURE_COORDINATES,
					i
				);

		// vertex buffers + binding
		source = 0;

		// positions
		mhwvbPositions = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
				vertexData.vertexDeclaration->getVertexSize( source ),
				vertexData.vertexCount,
				mMesh->getVertexBufferUsage(),
				mMesh->isVertexBufferShadowed()
			);
		vertexData.vertexBufferBinding->setBinding(source++, mhwvbPositions );

		// colours
		mhwvbColours = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
				vertexData.vertexDeclaration->getVertexSize( source ),
				vertexData.vertexCount,
				mMesh->getVertexBufferUsage(),
				mMesh->isVertexBufferShadowed()
			);
		vertexData.vertexBufferBinding->setBinding(source++, mhwvbColours );

		// normals
		mhwvbNormals = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
				vertexData.vertexDeclaration->getVertexSize( source ),
				vertexData.vertexCount,
				mMesh->getVertexBufferUsage(),
				mMesh->isVertexBufferShadowed()
				);
		vertexData.vertexBufferBinding->setBinding(source++, mhwvbNormals );

		// tex coord sets
		for (int i=0; i<mNumTexCoordSets; ++i)
		{
			mhwvbTexCoords[i] = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
					vertexData.vertexDeclaration->getVertexSize( source ),
					vertexData.vertexCount,
				mMesh->getVertexBufferUsage(),
				mMesh->isVertexBufferShadowed()
				);
			vertexData.vertexBufferBinding->setBinding(source++, mhwvbTexCoords[i] );
		}

		// we should be done now...

		return (mSubmesh != 0);
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::close()
	{
		YAKE_ASSERT( !mMesh.isNull() );
		YAKE_ASSERT( mSubmesh );

		unlockPositions();
		unlockNormals();
		for (int i=0; i<mNumTexCoordSets; ++i)
			unlockTexCoords( i );
		unlockColours();

		mSubmesh->parent->_setBounds( mBox );

		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::setMaterial( const String & material )
	{
		YAKE_ASSERT( !mMesh.isNull() );
		YAKE_ASSERT( mSubmesh );
		mSubmesh->setMaterialName( Ogre::String(material.c_str()) );
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
		YAKE_ASSERT( !mMesh.isNull() );
		YAKE_ASSERT( mSubmesh );

		YAKE_ASSERT( mSubmesh->vertexData );
		Ogre::VertexData& vertexData = *mSubmesh->vertexData;

		YAKE_ASSERT( vertexData.vertexBufferBinding );

		const Ogre::VertexElement* e = mSubmesh->vertexData->vertexDeclaration->findElementBySemantic( Ogre::VES_POSITION );
		YAKE_ASSERT( e );
		uint16 source = e->getSource();
		mhwvbPositions = vertexData.vertexBufferBinding->getBuffer( source );
		//mhwvbPositions = vertexData.vertexBufferBinding->getBuffer( 0 ); // 0 = positions, 1= colours, 1+set = tex coords
		YAKE_ASSERT( false == mhwvbPositions.isNull() );

		mpLockedPositions = static_cast<real*>(mhwvbPositions->lock( Ogre::HardwareBuffer::HBL_DISCARD ));
		YAKE_ASSERT( mpLockedPositions );

		return (mpLockedPositions!=0);
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::unlockPositions()
	{
		//YAKE_ASSERT( mpLockedPositions != 0 ).warning("should be locked!");
		YAKE_ASSERT( mhwvbPositions.isNull() == false ).warning("need a hwvb!");
		if (mpLockedPositions)
			mhwvbPositions->unlock();
		mpLockedPositions = 0;

		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::setPosition( uint32 index, const Vector3 & position )
	{
		YAKE_ASSERT( mpLockedPositions != 0 ).warning("should be locked!");
		YAKE_ASSERT( mhwvbPositions.isNull() == false ).warning("need a hwvb!");
		if (mhwvbPositions.isNull() || mpLockedPositions == 0)
			return false;
		float* pPos = &mpLockedPositions[ index * 3 ];
		*pPos = position.x; pPos++;
		*pPos = position.y; pPos++;
		*pPos = position.z;

		mBox.merge( Ogre::Vector3(position.x,position.y,position.z) );
		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::lockTexCoords( uint16 set, uint32 start, uint32 count, bool bRead /* = false  */)
	{
		YAKE_ASSERT( mpLockedTexCoords[set] == 0 ).warning("should be unlocked!");
		if (mpLockedTexCoords[set])
		{
			if (!unlockTexCoords(set))
				return false;
		}
		YAKE_ASSERT( !mMesh.isNull() );
		YAKE_ASSERT( mSubmesh );

		YAKE_ASSERT( mSubmesh->vertexData );
		Ogre::VertexData& vertexData = *mSubmesh->vertexData;

		YAKE_ASSERT( vertexData.vertexBufferBinding );
		mhwvbTexCoords[set] = vertexData.vertexBufferBinding->getBuffer( 3+set ); // 0 = positions, 1= colours, 2= normals, 3+set = tex coords
		YAKE_ASSERT( false == mhwvbTexCoords[set].isNull() );

		mpLockedTexCoords[set] = static_cast<real*>(mhwvbTexCoords[set]->lock( Ogre::HardwareBuffer::HBL_DISCARD ));
		YAKE_ASSERT( mpLockedTexCoords[set] );

		return (mpLockedTexCoords[set]!=0);
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::unlockTexCoords(uint16 set)
	{
		//YAKE_ASSERT( mpLockedTexCoords[set] != 0 ).warning("should be locked!");
		//YAKE_ASSERT( mhwvbTexCoords[set].isNull() == false ).warning("need a hwvb!");
		if (mpLockedTexCoords[set])
		{
			if (mhwvbTexCoords[set].get())
				if (mhwvbTexCoords[set]->isLocked())
					mhwvbTexCoords[set]->unlock();
			mpLockedTexCoords[set] = 0;
		}

		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::setTexCoord( uint16 set, uint16 index, const Vector3 & uv )
	{
		YAKE_ASSERT( mpLockedTexCoords[set] != 0 ).warning("should be locked!");
		YAKE_ASSERT( mhwvbTexCoords[set].isNull() == false ).warning("need a hwvb!");
		if (mhwvbTexCoords[set].isNull() || mpLockedTexCoords[set] == 0)
			return false;
		float* pUV = &mpLockedTexCoords[set][ index * 2 ];
		*pUV = uv.x; pUV++;
		*pUV = uv.y;
		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::lockColours( uint32 start, uint32 count, bool bRead /*= false*/ )
	{
		YAKE_ASSERT( mpLockedColours == 0 ).warning("should be unlocked!");
		if (mpLockedColours)
		{
			if (!unlockColours())
				return false;
		}
		YAKE_ASSERT( !mMesh.isNull() );
		YAKE_ASSERT( mSubmesh );

		YAKE_ASSERT( mSubmesh->vertexData );
		Ogre::VertexData& vertexData = *mSubmesh->vertexData;

		YAKE_ASSERT( vertexData.vertexBufferBinding );

		const Ogre::VertexElement* e = mSubmesh->vertexData->vertexDeclaration->findElementBySemantic( Ogre::VES_DIFFUSE );
		YAKE_ASSERT( e );
		uint16 source = e->getSource();
		mhwvbColours = vertexData.vertexBufferBinding->getBuffer( source ); // 1
		YAKE_ASSERT( false == mhwvbColours.isNull() );

		mpLockedColours = static_cast<uint32*>(mhwvbColours->lock( Ogre::HardwareBuffer::HBL_DISCARD ));
		YAKE_ASSERT( mpLockedColours );

		return (mpLockedColours!=0);
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::unlockColours()
	{
		//YAKE_ASSERT( mpLockedColours != 0 ).warning("should be locked!");
		//YAKE_ASSERT( mhwvbColours.isNull() == false ).warning("need a hwvb!");
		if (mhwvbColours.isNull())
			return false;

		if (mpLockedColours)
		{
			mhwvbColours->unlock();
			mhwvbColours.setNull();
			mpLockedColours = 0;
		}

		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::setColour( uint32 index, const Color & colour )
	{
		YAKE_ASSERT( mpLockedColours != 0 ).warning("should be locked!");
		YAKE_ASSERT( mhwvbColours.isNull() == false ).warning("need a hwvb!");
		if (mhwvbColours.isNull() || mpLockedColours == 0)
			return false;
		//RGBA
		mpLockedColours[ index ] =
					((static_cast<uint8>(colour.r * 255.0) & 0xFF) << 24) |
					((static_cast<uint8>(colour.g * 255.0) & 0xFF) << 16) |
					((static_cast<uint8>(colour.b * 255.0) & 0xFF) << 8) |
					(static_cast<uint8>(colour.a * 255.0) & 0xFF);
		//*pColour = colour.asRGBA();
		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::lockIndices( uint32 start, uint32 count )
	{
		YAKE_ASSERT( mpLockedIndices == 0 ).warning("should be unlocked!");
		if (mpLockedIndices)
		{
			if (!unlockIndices())
				return false;
		}
		YAKE_ASSERT( !mMesh.isNull() );
		YAKE_ASSERT( mSubmesh );

		YAKE_ASSERT( mSubmesh->indexData );
		Ogre::IndexData& indexData = *mSubmesh->indexData;

		mhwibIndices = indexData.indexBuffer;
		YAKE_ASSERT( false == mhwibIndices.isNull() );

		mpLockedIndices = static_cast<uint32*>(mhwibIndices->lock( Ogre::HardwareBuffer::HBL_DISCARD ));
		YAKE_ASSERT( mpLockedIndices );

		return (mpLockedIndices!=0);
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::setIndex( uint32 index, uint32 value )
	{
		YAKE_ASSERT( mpLockedIndices != 0 ).warning("should be locked!");
		YAKE_ASSERT( mhwibIndices.isNull() == false ).warning("need a hwib!");
		if (mhwibIndices.isNull() || mpLockedIndices == 0)
			return false;
		mpLockedIndices[ index ] = value;
		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::unlockIndices()
	{
		//YAKE_ASSERT( mpLockedIndices != 0 ).warning("should be locked!");
		//YAKE_ASSERT( mhwibIndices.isNull() == false ).warning("need a hwib!");
		if (mhwibIndices.isNull())
			return false;

		mhwibIndices->unlock();
		mhwibIndices.setNull();
		mpLockedIndices = 0;

		return true;
	}


	//-----------------------------------------------------
	bool OgreSubmeshAccess::lockNormals(uint32 start, uint32 count, bool bRead /* = false  */)
	{
		YAKE_ASSERT( mpLockedNormals == 0 ).warning("should be unlocked!");
		if (mpLockedNormals)
		{
			if (!unlockNormals())
				return false;
		}
		YAKE_ASSERT( !mMesh.isNull() );
		YAKE_ASSERT( mSubmesh );

		YAKE_ASSERT( mSubmesh->vertexData );
		Ogre::VertexData& vertexData = *mSubmesh->vertexData;

		YAKE_ASSERT( vertexData.vertexBufferBinding );

		const Ogre::VertexElement* e = mSubmesh->vertexData->vertexDeclaration->findElementBySemantic( Ogre::VES_NORMAL );
		YAKE_ASSERT( e );
		uint16 source = e->getSource();
		mhwvbNormals = vertexData.vertexBufferBinding->getBuffer( source );
		YAKE_ASSERT( false == mhwvbNormals.isNull() );

		mpLockedNormals = static_cast<real*>(mhwvbNormals->lock( Ogre::HardwareBuffer::HBL_DISCARD ));
		YAKE_ASSERT( mpLockedNormals );

		return (mpLockedNormals!=0);
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::unlockNormals()
	{
		//YAKE_ASSERT( mpLockedNormals != 0 ).warning("should be locked!");
		YAKE_ASSERT( mhwvbNormals.isNull() == false ).warning("need a hwvb!");
		if (mpLockedNormals)
			mhwvbNormals->unlock();
		mpLockedNormals = 0;

		return true;
	}

	//-----------------------------------------------------
	bool OgreSubmeshAccess::setNormal( uint32 index, const Vector3 & normal )
	{
		YAKE_ASSERT( mpLockedNormals != 0 ).warning("should be locked!");
		YAKE_ASSERT( mhwvbNormals.isNull() == false ).warning("need a hwvb!");
		if (mhwvbNormals.isNull() || mpLockedNormals == 0)
			return false;
		float* pN = &mpLockedNormals[ index * 3 ];
		*pN = normal.x; pN++;
		*pN = normal.y; pN++;
		*pN = normal.z;

		return true;
	}
}
}
}
