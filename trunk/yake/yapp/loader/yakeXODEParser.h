#ifndef YAKE_DATA_DOTSCENE_H
#define YAKE_DATA_DOTSCENE_H

#include <yake/data/yakeData.h>
#include <yapp/model/yakePhysical.h>

namespace yake {
namespace data {
namespace parser {
namespace xode {

using namespace yake::app; // model::Physical

	/** XODEParser base class providing default implementation for DOM parsing.
	* \todo read lights, environment, external references ..
	*/
	class YAPP_BASE_API XODEParser
	{
	public:
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
		virtual bool load( const SharedPtr<dom::INode> docNode,  physics::IWorld* pPWorld );

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
		

		virtual void readXODE( const SharedPtr<dom::INode> pNode );
		virtual void readWorld( const SharedPtr<dom::INode> pWorldNode );
		virtual void readSpace( const SharedPtr<dom::INode> pSpaceNode );
		virtual void readBody( const SharedPtr<dom::INode> pBodyNode );
		virtual void readGeom( const SharedPtr<dom::INode> pGeomNode, physics::IActor* pParentObject = NULL, const Transform& rParentTransform = Transform() );
		virtual void readBox( const SharedPtr<dom::INode> pNode, real& sX, real& sY, real& sZ );
		virtual void readCappedCylinder( const SharedPtr<dom::INode> pNode, real& radius, real& length );
		virtual void readCone( const SharedPtr<dom::INode> pNode, real& radius, real& length );
		virtual void readCylinder( const SharedPtr<dom::INode> pNode, real& radius, real& length );
		virtual void readPlane( const SharedPtr<dom::INode> pNode, real& a, real& b, real& c, real& d );
		virtual void readRay( const SharedPtr<dom::INode> pNode, real& length );
		virtual void readSphere( const SharedPtr<dom::INode> pNode, real& radius );
		virtual void readTrimesh( const SharedPtr<dom::INode> pNode, physics::TriangleMeshDesc::VertexVector& rVertices, physics::TriangleMeshDesc::IndexVector& rIndices );
		virtual void readVertices( const SharedPtr<dom::INode> pNode, physics::TriangleMeshDesc::VertexVector& rVertices );
		virtual void readIndices( const SharedPtr<dom::INode> pNode, physics::TriangleMeshDesc::IndexVector& rIndices );
		
		virtual void readTransform( const SharedPtr<dom::INode> pTransformNode, Transform& rTrans );

		virtual void readVector3( const SharedPtr<dom::INode> pVecNode, Vector3& rVec );  
		virtual void readPosition( const SharedPtr<dom::INode> pPosNode, Vector3& rPos );  
		virtual void readQuaternion( const SharedPtr<dom::INode> pQuatNode, Quaternion& rQuat );
		virtual void readRotation( const SharedPtr<dom::INode> pRotNode, Quaternion& rRot );
		virtual void readScale( const SharedPtr<dom::INode> pScaleNode, Vector3& rScale );  
		virtual void readAxisAngleRot( const SharedPtr<dom::INode> pRotNode, Quaternion& rRot );
		virtual void readMass( const SharedPtr<dom::INode> pMassNode, physics::IBody& rBody, const Transform& rParentTransform ); 
		virtual void readMassShape( const SharedPtr<dom::INode> pMShapeNode, physics::IBody& rBody, const Transform& rParentTransform ); 
		virtual void readMassAdjust( const SharedPtr<dom::INode> pMAdjustNode, physics::IBody& rBody ); 
		virtual void readJoint( const SharedPtr<dom::INode> pJointNode, const String& rBodyName, Transform& rParentTransform );
		
		virtual void readBall( const SharedPtr<dom::INode> pJointNode, physics::IDynamicActor* pActor1, physics::IDynamicActor* pActor2, Transform& rJointTransform );
		virtual void readFixed( const SharedPtr<dom::INode> pJointNode, physics::IDynamicActor* pActor1, physics::IDynamicActor* pActor2, Transform& rJointTransform );
		virtual void readHinge( const SharedPtr<dom::INode> pJointNode, physics::IDynamicActor* pActor1, physics::IDynamicActor* pActor2, Transform& rJointTransform );
		virtual void readHinge2( const SharedPtr<dom::INode> pJointNode, physics::IDynamicActor* pActor1, physics::IDynamicActor* pActor2, Transform& rJointTransform );
		virtual void readSlider( const SharedPtr<dom::INode> pJointNode, physics::IDynamicActor* pActor1, physics::IDynamicActor* pActor2, Transform& rJointTransform );
		virtual void readUniversal( const SharedPtr<dom::INode> pJointNode, physics::IDynamicActor* pActor1, physics::IDynamicActor* pActor2, Transform& rJointTransform );
		
		virtual void readAnchor( const SharedPtr<dom::INode> pAnchorNode, Vector3& rAnchor, Transform& rJointTransform );
		virtual void readAxis( const SharedPtr<dom::INode> pAxisNode, Vector3& rAxis, Transform& rJointTransform );
		
// 		readGroup
// 		readJointgroup
// 		virtual void readMassStruct
		
		struct JointDescription
		{
			String					mBody1Name; /// name of 2nd body is inside joint def  node
			SharedPtr<dom::INode>	mJointDefNode;
			Transform				mParentTransform;
		};
	
		typedef Vector<JointDescription> JointDescriptions;
		JointDescriptions			mJointDescriptions;
		
		/// Joint creation is only possible at post-parse step
		void createJointsFromDescriptions(); 
		void createJoint( const JointDescription& rDesc );
		
	private:
		SharedPtr<dom::INode>			mDocNode;
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
} // serializer
} // data
} // yake

#endif
