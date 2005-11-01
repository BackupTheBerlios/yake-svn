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
#include <yapp/model/yakePhysical.h>

namespace yake {
namespace data {
namespace parser {
namespace xode {

	/** XODEParser base class providing default implementation for DOM parsing.
	* \todo read lights, environment, external references ..
	*/
	class YAPP_BASE_API XODEParser
	{
	public:
		typedef SharedPtr<dom::INode> NodeSharedPtr;
		
		XODEParser( model::Physical& rBaseModel );
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
		virtual bool load( const NodeSharedPtr docNode,  physics::IWorld* pPWorld );

		/** Prepare for next run of load/store.
		*/
		virtual void reset();
	
	protected:
		/// XODE "transform" node representation
		struct Transform
		{
			Vector3 	mPosition;
			Quaternion	mRotation;
			Vector3		mScale;
			
			Transform() : mScale( Vector3::kUnitScale )
			{
			}
			
			bool isIdentity() const
			{
				bool result = mScale == Vector3::kUnitScale;
				result &= mPosition == Vector3::kZero;
				result &= mRotation == Quaternion::kZero;
				
				return result;
			}
			
			const Transform getDerivedTransform( const Transform& parentTransform )
			{
				Transform result;
				
				Quaternion const& rParRot = parentTransform.mRotation;
				Vector3 const& rParPos = parentTransform.mPosition;
				Vector3 const& rParScale = parentTransform.mScale;
				
				result.mPosition = parentTransform.mPosition 
					+ rParRot*( mPosition*rParScale );
					
				result.mRotation = rParRot*mRotation;
				
				result.mScale = mScale*rParScale;  
				
				return result;
			}
		};
		
		/// Default implementations for common functions.
		

		virtual void readXODE( const NodeSharedPtr pNode );
		virtual void readWorld( const NodeSharedPtr pWorldNode );
		virtual void readSpace( const NodeSharedPtr pSpaceNode );
		virtual void readBody( const NodeSharedPtr pBodyNode );
		virtual void readGeom( const NodeSharedPtr pGeomNode, physics::IActor* pParentObject = NULL, const Transform& rParentTransform = Transform() );
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
		virtual void readMass( const NodeSharedPtr pMassNode, physics::IBody& rBody, const Transform& rParentTransform ); 
		virtual void readMassShape( const NodeSharedPtr pMShapeNode, physics::IBody& rBody, const Transform& rParentTransform ); 
		virtual void readMassAdjust( const NodeSharedPtr pMAdjustNode, physics::IBody& rBody ); 
		virtual void readJoint( const NodeSharedPtr pJointNode, const String& rBodyName, Transform& rParentTransform );
		
		virtual void readBall( const NodeSharedPtr pJointNode, physics::IActor* pActor1, physics::IActor* pActor2, Transform& rJointTransform );
		virtual void readFixed( const NodeSharedPtr pJointNode, physics::IActor* pActor1, physics::IActor* pActor2, Transform& rJointTransform );
		virtual void readHinge( const NodeSharedPtr pJointNode, physics::IActor* pActor1, physics::IActor* pActor2, Transform& rJointTransform );
		virtual void readHinge2( const NodeSharedPtr pJointNode, physics::IActor* pActor1, physics::IActor* pActor2, Transform& rJointTransform );
		virtual void readSlider( const NodeSharedPtr pJointNode, physics::IActor* pActor1, physics::IActor* pActor2, Transform& rJointTransform );
		virtual void readUniversal( const NodeSharedPtr pJointNode, physics::IActor* pActor1, physics::IActor* pActor2, Transform& rJointTransform );
		
		virtual void readAnchor( const NodeSharedPtr pAnchorNode, Vector3& rAnchor, Transform& rJointTransform );
		virtual void readAxis( const NodeSharedPtr pAxisNode, Vector3& rAxis, Transform& rJointTransform );
		virtual String XODEParser::readMaterialExt( const NodeSharedPtr pNode );
		
// 		readGroup
// 		readJointgroup
// 		virtual void readMassStruct
		
		struct JointDescription
		{
			String					mBody1Name; /// name of 2nd body is inside joint def  node
			NodeSharedPtr	mJointDefNode;
			Transform				mParentTransform;
		};
	
		typedef Vector<JointDescription> JointDescriptions;
		JointDescriptions			mJointDescriptions;
		
		/// Joint creation is only possible at post-parse step
		void createJointsFromDescriptions(); 
		void createJoint( const JointDescription& rDesc );
		
	private:
		NodeSharedPtr			mDocNode;
		physics::IWorld*					mPWorld;
		model::Physical&					mBaseModel;
	};
	
	class YAPP_BASE_API XODEParserV1 : public XODEParser
	{
	public:
		XODEParserV1( model::Physical& rBaseModel ):XODEParser( rBaseModel ) {}
		virtual ~XODEParserV1() {}
		
		virtual Version getVersion() const
		{ 
			return Version( 0, 1, 0 );
		}
	};

} // dotscene
} // parser
} // data
} // yake

#endif
