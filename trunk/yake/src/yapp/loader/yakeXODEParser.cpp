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

#include <yapp/base/yappPCH.h>
#include <yapp/base/yapp.h>
#include <yapp/loader/yakeXODEParser.h>

namespace yake {
namespace data {
namespace parser {
namespace xode {

	using namespace yake::app;
	using namespace yake::base;
	
	//------------------------------------------------------
	XODEParser::XODEParser( model::Physical& rBaseModel ) : mBaseModel( rBaseModel )
	{
	}

	//------------------------------------------------------
	bool XODEParser::load(  const SharedPtr<dom::INode> docNode, physics::IWorld* pPWorld )
	{
		std::cout << "load()" << std::endl;
		
		YAKE_ASSERT( docNode.get() );
		YAKE_ASSERT( pPWorld );
		
		mDocNode = docNode;
		mPWorld = pPWorld;
		
		/// clearing out...
		mJointDescriptions.clear();
		
		std::cout << "parsing xode..." << std::endl;
		
		readXODE( mDocNode );
	
		return true;
	}
	
	//------------------------------------------------------
	void XODEParser::readXODE( const SharedPtr<dom::INode> pNode )
	{
		YAKE_ASSERT( pNode );
		
		const String name = pNode->getAttributeValueAs<String>( "name" );
		std::cout << "readXODE() [" << name << "]" << std::endl;
		
		// Assuming single world in .xode
		SharedPtr<dom::INode> pWorldNode = pNode->getNodeByName( "world" );
 		YAKE_ASSERT( pWorldNode );
				
		std::cout << "xode: found world = " << ( pWorldNode.get() ? "yes!" : "no :(" ) << std::endl;
		
		readWorld( pWorldNode );
		
		createJointsFromDescriptions();
	}
	
	//------------------------------------------------------
	void XODEParser::readWorld( const SharedPtr<dom::INode> pWorldNode )
	{
		YAKE_ASSERT( pWorldNode );
		
		// Assuming single space in <world>
		SharedPtr<dom::INode> pSpaceNode = pWorldNode->getNodeByName( "space" );
 		YAKE_ASSERT( pSpaceNode );
				
		std::cout << "xode::world: found space = " << ( pSpaceNode.get() ? "yes!" : "no :(" ) << std::endl;
		
		readSpace( pSpaceNode );
	}
	
	//------------------------------------------------------
	void XODEParser::reset()
	{
		mDocNode.reset();
	}

	//------------------------------------------------------
	void XODEParser::readSpace( const SharedPtr<dom::INode> pSpaceNode )
	{
		std::cout << "readSpace()" << std::endl;
			
		const dom::NodeList& nodes = pSpaceNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String nodeName = 
				yake::base::StringUtil::toLowerCase( (*it)->getValueAs<String>( "name" ) );
			
			if ( nodeName == "body" )
			{
				readBody( *it );
			}
			else if ( nodeName == "geom" )
			{
				readGeom( *it );
			}
			else if ( nodeName == "bodygroup" )
			{
				//TODO bodygroup reading here
			}
		}
	}

	/// Utility functor for use with std::find_if
	//------------------------------------------------------
	class NodeHunter : public std::unary_function<SharedPtr<dom::INode>&,bool>
	{
	public:
		NodeHunter( String const& rNodeName ):mNodeName( rNodeName )
		{
		}
		
		bool operator()( SharedPtr<dom::INode>const& pNode )
		{
			return pNode->getValueAs<String>( "name" ) == mNodeName; 
		}
		
	private:
		String mNodeName;
	}; 
	
	//------------------------------------------------------
	void XODEParser::readVector3( const SharedPtr<dom::INode> pVecNode, Vector3& rVec )
	{
		real x = StringUtil::parseReal( pVecNode->getAttributeValueAs<String>( "x" ) );
		real y = StringUtil::parseReal( pVecNode->getAttributeValueAs<String>( "y" ) );
		real z = StringUtil::parseReal( pVecNode->getAttributeValueAs<String>( "z" ) );
		
		rVec = Vector3( x, y, z );
	}

	//------------------------------------------------------
	void XODEParser::readScale( const SharedPtr<dom::INode> pScaleNode, Vector3& rScale )
	{
		readVector3( pScaleNode, rScale );
	}

	//------------------------------------------------------
	void XODEParser::readPosition( const SharedPtr<dom::INode> pPosNode, Vector3& rPos )
	{
		readVector3( pPosNode, rPos );
	}

