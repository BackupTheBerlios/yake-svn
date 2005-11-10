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
#ifndef YAKE_DATA_XODEPARSER_H
#define YAKE_DATA_XODEPARSER_H

#include <yake/data/yakeData.h>
#include <yake/physics/yakePhysics.h>
#include <yapp/model/yakePhysical.h>

namespace yake {
namespace data {
namespace parser {
namespace xode {

	/** XODEParser base class providing default implementation for DOM parsing.
	*/
	class YAPP_BASE_API XODEParser
	{
	public:
		typedef SharedPtr<dom::INode> NodeSharedPtr;
		typedef dom::NodeList::const_iterator NodeListIter;

		
		XODEParser();
		virtual ~XODEParser() {}

		String getName() const
		{ 
			return "yake.data.xode";
		}

		virtual Version getVersion() const = 0;

		/** Build physics model from DOM!
			\param file 
			\return 
		*/
		virtual bool load( const NodeSharedPtr docNode );

		/** Prepare for next run of load/store.
		*/
		virtual void reset();
		
		/// XODE "transform" node representation
		struct Transform
		{
			Vector3 	position_;
			Quaternion	rotation_;
			Vector3		scale_;
			
			Transform() : scale_( Vector3::kUnitScale )
			{
			}
			
			bool isIdentity() const
			{
				bool result = scale_ == Vector3::kUnitScale;
				result &= position_ == Vector3::kZero;
				result &= rotation_ == Quaternion::kZero;
				
				return result;
			}
			
			const Transform getDerivedTransform( const Transform& parentTransform )
			{
				Transform result;
				
				Quaternion const& rParRot = parentTransform.rotation_;
				Vector3 const& rParPos = parentTransform.position_;
				Vector3 const& rParScale = parentTransform.scale_;
				
				result.position_ = parentTransform.position_ 
					+ rParRot*( position_*rParScale );
					
				result.rotation_ = rParRot*rotation_;
				
				result.scale_ = scale_*rParScale;  
				
				return result;
			}
		};

		// Event-spawning parser makes heavy use of these
		struct Desc
		{
		  String	name_;
		  String	parentName_;
		  Transform	transform_;
		};

		struct BodyDesc : public Desc
		{
		};

		struct GeomDesc : public Desc
		{
		    SharedPtr<physics::IShape::Desc> shape_;
		};

		struct MassDesc : public Desc // ??? is this needed??
		{
		  SharedPtr<physics::IBody::MassDesc> mass_;
		};

		struct JointDesc : public Desc
		{
		  String otherBody_;

		  SharedPtr<physics::IJoint::DescBase> joint_;
		};

		struct MaterialDesc : public Desc // ?? no... not needed
		{
		  physics::IMaterial::Desc material_;
		};
		
		typedef SignalX< void( const BodyDesc& ) > BodySignal;
		typedef SignalX< void( const GeomDesc& ) > GeomSignal;
		typedef SignalX< void( const MassDesc& ) > MassSignal;
		typedef SignalX< void( const JointDesc& ) > JointSignal;
		typedef SignalX< void( const MaterialDesc& ) > MaterialSignal;
		typedef SignalX< void ( void ) > EventSignal; // special event handler type
	
		SignalConnection subscribeToBodySignal( const BodySignal::slot_type& slot )
		{ return sigBody_.connect(slot); } 
		SignalConnection subscribeToGeomSignal( const GeomSignal::slot_type& slot )
		{ return sigGeom_.connect(slot); } 
		SignalConnection subscribeToMassSignal( const MassSignal::slot_type& slot )
		{ return sigMass_.connect(slot); } 
		SignalConnection subscribeToJointSignal( const JointSignal::slot_type& slot )
		{ return sigJoint_.connect(slot); } 
		SignalConnection subscribeToMaterialSignal( const MaterialSignal::slot_type& slot )
		{ return sigMaterial_.connect(slot); } 

		SignalConnection subscribeToParseEndedSignal( const EventSignal::slot_type& slot )
		{ return sigParseEnded_.connect( slot ); } 

		typedef AssocVector< String, BodyDesc > BodyDescMap;
		typedef AssocVector< String, GeomDesc > GeomDescMap;
		typedef AssocVector< String, MassDesc > MassDescMap;
		typedef AssocVector< String, JointDesc > JointDescMap;
		typedef AssocVector< String, MaterialDesc > MaterialDescMap;

		const BodyDescMap& getBodyDescriptions() { return bodyDescriptions_; }
		const GeomDescMap& getGeomDescriptions() { return geomDescriptions_; }
		const MassDescMap& getMassDescriptions() { return massDescriptions_; }
		const JointDescMap& getJointDescriptions() { return jointDescriptions_; }
		const MaterialDescMap& getMaterialDescriptions() { return materialDescriptions_; }
		
