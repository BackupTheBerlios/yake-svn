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
	class XODEParser
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
		virtual bool load( const SharedPtr<dom::INode>& docNode,  physics::IWorld* pPWorld );

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
			
			const Transform getDerivedTransform( Transform const& parentTransform )
			{
				Transform result;
				
				Quaternion const& rParRot = parentTransform.mRotation;
				Vector3 const& rParPos = parentTransform.mPosition;
				Vector3 const& rParScale = parentTransform.mScale;
				
				result.mPosition = parentTransform.mPosition 
					+ rParRot*( mPosition*rParScale );
					
				result.mRotation = rParRot*mRotation;
				
				result.mScale = mScale*rParScale;;  
				
				return result;
			}
		};
		
		/// Default implementations for common functions.
		

		virtual void readXODE( SharedPtr<dom::INode> const& pNode );
		virtual void readWorld( SharedPtr<dom::INode> const& pWorldNode );
		virtual void readSpace( SharedPtr<dom::INode> const& pSpaceNode );
		virtual void readBody( SharedPtr<dom::INode> const& pBodyNode );
		virtual void readGeom( SharedPtr<dom::INode> const& pGeomNode, physics::IComplexObject* pParentObject = NULL, Transform const& rParentTransform = Transform() );
		virtual void readBox( SharedPtr<dom::INode> const& pNode, real& sX, real& sY, real& sZ );
		virtual void readCappedCylinder( SharedPtr<dom::INode> const& pNode, real& radius, real& length );
		virtual void readCone( SharedPtr<dom::INode> const& pNode, real& radius, real& length );
		virtual void readCylinder( SharedPtr<dom::INode> const& pNode, real& radius, real& length );
		virtual void readPlane( SharedPtr<dom::INode> const& pNode, real& a, real& b, real& c, real& d );
		virtual void readRay( SharedPtr<dom::INode> const& pNode, real& length );
		virtual void readSphere( SharedPtr<dom::INode> const& pNode, real& radius );
		virtual void readTrimesh( SharedPtr<dom::INode> const& pNode, physics::ICollisionGeometry* pCollGeom );
		
		virtual void readTransform( SharedPtr<dom::INode> const& pTransformNode, Transform& rTrans );

		virtual void readVector3( SharedPtr<dom::INode> const& pVecNode, Vector3& rVec );  
		virtual void readPosition( SharedPtr<dom::INode> const& pPosNode, Vector3& rPos );  
		virtual void readQuaternion( SharedPtr<dom::INode> const& pQuatNode, Quaternion& rQuat );
		virtual void readRotation( SharedPtr<dom::INode> const& pRotNode, Quaternion& rRot );
		virtual void readScale( SharedPtr<dom::INode> const& pScaleNode, Vector3& rScale );  
		virtual void readAxisAngleRot( SharedPtr<dom::INode> const& pRotNode, Quaternion& rRot );
		virtual void readMass( SharedPtr<dom::INode> const& pMassNode, physics::IBody* pBody, Transform const& rParentTransform ); 
		virtual void readMassShape( SharedPtr<dom::INode> const& pMShapeNode, physics::IBody* pBody, Transform const& rParentTransform ); 
		virtual void readMassAdjust( SharedPtr<dom::INode> const& pMAdjustNode, physics::IBody* pBody ); 
				
// 		readGroup
// 		readJointgroup
// 		virtual void readJoint
// 		virtual void readMass
// 		virtual void readMassStruct
// 		readMassShape
// 		readTransform
		
		
		
		
/*		virtual void readScene( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN );
		virtual void readNodes( const SharedPtr<dom::INode> & pNodes, graphics::ISceneNode* pParentSN );
		virtual void readNode( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN );
		virtual void readEntity( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN );
		virtual void readOrientation( const SharedPtr<dom::INode> & pNode, Quaternion & orientation );
		virtual void readPosition( const SharedPtr<dom::INode> & pNode, Vector3 & position );*/
		
	private:
		SharedPtr<dom::INode>					mDocNode;
		physics::IWorld*					mPWorld;
		model::Physical&					mBaseModel;
	};
	
	class XODEParserV1 : public XODEParser
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
