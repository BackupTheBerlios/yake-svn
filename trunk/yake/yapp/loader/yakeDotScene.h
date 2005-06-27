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
#ifndef YAKE_DATA_DOTSCENE_H
#define YAKE_DATA_DOTSCENE_H

#include <yake/data/yakeData.h>

#pragma warning(disable: 4244)

namespace yake {
namespace data {
namespace serializer {
namespace dotscene {

	typedef ::yake::Vector< graphics::ISceneNode* > SceneNodeList;

	/** DotSceneReader base class providing default implementation for DOM parsing of dotScene files.
	* \todo read environment, external references ..
	*/
	class DotSceneSerializer
	{
	public:
		virtual ~DotSceneSerializer() {}

		String getName() const
		{ return "yake.data.dotScene"; }

		virtual Version getVersion() const = 0;

		/** Build graphics scene from DOM!
			\param file 
			\return 
		*/
		virtual bool load(	const SharedPtr<dom::INode> & docNode,
							graphics::IWorld* pGWorld);

		SceneNodeList getRootLevelSceneNodes() const;
		String getNameForSceneNode( graphics::ISceneNode* pSceneNode ) const
		{
			SceneNodeNameMap::const_iterator itFind = mNodeNames.find( pSceneNode );
			if (itFind == mNodeNames.end())
				return String("");
			return itFind->second;
		}

		/** Prepare for next run of load/store.
		*/
		virtual void reset();
	protected:
		/// Default implementations for common functions.

		//virtual void readWorld( const SharedPtr<dom::INode>& pNode, graphics::ISceneNode* pParen );
		virtual void readScene( const SharedPtr<dom::INode>& pNode, graphics::ISceneNode* pParentSN);
		
		virtual void readNodes( const SharedPtr<dom::INode> & pNodes, graphics::ISceneNode* pParentSN );
		virtual void readNode( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN );
		virtual void readEntity( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN );
		virtual void readRotation( const SharedPtr<dom::INode> & pNode, Quaternion & rotation );
		virtual void readPosition( const SharedPtr<dom::INode> & pNode, Vector3 & position );
		virtual void readScale( const SharedPtr<dom::INode> & pNode, Vector3& rScale );
		virtual void readVector( const SharedPtr<dom::INode> & pNode, Vector3& rVec );
		
		/// Lights (! :P )
		virtual void readLight( const SharedPtr<dom::INode>& pNode, graphics::ISceneNode* pParentSN );
		virtual void readColour( const SharedPtr<dom::INode>& pNode, Color& colour );
		virtual void readLightRange( const SharedPtr<dom::INode>& pNode, graphics::ILight* pLight );
		virtual void readLightAttenuation( const SharedPtr<dom::INode>& pNode, graphics::ILight* pLight );

	private:
		SharedPtr<dom::INode>					mDocNode;
		graphics::IWorld*				mGWorld;
		SceneNodeList							mRootNodes;

		typedef AssocVector<graphics::ISceneNode*, String> SceneNodeNameMap;
		SceneNodeNameMap						mNodeNames;
	};

	class DotSceneSerializerV1 : public DotSceneSerializer
	{
	public:
		virtual Version getVersion() const
		{ return Version(0,1,0); }
	};


} // dotscene
} // serializer
} // data
} // yake

#endif
