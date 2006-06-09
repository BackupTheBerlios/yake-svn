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
#ifndef YAKE_DATA_DOTSCENE_H
#define YAKE_DATA_DOTSCENE_H

#include <yake/loader/prerequisites.h>
#include <yake/data/yakeData.h>
#include <yake/graphics/yakeGraphics.h>

#pragma warning(disable: 4244)

namespace yake {
namespace data {
namespace parser {
namespace dotscene {

	/** DotSceneParser base class providing default implementation for DOM parsing of dotScene files.
	* \todo read environment, external references ..
	*/
	class YAKE_LOADER_API DotSceneParser
	{
	public:
		
		struct Desc
		{
			String		name;
			String		id;
			String		parentNodeName;
			String		path;
		};
		
		struct Transform
		{
			math::Vector3		position;
			math::Quaternion	rotation;
			math::Vector3		scale; 
		};
		
		struct NodeDesc : public Desc
		{
			Transform	transform;
		};
		
		struct EntityDesc : public Desc
		{
			String		meshFile;
			String		materialFile;
			bool		castsShadows;
			bool		isStatic;
		};
		
		struct CameraDesc : public Desc
		{
			real					fov;
			real					aspectRatio;
			graphics::ICamera::ProjectionType	projectionType;
			String					trackTargetName;
			math::Vector3				normal;
			
			struct Clipping
			{
				real nearClip;
				real farClip;
			} clipping;
		};
		
		struct LightDesc : public Desc
		{
			graphics::ILight::LightType	type;
			bool				visible;
			bool				castsShadows;
			math::Color			diffuseColor;
			math::Color			specularColor;
			math::Vector3			normal;
			
			struct Range
			{
				real inner;
				real outer;
				real falloff;
			} range;
			
			struct Attenuation
			{
				real range;
				real constant;
				real linear;
				real quadratic;
			}							attenuation;
			
			LightDesc() : visible( true ), castsShadows( false )
			{}
		};
		
		typedef AssocVector< String, NodeDesc > SceneNodeDescMap;
		typedef AssocVector< String, EntityDesc > EntityDescMap;
		typedef AssocVector< String, CameraDesc > CameraDescMap;
		typedef AssocVector< String, LightDesc > LightDescMap;

		static const String ROOT_NODE_NAME;
		
		virtual ~DotSceneParser() {}

		String getName() const
		{ return "yake.data.dotScene"; }

		virtual Version getVersion() const = 0;

		/** Build graphics scene from DOM!
			\param file 
			\return 
		*/
		virtual bool load(	const SharedPtr<dom::INode>& docNode );

		/** Prepare for next run of load/store.
		*/
		virtual void reset();
		
		const SceneNodeDescMap& getSceneNodeDescriptions() { return mSNDescriptions; }
		const EntityDescMap& getEntityDescriptions() { return mEntityDescriptions; }
		const CameraDescMap& getCameraDescriptions() { return mCameraDescriptions; }
		const LightDescMap& getLightDescriptions() { return mLightDescriptions; }
		
		typedef SignalX< void(const NodeDesc) > NodeSignal;
		typedef SignalX< void(const EntityDesc) > EntitySignal;
		typedef SignalX< void(const CameraDesc) > CameraSignal;
		typedef SignalX< void(const LightDesc) > LightSignal;
		
		
		SignalConnection subscribeToNodeSignal( const NodeSignal::slot_type& slot )
		{ return mSigNode.connect(slot); } 
		
		SignalConnection subscribeToEntitySignal( const EntitySignal::slot_type& slot )
		{ return mSigEntity.connect(slot); } 
	
		SignalConnection subscribeToCameraSignal( const CameraSignal::slot_type& slot )
		{ return mSigCamera.connect(slot); } 
	
		SignalConnection subscribeToLightSignal( const LightSignal::slot_type& slot )
		{ return mSigLight.connect(slot); } 
		
	private:
		NodeSignal		mSigNode;
		EntitySignal	mSigEntity;
		CameraSignal	mSigCamera;
		LightSignal		mSigLight;

	protected:
		/// Default implementations for common functions

		virtual void readScene( const SharedPtr<dom::INode>& pNode );
		
		virtual void readNodes( const SharedPtr<dom::INode>& pNodes, const String& path );
		
		virtual void readNode( const SharedPtr<dom::INode>& pNode, const String& parentNodeName, const String& path );
		virtual void readEntity( const SharedPtr<dom::INode>& pNode, const String& parentNodeName, const String& path );
		
		virtual void readRotation( const SharedPtr<dom::INode>& pNode, math::Quaternion& rotation );
		virtual void readPosition( const SharedPtr<dom::INode>& pNode, math::Vector3& position );
		virtual void readScale( const SharedPtr<dom::INode>& pNode, math::Vector3& rScale );
		virtual void readVector( const SharedPtr<dom::INode>& pNode, math::Vector3& rVec );
		
		virtual void readLight( const SharedPtr<dom::INode>& pNode, const String& parentNodeName, const String& path );
		
		virtual void readColour( const SharedPtr<dom::INode>& pNode, math::Color& colour );
		virtual void readLightRange( const SharedPtr<dom::INode>& pNode, LightDesc& );
		virtual void readLightAttenuation( const SharedPtr<dom::INode>& pNode, LightDesc& );
		
		virtual void readCamera( const SharedPtr<dom::INode>& pNode, const String& parentNodeName, const String& path );
		
		virtual void readCameraClipping( const SharedPtr<dom::INode>& pNode, CameraDesc& );

	private:
		SharedPtr<dom::INode>	mDocNode;

		SceneNodeDescMap		mSNDescriptions;
		EntityDescMap			mEntityDescriptions;
		CameraDescMap			mCameraDescriptions;
		LightDescMap			mLightDescriptions;
		
		
	};

	class YAKE_LOADER_API DotSceneParserV1 : public DotSceneParser
	{
		YAKE_DECLARE_CLASS( DotSceneParserV1 );
	public:
		virtual Version getVersion() const
		{ return Version( 0, 1, 2 ); }
	};

	///HACK It is here for backwards compatibility
	typedef DotSceneParserV1 DotSceneSerializerV1;

} // dotscene
} // parser
} // data
} // yake

#endif
