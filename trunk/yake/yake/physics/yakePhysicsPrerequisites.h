/*
   ------------------------------------------------------------------------------------
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
#ifndef YAKE_PHYSICS_PREQREQUISITES_H
#define YAKE_PHYSICS_PREQREQUISITES_H

#ifndef YAKE_BASE_PREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif

#if defined( YAKE_PHYSICS_EXPORTS )
#	define YAKE_PHYSICS_API DLLEXPORT
#else
#	define YAKE_PHYSICS_API DLLIMPORT
#endif

#include <boost/any.hpp>

#define YAKE_PHYSICS_COMMON_POINTERS( CLASS ) \
	typedef CLASS* CLASS##Ptr; \
	typedef Deque<CLASS##Ptr> CLASS##PtrList;

namespace yake {
	/** @todo move out */
	template<class ListenerType>
		class ListenerManager
		{
		private:
			typedef Deque<ListenerType*> ListenerPtrList;
			ListenerPtrList	mListeners;
		public:
			virtual ~ListenerManager()
			{
				ConstDequeIterator<ListenerPtrList> it(mListeners);
				while (it.hasMoreElements())
					delete it.getNext();
			}

			void pushListener(ListenerType* pListener)
			{
				if (pListener)
					mListeners.push_back( pListener );
			}
			void removeListener(ListenerType* pListener)
			{
				typename ListenerPtrList::iterator itFind = std::find( mListeners.begin(), mListeners.end(), pListener );
				if (itFind != mListeners.end())
					mListeners.erase( itFind );
			}
		};

	using namespace base;
	using namespace math;
namespace physics {
	typedef ::yake::uint32 TriangleMeshId;
	const TriangleMeshId kTriangleMeshIdNone = 0xFFFFFFFF;

	/* not used (yet?)
	enum PhysicsErrorCode
	{
		kPEC_NONE = 10000,
		kPEC_ERROR = 10001,
		kPEC_INVALID_AXIS = 10002,
		kPEC_INVALID_PARAMETER = 10003,
		kPEC_ALREADY_EXISTS = 10004
	};
	*/

	struct Property
	{
		Property();
		Property( const String& n, const boost::any& v );

		String		name;
		boost::any	value;
	};
	typedef Deque<String> StringVector;

	class YAKE_PHYSICS_API IPropertyQueryHandler
	{
	public:
		virtual ~IPropertyQueryHandler() {}
		virtual StringVector getPropertyNames() const = 0;
		virtual void setProperty( const String& rName, const boost::any& rValue ) = 0;
	};

	struct TriangleMeshDesc
	{
		/** Important Notice: Vector3Vector, VertexVector, NormalVector and IndexVector HAVE to
			be vectors because they may be accessed like C arrays ( e.g. &arr[0] ).
			Is this OK? Today, it's practically OK and tomorrow it'll be even more as it's part
			of the first Technical Corrigendum.
		*/
		typedef Vector<Vector3> Vector3Vector;
		typedef Vector3Vector VertexVector;
		typedef Vector3Vector NormalVector;
		typedef uint32 IndexType;
		typedef Vector<IndexType> IndexVector;

		TriangleMeshDesc()
		{
		}
		TriangleMeshDesc( const VertexVector& rVertices, const IndexVector& rIndices ) :
				vertices( rVertices ),
				indices( rIndices )
		{
		}
		
		TriangleMeshDesc(	const VertexVector& rVertices,
							const IndexVector& rIndices,
							const NormalVector& rNormals ) :
				vertices( rVertices ),
				indices( rIndices ),
				normals( rNormals )
		{
		}
		
		VertexVector	vertices;
		IndexVector		indices;
		NormalVector	normals;
	};
}
}

#endif

