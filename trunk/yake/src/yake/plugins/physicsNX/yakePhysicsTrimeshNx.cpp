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
#include <yake/plugins/physicsNX/yakePCH.h>
#include <yake/plugins/physicsNX/yakePhysicsTrimeshNx.h>

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace physics {

	TrimeshNxManager::TriRefMap TrimeshNxManager::msTriRefs;
	uint32 TrimeshNxManager::msLastId = 0;

		static NxTriangleMesh* getById(const TriangleMeshId id);
		static NxTriangleMesh* createMesh(const NxTriangleMeshDesc & rkNxDesc, TriangleMeshId & retId);
		static void releaseMesh(const TriangleMeshId id);
	NxTriangleMesh* TrimeshNxManager::createMesh(const TriangleMeshDesc & rkDesc, TriangleMeshId & retId)
	{
		//@todo optimize transfer

		const size_t numVerts = rkDesc.vertices.size();
		YAKE_ASSERT( numVerts >= 3 );

		const size_t numIndices = rkDesc.indices.size();
		YAKE_ASSERT( numIndices >= 3 );

		size_t idx = 0;

		// transfer vertices
		NxVec3* pVerts = new NxVec3[ numVerts ];
		ConstVectorIterator<TriangleMeshDesc::VertexVector> itV( rkDesc.vertices.begin(), rkDesc.vertices.end() );
		while (itV.hasMoreElements())
		{
			const Vector3 & v = itV.getNext();
			pVerts[idx].x = toNx(v.x);
			pVerts[idx].y = toNx(v.y);
			pVerts[idx].z = toNx(v.z);
			++idx;
		}
		YAKE_ASSERT( idx == numVerts );

		// transfer indices
		idx = 0;
		NxU32* pIndices = new NxU32[ numIndices*3 ];
		ConstVectorIterator<TriangleMeshDesc::IndexVector> itI( rkDesc.indices.begin(), rkDesc.indices.end() );
		while (itI.hasMoreElements())
		{
			pIndices[idx++] = itI.getNext();
		}
		YAKE_ASSERT( idx == numIndices );

		// create NX mesh desc and mesh
		NxTriangleMeshDesc meshDesc;
		meshDesc.numVertices				= NxU32(numVerts)	;
		meshDesc.numTriangles				= NxU32(numIndices / 3);
		meshDesc.pointStrideBytes			= sizeof(NxVec3);
		meshDesc.triangleStrideBytes		= 3*sizeof(NxU32);
		meshDesc.points						= pVerts;
		meshDesc.triangles					= pIndices;
		meshDesc.flags						= 0;

		meshDesc.heightFieldVerticalAxis	= NX_NOT_HEIGHTFIELD; // NX_NOT_HEIGHTFIELD or NX_Y or...
		//meshDesc.heightFieldVerticalExtent  = 0;

		//
		NxTriangleMesh* pMesh = getNxSDK()->createTriangleMesh( meshDesc );
		YAKE_ASSERT( pMesh );
		if(!pMesh)
			return 0;

		retId = ++msLastId;
		msTriRefs.insert( std::make_pair( retId, MeshCountPair(pMesh, 1) ) );

		std::cout << "Y!NX:MESH_SHAPE created " << retId << std::endl;

		return pMesh;
	}
	NxTriangleMesh* TrimeshNxManager::getById(const TriangleMeshId id)
	{
		YAKE_ASSERT( id != kTriangleMeshIdNone );
		if (id == kTriangleMeshIdNone)
			return 0;

		TriRefMap::iterator itFind = msTriRefs.find( id );
		YAKE_ASSERT( itFind != msTriRefs.end() ).debug("Trying to release a non-managed mesh!");
		if (itFind == msTriRefs.end())
			return 0;

		return itFind->second.first;
	}
	void TrimeshNxManager::releaseMesh(const TriangleMeshId id)
	{
		std::cout << "Y!NX:MESH_SHAPE releasing " << id << std::endl;

		YAKE_ASSERT( id != kTriangleMeshIdNone );
		if (id == kTriangleMeshIdNone)
			return;

		TriRefMap::iterator itFind = msTriRefs.find( id );
		YAKE_ASSERT( itFind != msTriRefs.end() ).debug("Trying to release a non-managed mesh!");
		if (itFind == msTriRefs.end())
			return;

		MeshCountPair& meshCountPair = itFind->second;
		
		--meshCountPair.second; // decrease ref count
		if (meshCountPair.second == 0) // ref count == 0 ? destroy!
		{
			YAKE_ASSERT( meshCountPair.first );
			if (meshCountPair.first)
			{
				NxTriangleMesh* pNxMesh = meshCountPair.first;
				{ //@todo dereferencable in stlport fails!
					YAKE_ASSERT( 0 ).warning("NX: TRIMESH WAS NOT FREED! LEAKAGE!");
					try {
						getNxSDK()->releaseTriangleMesh( *pNxMesh );
					} catch (...) {
						YAKE_LOG("NX: Catched exception from NX when releasing triangle mesh.\n");
					}
				}
			}
			msTriRefs.erase( itFind );
		}
	}
	void TrimeshNxManager::destroyAll()
	{
		YAKE_ASSERT( msTriRefs.size() == 0 ).warning("Not all meshes had been released! Releasing the rest now.");
		VectorIterator< TriRefMap > it(msTriRefs.begin(), msTriRefs.end());
		while (it.hasMoreElements())
		{
			std::pair<TriangleMeshId, MeshCountPair> & p = it.getNext();
			YAKE_ASSERT( p.second.first );
			if (p.second.first)
			{
				try {
					getNxSDK()->releaseTriangleMesh( *p.second.first );
				} catch (...) {
					YAKE_LOG("NX: Catched exception from NX when releasing triangle mesh.\n");
				}
			}
			p.second.second = 0;
		}
		msTriRefs.clear();
	}

}
}