	//------------------------------------------------------
	/**
	 *        Reads quaternion from pQuatNode to rQuat
	 *        Ignores aformat attribute  
	 * @param pQuatNode 
	 * @param rQuat 
	 */
	void XODEParser::readQuaternion( const SharedPtr<dom::INode> pQuatNode, Quaternion& rQuat )
	{
		real x = StringUtil::parseReal( pQuatNode->getAttributeValueAs<String>( "x" ) );
		real y = StringUtil::parseReal( pQuatNode->getAttributeValueAs<String>( "y" ) );
		real z = StringUtil::parseReal( pQuatNode->getAttributeValueAs<String>( "z" ) );
		real w = StringUtil::parseReal( pQuatNode->getAttributeValueAs<String>( "w" ) );
		
		rQuat = Quaternion( w, x, y, z );
	}
	
	//------------------------------------------------------
	/**
	 *        Reads angle and axis x, y, z attributes and spawns quaternion with them in rRot
	 * @param pRotNode 
	 * @param rRot 
	 */
	void XODEParser::readAxisAngleRot( const SharedPtr<dom::INode> pRotNode, Quaternion& rRot )
	{
		real x = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "x" ) );
		real y = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "y" ) );
		real z = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "z" ) );
		real angle = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "angle" ) );
		
		rRot.FromAngleAxis( angle, Vector3( x, y, z ) );
	}
	  
	//------------------------------------------------------
	void XODEParser::readRotation( const SharedPtr<dom::INode> pRotNode, Quaternion& rRot )
	{
		const dom::NodeList& nodes = pRotNode->getNodes();
		
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>("name") );

			if ( nodeName == "quaternion" )
			{
				readQuaternion( *it, rRot );
			}
			else if ( nodeName == "euler" )
			{
				YAKE_ASSERT( false ).error( "euler is NYI" );
			}
			else if ( nodeName == "axisangle" )
			{
				readAxisAngleRot( *it, rRot );
			}
		}
	}

	//------------------------------------------------------
	void XODEParser::readTransform( const SharedPtr<dom::INode> pTransformNode, Transform& rTrans )
	{
		const dom::NodeList& nodes = pTransformNode->getNodes();
		
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>("name") );
		
			if ( nodeName == "position" )
			{
				readPosition( *it, rTrans.mPosition );
			}
			else if ( nodeName == "rotation" )
			{
				readRotation( *it, rTrans.mRotation );
			}
			else if ( nodeName == "scale" )
			{
				readScale( *it, rTrans.mScale );
			}
			else if ( nodeName == "matrix4f" )
			{
				YAKE_ASSERT( false ).error( "matrix4f is NYI" );
			}
		}
	}

	//------------------------------------------------------
	void XODEParser::readMassShape( const SharedPtr<dom::INode> pMShapeNode, 
					physics::IBody& rBody, 
					Transform const& rParentTransform )
	{
	
		real density = StringUtil::parseReal( pMShapeNode->getAttributeValueAs<String>( "density" ) );

		std::cout << "reading mass_shape with density = " << density << " " << std::endl;
	
		const dom::NodeList& nodes = pMShapeNode->getNodes();
		
		typedef dom::NodeList::const_iterator NodeListIter;
		for ( NodeListIter it = nodes.begin(); it != nodes.end(); ++it)
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>( "name" ) );
		
			std::cout << "reading mass_shape that is " << nodeName << " " << std::endl;

			if ( nodeName == "cylinder" )
			{
				real radius, length;
				readCylinder( *it, radius, length );
				
				physics::IBody::CylinderMassDesc desc( radius, length, density, rParentTransform.mPosition );
				
				rBody.addMass( desc );
			}
			else if ( nodeName == "sphere" )
			{
				real radius;
				readSphere( *it, radius );
				
				physics::IBody::SphereMassDesc desc( radius, density, rParentTransform.mPosition );
				
				rBody.addMass( desc );
			}
			else if ( nodeName == "box" )
			{
				real sX, sY, sZ;
				readBox( *it, sX, sY, sZ );
				
				physics::IBody::BoxMassDesc desc( sX, sY, sZ, density, rParentTransform.mPosition );
				
				rBody.addMass( desc );
			}
			else if ( nodeName == "cappedCylinder" )
			{
				real radius, length;
				readCappedCylinder( *it, radius, length );
				
				physics::IBody::CapsuleMassDesc desc( radius, length, density, rParentTransform.mPosition );
				
				rBody.addMass( desc );
			}
		}
	}
		
	//------------------------------------------------------
	void XODEParser::readMassAdjust( const SharedPtr<dom::INode> pMAdjustNode, 
					 physics::IBody& rBody )
	{
			real total = StringUtil::parseReal( pMAdjustNode->getAttributeValueAs<String>( "total" ) );
			
			rBody.setMass( total );
	}
	
	//------------------------------------------------------
	void XODEParser::readMass( const SharedPtr<dom::INode> pMassNode, physics::IBody& rBody, Transform const& rParentTransform )
	{
		const dom::NodeList& nodes = pMassNode->getNodes();

		// Seek transform
		Transform massTransform;
		
		NodeHunter hunter( "transform" );
		typedef dom::NodeList::const_iterator NodeListIter;
		NodeListIter iTrans = 
			std::find_if<NodeListIter,NodeHunter>( nodes.begin(), nodes.end(), hunter );
		
		if ( iTrans != nodes.end() )
		{
			readTransform( *iTrans, massTransform );
		}
		
		if ( !rParentTransform.isIdentity() )
		{
			massTransform = massTransform.getDerivedTransform( rParentTransform );
		}
		
		for ( NodeListIter it = nodes.begin(); it != nodes.end(); ++it)
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>( "name" ) );

			if ( nodeName == "mass_shape" )
			{
				readMassShape( *it, rBody, massTransform );
			}
			else if ( nodeName == "adjust" )
			{
				readMassAdjust( *it, rBody );
			}
			else if ( nodeName == "mass" )
			{
				readMass( *it, rBody, massTransform );
			}
		}
		
	} 

	//------------------------------------------------------
	void XODEParser::readBody( const SharedPtr<dom::INode> pBodyNode )
	{
		const String name = pBodyNode->getAttributeValueAs<String>( "name" );
		std::cout << "readBody() [" << name << "]" << std::endl;
		
		physics::IDynamicActor::Desc actorDesc;
		
		physics::WeakIDynamicActorPtr pDynActor = mPWorld->createDynamicActor( actorDesc );
		YAKE_ASSERT( !pDynActor.expired() ).error( "Failed to create actor!" );
		
		mBaseModel.addActor( physics::WeakIActorPtr( pDynActor ), name );
		
		const dom::NodeList& nodes = pBodyNode->getNodes();
		
		
		// Seek transform
		Transform bodyTransform;
		
		NodeHunter hunter( "transform" );
		typedef dom::NodeList::const_iterator NodeListIter;
		NodeListIter iTrans = 
			std::find_if<NodeListIter,NodeHunter>( nodes.begin(), nodes.end(), hunter );
		
		if ( iTrans != nodes.end() )
		{
			std::cout << "readBody() [" << name << "] : body has transform " << std::endl;

			readTransform( *iTrans, bodyTransform );
		}
		
		if ( !bodyTransform.isIdentity() )
		{
			pDynActor.lock()->setPosition( bodyTransform.mPosition );
			pDynActor.lock()->setOrientation( bodyTransform.mRotation );
		}

		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>( "name" ) );

			std::cout << " body [" << name << "] has node #" << nodeName << std::endl;
			
			if ( nodeName == "geom" )
			{
				readGeom( *it, pDynActor.lock().get(), bodyTransform );
			}
			else if ( nodeName == "mass" )
			{
				
				readMass( *it, pDynActor.lock()->getBody(), bodyTransform );
			}
			else if ( nodeName == "joint" )
			{
				readJoint( *it, name, bodyTransform );
			}
		}
	}
	
	
	//------------------------------------------------------
	/**
	 *        Reads geom xode node and attaches ICollisionGeometry* to *pParentObject.
	 *        If no parent object supplied then attaches geometry to mBaseModel directly.
	 *        No folded bodys, geoms, spaces processed.
	 * @param pGeomNode 
	 * @param pParentObject 
	 */
	void XODEParser::readGeom( const SharedPtr<dom::INode> pGeomNode, 
				   physics::IActor* pParentObject,
				   const Transform& rParentTransform )
	{
		const String name = pGeomNode->getAttributeValueAs<String>( "name" );
		std::cout << "readGeom() [" << name << "]" << std::endl;
		
		physics::IShape* pShape = NULL;

		if ( !pParentObject )
		{
			YAKE_LOG("XODE::readGeom() creating static actor...");
			// adding shape to new IStaticActor
			//physics::IStaticActor::Desc desc;
			physics::WeakIStaticActorPtr pStaticActor( mPWorld->createStaticActor() );
			pParentObject = pStaticActor.lock().get();
			mBaseModel.addActor( physics::WeakIActorPtr( pStaticActor ), name );
		}

		const dom::NodeList& nodes = pGeomNode->getNodes();
		
		// Looking for transform
		NodeHunter hunter( "transform" );
		typedef dom::NodeList::const_iterator NodeListIter;
		NodeListIter iTrans = 
			std::find_if<NodeListIter,NodeHunter>( nodes.begin(), nodes.end(), hunter );

		Transform geomTransform;
			
		if ( iTrans != nodes.end() )
		{
			std::cout << "readGeom() [" << name << "] : geom has transform " << std::endl;

			readTransform( *iTrans, geomTransform );
		}

		
///FIXME We don't need derived transform here as it is automaitcally handled by concrete plugin
/*		if ( !rParentTransform.isIdentity() )
		{
			geomTransform = geomTransform.getDerivedTransform( rParentTransform );
		}*/
		
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>( "name" ) );

			std::cout << " geom [" << name << "] has node #" << nodeName << std::endl;

			if ( nodeName == "box" )
			{
				real sizeX, sizeY, sizeZ;
				readBox( *it, sizeX, sizeY, sizeZ );
				
				sizeX *= rParentTransform.mScale.x;
				sizeY *= rParentTransform.mScale.y;
				sizeZ *= rParentTransform.mScale.z;
			
				std::cout << " box with sizes: " << sizeX << ", " << sizeY << ", " << sizeZ << std::endl;
				
				physics::IShape::BoxDesc desc( 
												Vector3( sizeX, sizeY, sizeZ ), 
												physics::WeakIMaterialPtr(), 						// FIXME material
												geomTransform.mPosition,
												geomTransform.mRotation );
										
				YAKE_ASSERT( pParentObject ).warning( "FIXME Static shapes are not supported" );
				if (pParentObject)
					pParentObject->createShape( desc );
			}
			else if ( nodeName == "cappedCylinder" )
			{
				real radius, length;
				readCappedCylinder( *it, radius, length );
				
				real scale = ( rParentTransform.mScale.x 
						+ rParentTransform.mScale.y
						+ rParentTransform.mScale.z ) /3;
					
				radius *= scale;
				length *= scale;
				
				physics::IShape::CapsuleDesc desc( 
												length,
												radius, 
												physics::WeakIMaterialPtr(), 						// FIXME material
												geomTransform.mPosition,
												geomTransform.mRotation );
										
				YAKE_ASSERT( pParentObject ).warning( "FIXME Static shapes are not supported" );
				if (pParentObject)
					pParentObject->createShape( desc );
			}
			else if ( nodeName == "cone" )
			{
				real radius, length;
				readCone( *it, radius, length );
				
				real scale = ( rParentTransform.mScale.x 
						+ rParentTransform.mScale.y
						+ rParentTransform.mScale.z ) /3;
					
				radius *= scale;
				length *= scale;
				
				YAKE_ASSERT( false ).error( "NOT implemented in Yake" ); //TODO
			}
			else if ( nodeName == "cylinder" )
			{
				real radius, length;
				readCylinder( *it, radius, length );
				
				real scale = ( rParentTransform.mScale.x 
						+ rParentTransform.mScale.y
						+ rParentTransform.mScale.z ) /3;
				
				radius *= scale;
				length *= scale;
				
				physics::IShape::CapsuleDesc desc( 
												length,
												radius, 
												physics::WeakIMaterialPtr(), 						// FIXME material
												geomTransform.mPosition,
												geomTransform.mRotation );
										
				YAKE_ASSERT( pParentObject ).warning( "FIXME Static shapes are not supported" );
				if (pParentObject)
					pParentObject->createShape( desc );
			}
			else if ( nodeName == "plane" )
			{
				real a, b, c, d;
				readPlane( *it, a, b, c, d );
				
				physics::IShape::PlaneDesc desc( 
												Vector3( a, b, c ),
												d, 
												physics::WeakIMaterialPtr(), 						// FIXME material
												geomTransform.mPosition,
												geomTransform.mRotation );
										
				YAKE_ASSERT( pParentObject ).warning( "FIXME Static shapes are not supported" );
				if (pParentObject)
					pParentObject->createShape( desc );
			}
			else if ( nodeName == "ray" )
			{
				real length;
				readRay( *it, length );
				
				real scale = ( rParentTransform.mScale.x 
						+ rParentTransform.mScale.y
						+ rParentTransform.mScale.z ) /3;
					
				length *= scale;
				
				YAKE_ASSERT( false ).error( "NOT implemented in Yake" ); //TODO
			}
			else if ( nodeName == "sphere" )
			{
				real radius;
				readSphere( *it, radius );
				
				real scale = ( rParentTransform.mScale.x 
						+ rParentTransform.mScale.y
						+ rParentTransform.mScale.z ) /3;
					
				radius *= scale;
				
				physics::IShape::SphereDesc desc( 
												radius, 
												physics::WeakIMaterialPtr(), 						// FIXME material
												geomTransform.mPosition,
												geomTransform.mRotation );
										
				YAKE_ASSERT( pParentObject ).warning( "FIXME Static shapes are not supported" );
				if (pParentObject)
					pParentObject->createShape( desc );
			}
			else if ( nodeName == "trimesh" )
			{
				physics::TriangleMeshDesc::VertexVector vertices;
				physics::TriangleMeshDesc::IndexVector indices;
				
				readTrimesh( *it, vertices, indices );
				
				physics::TriangleMeshDesc::VertexVector scaledVertices;
				
				ConstVectorIterator< physics::TriangleMeshDesc::VertexVector > iV( vertices.begin(), vertices.end() );
				
				while ( iV.hasMoreElements() )
				{
					Vector3 const& vertex = iV.getNext();
					scaledVertices.push_back( vertex * rParentTransform.mScale );
				}
				
				physics::TriangleMeshDesc dataDesc( vertices, indices );
				
				physics::TriangleMeshId meshId = mPWorld->createTriangleMesh( dataDesc );
				
				physics::IShape::TriMeshDesc desc( 
												meshId, 
												physics::WeakIMaterialPtr(), 						// FIXME material
												geomTransform.mPosition,
												geomTransform.mRotation );
										
				YAKE_ASSERT( pParentObject ).warning( "FIXME Static shapes are not supported" );
				if (pParentObject)
					pParentObject->createShape( desc );
			}
		}
	}

	//------------------------------------------------------
	void XODEParser::readBox( const SharedPtr<dom::INode> pNode, real& sizeX, real& sizeY, real& sizeZ )
	{
		sizeX = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "sizex" ) );
		sizeY = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "sizey" ) );
		sizeZ = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "sizez" ) );
	}
	
	//------------------------------------------------------
	void XODEParser::readCappedCylinder( const SharedPtr<dom::INode> pNode, real& radius, real& length )
	{
		radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
		length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
	}

	//------------------------------------------------------
	void XODEParser::readCone( const SharedPtr<dom::INode> pNode, real& radius, real& length )
	{
		radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
		length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
	}

	//------------------------------------------------------
	void XODEParser::readCylinder( const SharedPtr<dom::INode> pNode, real& radius, real& length )
	{
		radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
		length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
	}

	//------------------------------------------------------
	void XODEParser::readPlane( const SharedPtr<dom::INode> pNode, real& a, real& b, real& c, real& d )
	{
		a = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "a" ) );
		b = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "b" ) );
		c = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "c" ) );
		d = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "d" ) );
	}

	//------------------------------------------------------
	void XODEParser::readRay( const SharedPtr<dom::INode> pNode, real& length )
	{
		length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
	}

	//------------------------------------------------------
	void XODEParser::readSphere( const SharedPtr<dom::INode> pNode, real& radius )
	{
		radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
	}

	//------------------------------------------------------
	void XODEParser::readTrimesh( const SharedPtr<dom::INode> pMeshNode,
				      physics::TriangleMeshDesc::VertexVector& rVertices,
				      physics::TriangleMeshDesc::IndexVector& rIndices )
	{
		const dom::NodeList& nodes = pMeshNode->getNodes();
		
		typedef dom::NodeList::const_iterator NodeListIter;
		for ( NodeListIter it = nodes.begin(); it != nodes.end(); ++it )
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>( "name" ) );
		
			std::cout << "   Reading " << nodeName << " now..." << std::endl;
			
			if ( nodeName == "vertices" )
			{
				readVertices( *it, rVertices );
			}
			else if ( nodeName == "triangles" )
			{
				readIndices( *it, rIndices );
			}
		}
	}

	//------------------------------------------------------
	void XODEParser::readVertices( const SharedPtr<dom::INode> pNode, 
				       physics::TriangleMeshDesc::VertexVector& rVertices )
	{
		const dom::NodeList& nodes = pNode->getNodes();
		
		typedef dom::NodeList::const_iterator NodeListIter;
		for ( NodeListIter it = nodes.begin(); it != nodes.end(); ++it )
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>( "name" ) );
		
			if ( nodeName == "v" )
			{
				real x = StringUtil::parseReal( (*it)->getAttributeValueAs< String >( "x" ) );
				real y = StringUtil::parseReal( (*it)->getAttributeValueAs< String >( "y" ) );
				real z = StringUtil::parseReal( (*it)->getAttributeValueAs< String >( "z" ) );
			
				rVertices.push_back( Vector3( x, y, z ) );
			}
		}
	}

	//------------------------------------------------------
	void XODEParser::readIndices( const SharedPtr<dom::INode> pNode, 
				       physics::TriangleMeshDesc::IndexVector& rIndices )
	{
		const dom::NodeList& nodes = pNode->getNodes();
		
		typedef int32 IndexT;
		
		typedef dom::NodeList::const_iterator NodeListIter;
		for ( NodeListIter it = nodes.begin(); it != nodes.end(); ++it )
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>( "name" ) );
		
			if ( nodeName == "t" )
			{
				IndexT ia = StringUtil::parseInt( (*it)->getAttributeValueAs< String >( "ia" ) );
				IndexT ib = StringUtil::parseInt( (*it)->getAttributeValueAs< String >( "ib" ) );
				IndexT ic = StringUtil::parseInt( (*it)->getAttributeValueAs< String >( "ic" ) );
			
				rIndices.push_back( ia );
				rIndices.push_back( ib );
				rIndices.push_back( ic );
			}
		}
	}
	
	//------------------------------------------------------
	void XODEParser::readJoint(	const SharedPtr<dom::INode> pJointNode,
								const String& rBodyName,
								Transform& rParentTransform )
	{
		JointDescription desc;
		
		desc.mBody1Name = rBodyName;
		desc.mJointDefNode = pJointNode;
		desc.mParentTransform = rParentTransform;
		
		mJointDescriptions.push_back( desc );
	}
	
	//------------------------------------------------------
	void XODEParser::createJointsFromDescriptions()
	{
		for( JointDescriptions::iterator j = mJointDescriptions.begin(); j != mJointDescriptions.end(); ++j )
			createJoint( *j );
	} 
	
	//------------------------------------------------------
	void XODEParser::createJoint( const JointDescription& rDesc )
	{
		
		const dom::NodeList& nodes = rDesc.mJointDefNode->getNodes();
		
		String name = rDesc.mJointDefNode->getAttributeValueAs<String>( "name" ); 
		
		typedef dom::NodeList::const_iterator NodeListIter;
		
		// Looking for link2
		NodeHunter linkHunter( "link2" );
		NodeListIter iLink = 
			std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), linkHunter );
		
		YAKE_ASSERT( iLink != nodes.end() ).error( "2nd body name is not present in joint definition!" );
		
		String body2Name = (*iLink)->getAttributeValueAs<String>( "body" );
		
		// Looking for transform
		NodeHunter hunter( "transform" );
		NodeListIter iTrans = 
			std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), hunter );
			
		Transform jointTransform;
			
		if ( iTrans != nodes.end() )
		{
			std::cout << "readJoint() [" << name << "] : joint has transform " << std::endl;

			readTransform( *iTrans, jointTransform );
		}
		
		if ( !rDesc.mParentTransform.isIdentity() )
		{
			jointTransform = jointTransform.getDerivedTransform( rDesc.mParentTransform );
		}
		
		physics::WeakIActorPtr pActor1 = mBaseModel.getActorByName( rDesc.mBody1Name );
		physics::WeakIActorPtr pActor2 = mBaseModel.getActorByName( body2Name );
		
		physics::IDynamicActor* pDynActor1 = dynamic_cast<physics::IDynamicActor*>( pActor1.lock().get() );
		physics::IDynamicActor* pDynActor2 = dynamic_cast<physics::IDynamicActor*>( pActor2.lock().get() );

		YAKE_ASSERT( pDynActor1 && pDynActor2 ).error( "You're trying to attach joint to non-dynamic actors! That's not possible!" );
				
		for ( NodeListIter it = nodes.begin(); it != nodes.end(); ++it )
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>( "name" ) );
		
			std::cout << "   Reading " << nodeName << " now..." << std::endl;
			
			if ( nodeName == "ball" )
			{
				readBall( *it, pDynActor1, pDynActor2, jointTransform );
			}
			else if ( nodeName == "fixed" )
			{
				readFixed( *it, pDynActor1, pDynActor2, jointTransform );
			}
			else if ( nodeName == "hinge" )
			{
				readHinge( *it, pDynActor1, pDynActor2, jointTransform );
			}
			else if ( nodeName == "hinge2" )
			{
				readHinge2( *it, pDynActor1, pDynActor2, jointTransform );
			}
			else if ( nodeName == "slider" )
			{
				readSlider( *it, pDynActor1, pDynActor2, jointTransform );
			}
			else if ( nodeName == "universal" )
			{
				readUniversal( *it, pDynActor1, pDynActor2, jointTransform );
			}
			else if ( nodeName == "amotor" )
			{
				YAKE_ASSERT( false ).error( "amotor joint type is not supported!" );
			}
		}
	}
	
	//------------------------------------------------------
	void XODEParser::readBall(	const SharedPtr<dom::INode> pJointNode,
								physics::IDynamicActor* pActor1,
								physics::IDynamicActor* pActor2,
								Transform& rJointTransform )
	{
		const dom::NodeList& nodes = pJointNode->getNodes();
		
		typedef dom::NodeList::const_iterator NodeListIter;
		
		// Looking for anchor
		NodeHunter anchorHunter( "anchor" );
		NodeListIter iAnchor = 
			std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), anchorHunter );
		
		YAKE_ASSERT( iAnchor != nodes.end() ).error( "Anchor was not defined for ball joint!" );
	
		Vector3 anchor;
		
		readAnchor( *iAnchor, anchor, rJointTransform );
		
		physics::IJoint::DescBall desc( *pActor1, *pActor2, anchor );
		
		mPWorld->createJoint( desc ); 
	}

	//------------------------------------------------------
	void XODEParser::readFixed(	const SharedPtr<dom::INode> pJointNode,
								physics::IDynamicActor* pActor1,
								physics::IDynamicActor* pActor2,
								Transform& rJointTransform )
	{
		physics::IJoint::DescFixed desc( *pActor1, *pActor2 );
		
		mPWorld->createJoint( desc ); 
	}
	
	//------------------------------------------------------
	void XODEParser::readHinge(	const SharedPtr<dom::INode> pJointNode,
								physics::IDynamicActor* pActor1,
								physics::IDynamicActor* pActor2,
								Transform& rJointTransform )
	{
		const dom::NodeList& nodes = pJointNode->getNodes();
		
		typedef dom::NodeList::const_iterator NodeListIter;
		
		// Looking for anchor
		NodeHunter anchorHunter( "anchor" );
		NodeListIter iAnchor = 
			std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), anchorHunter );
		
		YAKE_ASSERT( iAnchor != nodes.end() ).error( "Anchor was not defined for hinge joint!" );
		
		// Looking for axis
		NodeHunter axisHunter( "axis" );
		NodeListIter iAxis = 
			std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), axisHunter );
		
		YAKE_ASSERT( iAxis != nodes.end() ).error( "Axis was not defined for hinge joint!" );
	
		Vector3 anchor;
		Vector3 axis;
		
		readAnchor( *iAnchor, anchor, rJointTransform );
		readAxis( *iAxis, axis, rJointTransform );
		
		physics::IJoint::DescHinge desc( *pActor1, *pActor2, axis, anchor );
		
		mPWorld->createJoint( desc ); 
	}
	
	//------------------------------------------------------
	void XODEParser::readHinge2(	const SharedPtr<dom::INode> pJointNode,
								physics::IDynamicActor* pActor1,
								physics::IDynamicActor* pActor2,
								Transform& rJointTransform )
	{
		const dom::NodeList& nodes = pJointNode->getNodes();
		
		typedef dom::NodeList::const_iterator NodeListIter;
		
		// Looking for anchor
		NodeHunter anchorHunter( "anchor" );
		NodeListIter iAnchor = 
			std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), anchorHunter );
		
		YAKE_ASSERT( iAnchor != nodes.end() ).error( "Anchor was not defined for hinge joint!" );
		
		// Looking for axis
		NodeHunter axisHunter( "axis" );
		NodeListIter iAxis0 = 
			std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), axisHunter );
		
		YAKE_ASSERT( iAxis0 != nodes.end() ).error( "Axis0 was not defined for hinge2 joint!" );
		
		NodeListIter iAxis1 = 
			std::find_if<NodeListIter, NodeHunter>( iAxis0 + 1, nodes.end(), axisHunter );
		
		YAKE_ASSERT( iAxis1 != nodes.end() ).error( "Axis1 was not defined for hinge2 joint!" );
	
		Vector3 anchor;
		Vector3 axis0;
		Vector3 axis1;
		
		readAnchor( *iAnchor, anchor, rJointTransform );
		readAxis( *iAxis0, axis0, rJointTransform );
		readAxis( *iAxis1, axis1, rJointTransform );
		
		physics::IJoint::DescHinge2 desc( *pActor1, *pActor2, axis0, axis1, anchor );
		
		mPWorld->createJoint( desc ); 
	}
	
	//------------------------------------------------------
	void XODEParser::readSlider(	const SharedPtr<dom::INode> pJointNode,
								physics::IDynamicActor* pActor1,
								physics::IDynamicActor* pActor2,
								Transform& rJointTransform )
	{
		const dom::NodeList& nodes = pJointNode->getNodes();
		
		typedef dom::NodeList::const_iterator NodeListIter;
		
		// Looking for axis
		NodeHunter axisHunter( "axis" );
		NodeListIter iAxis = 
			std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), axisHunter );
		
		YAKE_ASSERT( iAxis != nodes.end() ).error( "Axis was not defined for hinge joint!" );
	
		Vector3 axis;
		
		readAxis( *iAxis, axis, rJointTransform );
		
		physics::IJoint::DescSlider desc( *pActor1, *pActor2, axis );
		
		mPWorld->createJoint( desc ); 
	}
	
	//------------------------------------------------------
	void XODEParser::readUniversal(	const SharedPtr<dom::INode> pJointNode,
									physics::IDynamicActor* pActor1,
									physics::IDynamicActor* pActor2,
									Transform& rJointTransform )
	{
		const dom::NodeList& nodes = pJointNode->getNodes();
		
		typedef dom::NodeList::const_iterator NodeListIter;
		
		// Looking for anchor
		NodeHunter anchorHunter( "anchor" );
		NodeListIter iAnchor = 
			std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), anchorHunter );
		
		YAKE_ASSERT( iAnchor != nodes.end() ).error( "Anchor was not defined for hinge joint!" );
		
		// Looking for axis
		NodeHunter axisHunter( "axis" );
		NodeListIter iAxis0 = 
			std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), axisHunter );
		
		YAKE_ASSERT( iAxis0 != nodes.end() ).error( "Axis0 was not defined for hinge2 joint!" );
		
		NodeListIter iAxis1 = 
			std::find_if<NodeListIter, NodeHunter>( iAxis0 + 1, nodes.end(), axisHunter );
		
		YAKE_ASSERT( iAxis1 != nodes.end() ).error( "Axis1 was not defined for hinge2 joint!" );
	
		Vector3 anchor;
		Vector3 axis0;
		Vector3 axis1;
		
		readAnchor( *iAnchor, anchor, rJointTransform );
		readAxis( *iAxis0, axis0, rJointTransform );
		readAxis( *iAxis1, axis1, rJointTransform );
		
		physics::IJoint::DescUniversal desc( *pActor1, *pActor2, axis0, axis1, anchor );
		
		mPWorld->createJoint( desc ); 
	}
	
	//------------------------------------------------------
	void XODEParser::readAnchor(	const SharedPtr<dom::INode> pAnchorNode,
									Vector3& rAnchor,
									Transform& rJointTransform )
	{
		rAnchor.x = StringUtil::parseReal( pAnchorNode->getAttributeValueAs<String>( "x" ) );
		rAnchor.y = StringUtil::parseReal( pAnchorNode->getAttributeValueAs<String>( "y" ) );
		rAnchor.z = StringUtil::parseReal( pAnchorNode->getAttributeValueAs<String>( "z" ) );
		
		rAnchor = rJointTransform.mPosition + rJointTransform.mRotation*( rJointTransform.mScale*rAnchor );
	}
	
	//------------------------------------------------------
	void XODEParser::readAxis(	const SharedPtr<dom::INode> pAxisNode,
								Vector3& rAxis,
								Transform& rJointTransform )
	{
		rAxis.x = StringUtil::parseReal( pAxisNode->getAttributeValueAs<String>( "x" ) );
		rAxis.y = StringUtil::parseReal( pAxisNode->getAttributeValueAs<String>( "y" ) );
		rAxis.z = StringUtil::parseReal( pAxisNode->getAttributeValueAs<String>( "z" ) );
		
		/// TODO What about all other axis params?
		
		rAxis = rJointTransform.mRotation*rAxis;
	}

} //xode
} //parser
} //data
} //yake