	protected:
		BodySignal	sigBody_;
		GeomSignal	sigGeom_;
		MassSignal	sigMass_;
		JointSignal	sigJoint_;
		MaterialSignal	sigMaterial_;
		EventSignal	sigParseEnded_;
	
		BodyDescMap bodyDescriptions_;
		GeomDescMap geomDescriptions_;
		MassDescMap massDescriptions_;
		JointDescMap jointDescriptions_;
		MaterialDescMap materialDescriptions_;
		
		/// Default implementations for common functions.
		virtual void readXODE( const NodeSharedPtr pNode );
		virtual void readWorld( const NodeSharedPtr pWorldNode );
		virtual void readSpace( const NodeSharedPtr pSpaceNode, const String& parent );
		virtual void readBody( const NodeSharedPtr pBodyNode, const String& parent );
		virtual void readGeom( const NodeSharedPtr pGeomNode, const String& parent , const Transform& rParentTransform = Transform() );
		virtual void readBox( const NodeSharedPtr pNode, real& sX, real& sY, real& sZ );
		virtual void readCappedCylinder( const NodeSharedPtr pNode, real& radius, real& length );
		virtual void readCone( const NodeSharedPtr pNode, real& radius, real& length );
		virtual void readCylinder( const NodeSharedPtr pNode, real& radius, real& length );
		virtual void readPlane( const NodeSharedPtr pNode, real& a, real& b, real& c, real& d );
		virtual void readRay( const NodeSharedPtr pNode, real& length );
		virtual void readSphere( const NodeSharedPtr pNode, real& radius );
		virtual void readTrimesh( const NodeSharedPtr pNode, physics::TriangleMeshDesc::VertexVector& rVertices, physics::TriangleMeshDesc::IndexVector& rIndices );
		virtual void readVertices( const NodeSharedPtr pNode, physics::TriangleMeshDesc::VertexVector& rVertices );
		virtual void readIndices( const NodeSharedPtr pNode, physics::TriangleMeshDesc::IndexVector& rIndices );
		
		virtual void readTransform( const NodeSharedPtr pTransformNode, Transform& rTrans );

		virtual void readVector3( const NodeSharedPtr pVecNode, Vector3& rVec );  
		virtual void readPosition( const NodeSharedPtr pPosNode, Vector3& rPos );  
		virtual void readQuaternion( const NodeSharedPtr pQuatNode, Quaternion& rQuat );
		virtual void readRotation( const NodeSharedPtr pRotNode, Quaternion& rRot );
		virtual void readScale( const NodeSharedPtr pScaleNode, Vector3& rScale );  
		virtual void readAxisAngleRot( const NodeSharedPtr pRotNode, Quaternion& rRot );
		virtual void readMass( const NodeSharedPtr pMassNode, const String& parent , const Transform& rParentTransform ); 
		virtual void readMassShape( const NodeSharedPtr pMShapeNode, const String& parent, const Transform& rParentTransform ); 
		virtual void readMassAdjust( const NodeSharedPtr pMAdjustNode, const String& parent ); 
		virtual void readJoint( const NodeSharedPtr pJointNode, const String& parent , Transform& rParentTransform );
		
		virtual physics::IJoint::DescBase* readBall( const NodeSharedPtr pJointNode, Transform& rJointTransform );
		virtual physics::IJoint::DescBase* readFixed( const NodeSharedPtr pJointNode, Transform& rJointTransform );
		virtual physics::IJoint::DescBase* readHinge( const NodeSharedPtr pJointNode, Transform& rJointTransform );
		virtual physics::IJoint::DescBase* readHinge2( const NodeSharedPtr pJointNode, Transform& rJointTransform );
		virtual physics::IJoint::DescBase* readSlider( const NodeSharedPtr pJointNode, Transform& rJointTransform );
		virtual physics::IJoint::DescBase* readUniversal( const NodeSharedPtr pJointNode, Transform& rJointTransform );
		
		virtual void readAnchor( const NodeSharedPtr pAnchorNode, Vector3& rAnchor, Transform& rJointTransform );
		virtual void readAxis( const NodeSharedPtr pAxisNode, Vector3& rAxis, Transform& rJointTransform );
		virtual void readMaterialExt( const NodeSharedPtr pNode, const String& parent  );

	private:
		NodeSharedPtr		mDocNode;
	};
	
	class YAPP_BASE_API XODEParserV1 : public XODEParser
	{
	public:
		virtual ~XODEParserV1() {}
		
		virtual Version getVersion() const
		{ 
			return Version( 0, 5, 0 );
		}
	};

} // xode
} // parser
} // data
} // yake

#endif
