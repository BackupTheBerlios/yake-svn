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
#include <yapp/loader/yakeXODE.h>

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
	bool XODEParser::load(  SharedPtr<dom::INode> const& docNode, physics::IWorld* pPWorld )
	{
		std::cout << "load()" << std::endl;
		
		YAKE_ASSERT( docNode.get() );
		YAKE_ASSERT( pPWorld );
		
		mDocNode = docNode;
		mPWorld = pPWorld;
		
		std::cout << "parsing xode..." << std::endl;
		
		readXODE( mDocNode );
	
		return true;
	}
	
	//------------------------------------------------------
	void XODEParser::readXODE( SharedPtr<dom::INode> const& pNode )
	{
		YAKE_ASSERT( pNode );
		
		const String name = pNode->getAttributeValueAs<String>( "name" );
		std::cout << "readXODE() [" << name << "]" << std::endl;
		
		// Assuming single world in .xode
		SharedPtr<dom::INode> pWorldNode = pNode->getNodeByName( "world" );
 		YAKE_ASSERT( pWorldNode );
				
		std::cout << "xode: found world = " << ( pWorldNode.get() ? "yes!" : "no :(" ) << std::endl;
		
		readWorld( pWorldNode );
	}
	
	//------------------------------------------------------
	void XODEParser::readWorld( SharedPtr<dom::INode> const& pWorldNode )
	{
		YAKE_ASSERT( pNode );
		
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
	void XODEParser::readSpace( SharedPtr<dom::INode> const& pSpaceNode )
	{
		std::cout << "readSpace()" << std::endl;
			
		dom::NodeList const& nodes = pSpaceNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String nodeName = 
				yake::base::StringUtil::toLowerCase( (*it)->getValueAs<String>("name") );
			
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
	void XODEParser::readVector3( SharedPtr<dom::INode> const& pVecNode, Vector3& rVec )
	{
		real x = StringUtil::parseReal( pVecNode->getAttributeValueAs<String>( "x" ) );
		real y = StringUtil::parseReal( pVecNode->getAttributeValueAs<String>( "y" ) );
		real z = StringUtil::parseReal( pVecNode->getAttributeValueAs<String>( "z" ) );
		
		rVec = Vector3( x, y, z );
	}

	//------------------------------------------------------
	void XODEParser::readScale( SharedPtr<dom::INode> const& pScaleNode, Vector3& rScale )
	{
		readVector3( pScaleNode, rScale );
	}

	//------------------------------------------------------
	void XODEParser::readPosition( SharedPtr<dom::INode> const& pPosNode, Vector3& rPos )
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
	void XODEParser::readQuaternion( SharedPtr<dom::INode> const& pQuatNode, Quaternion& rQuat )
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
	void XODEParser::readAxisAngleRot( SharedPtr<dom::INode> const& pRotNode, Quaternion& rRot )
	{
		real x = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "x" ) );
		real y = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "y" ) );
		real z = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "z" ) );
		real angle = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "angle" ) );
		
		rRot.FromAngleAxis( angle, Vector3( x, y, z ) );
	}
	  
	//------------------------------------------------------
	void XODEParser::readRotation( SharedPtr<dom::INode> const& pRotNode, Quaternion& rRot )
	{
		dom::NodeList const& nodes = pRotNode->getNodes();
		
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
	void XODEParser::readTransform( SharedPtr<dom::INode> const& pTransformNode, Transform& rTrans )
	{
		dom::NodeList const& nodes = pTransformNode->getNodes();
		
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
	void XODEParser::readMassShape( SharedPtr<dom::INode> const& pMShapeNode, 
					physics::IBody* pBody, 
					Transform const& rParentTransform )
	{
	
		real density = StringUtil::parseReal( pMShapeNode->getAttributeValueAs<String>( "density" ) );

		std::cout << "reading mass_shape with density = " << density << " " << std::endl;
	
		dom::NodeList const& nodes = pMShapeNode->getNodes();
		
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
				YAKE_ASSERT( false ).error( "cylinder mass is NYI" );
			}
			else if ( nodeName == "sphere" )
			{
				real radius;
				readSphere( *it, radius );
				pBody->setMassSphere( radius, density );
			}
			else if ( nodeName == "box" )
			{
				real sX, sY, sZ;
				readBox( *it, sX, sY, sZ );
				pBody->setMassBox( sX, sY, sZ, density );
			}
			else if ( nodeName == "cappedCylinder" )
			{
				real radius, length;
				readCappedCylinder( *it, radius, length );
				YAKE_ASSERT( false ).error( "cappedCylinder mass is NYI" );
			}
		}
		
		pBody->translateMass( rParentTransform.mPosition );
	}
		
	//------------------------------------------------------
	void XODEParser::readMassAdjust( SharedPtr<dom::INode> const& pMAdjustNode, 
					 physics::IBody* pBody )
	{
			real total = StringUtil::parseReal( pMAdjustNode->getAttributeValueAs<String>( "total" ) );
			
			pBody->setMass( total );
	}
	
	//------------------------------------------------------
	void XODEParser::readMass( SharedPtr<dom::INode> const& pMassNode, physics::IBody* pBody, Transform const& rParentTransform )
	{
		dom::NodeList const& nodes = pMassNode->getNodes();

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
				StringUtil::toLowerCase( (*it)->getValueAs<String>("name") );

			if ( nodeName == "mass_shape" )
			{
				readMassShape( *it, pBody, massTransform );
			}
			else if ( nodeName == "adjust" )
			{
				readMassAdjust( *it, pBody );
			}
			else if ( nodeName == "mass" )
			{
				readMass( *it, pBody, massTransform );
			}
		}
		
	} 

	//------------------------------------------------------
	void XODEParser::readBody( SharedPtr<dom::INode> const& pBodyNode )
	{
		const String name = pBodyNode->getAttributeValueAs<String>( "name" );
		std::cout << "readBody() [" << name << "]" << std::endl;
		
		SharedPtr<physics::IComplexObject> pComplexObj( mPWorld->createEmptyPhysicsObject() );
		YAKE_ASSERT( pComplexObj );
		
		physics::IBody* pBody = mPWorld->createBody();
		YAKE_ASSERT( pBody );
		
		pComplexObj->setBody( pBody );
		mBaseModel.addComplex( pComplexObj , name );
		
		dom::NodeList const& nodes = pBodyNode->getNodes();
		
		
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
			pBody->setPosition( bodyTransform.mPosition );
			pBody->setOrientation( bodyTransform.mRotation );
		}

		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>("name") );

			std::cout << " body [" << name << "] has node #" << nodeName << std::endl;
			
			if ( nodeName == "geom" )
			{
				readGeom( *it, pComplexObj.get(), bodyTransform );
			}
			else if ( nodeName == "mass" )
			{
				readMass( *it, pBody, bodyTransform );
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
	void XODEParser::readGeom( SharedPtr<dom::INode> const& pGeomNode, 
				   physics::IComplexObject* pParentObject,
				   Transform const& rParentTransform )
	{
		const String name = pGeomNode->getAttributeValueAs<String>( "name" );
		std::cout << "readGeom() [" << name << "]" << std::endl;
		
		physics::ICollisionGeometry* pCollGeom = NULL;

		if ( pParentObject == NULL )
		{
			// adding coll. geometry to new empty IComplexObject
			pParentObject = mPWorld->createEmptyPhysicsObject();
			SharedPtr<physics::IComplexObject> pCO( pParentObject );
			mBaseModel.addComplex( pCO, name );
		}

		dom::NodeList const& nodes = pGeomNode->getNodes();
		
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
		
		if ( !rParentTransform.isIdentity() )
		{
			geomTransform = geomTransform.getDerivedTransform( rParentTransform );
		}
		
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String nodeName = 
				StringUtil::toLowerCase( (*it)->getValueAs<String>("name") );

			std::cout << " geom [" << name << "] has node #" << nodeName << std::endl;

			//TODO Refactor this ifelse out (a map?)
			// Refactor: map[name|func_returning_ptr_to_coll_geom]
			// Refactor: pCollGeom = funcs[ name ]( *it )
			if ( nodeName == "box" )
			{
				real sizeX, sizeY, sizeZ;
				readBox( *it, sizeX, sizeY, sizeZ );
				pCollGeom = mPWorld->createCollisionGeomBox( sizeX, sizeY, sizeZ );
			}
			else if ( nodeName == "cappedCylinder" )
			{
				real radius, length;
				readCappedCylinder( *it, radius, length );
				YAKE_ASSERT( false ).error( "NOT implemented in Yake" ); //TODO
			}
			else if ( nodeName == "cone" )
			{
				real radius, length;
				readCone( *it, radius, length );
				YAKE_ASSERT( false ).error( "NOT implemented in Yake" ); //TODO
			}
			else if ( nodeName == "cylinder" )
			{
				real radius, length;
				readCylinder( *it, radius, length );
				YAKE_ASSERT( false ).error( "NOT implemented in Yake" ); //TODO
			}
			else if ( nodeName == "plane" )
			{
				real a, b, c, d;
				readPlane( *it, a, b, c, d );
				
				// FIXME Special case: not placeable geometry!
				// This should be fixed in yake::physics interfaces
				if ( !geomTransform.isIdentity() )
				{
					Vector3 normal( a, b, c );
					
					normal = geomTransform.mRotation*normal;
					d += normal.dotProduct( geomTransform.mPosition );
					
					a = normal[0];
					b = normal[1];
					c = normal[2];
				}
				
				pCollGeom = mPWorld->createCollisionGeomPlane( a, b, c, d );
			}
			else if ( nodeName == "ray" )
			{
				real length;
				readRay( *it, length );
				YAKE_ASSERT( false ).error( "NOT implemented in Yake" ); //TODO
			}
			else if ( nodeName == "sphere" )
			{
				real radius;
				readSphere( *it, radius );
				pCollGeom = mPWorld->createCollisionGeomSphere( radius );
			}
			else if ( nodeName == "trimesh" )
			{
			//	readTrimesh( *it, pCollGeom );
			}
		}

		if ( !geomTransform.isIdentity() )
		{
			// FIXME For now plane is a special case
			if ( pCollGeom->getType() != physics::ICollisionGeometry::CGT_PLANE )
			{
				physics::ICollisionGeometry* pCG = pCollGeom;
				pCollGeom = mPWorld->createCollisionGeomTransform();

				std::cout << "!! Setting transform !!:" << std::endl;
				std::cout << " 	position: " << geomTransform.mPosition[0] << "," << 
					geomTransform.mPosition[1] << "," <<
					geomTransform.mPosition[2] << std::endl;
				pCollGeom->setPosition( geomTransform.mPosition );
				pCollGeom->setOrientation( geomTransform.mRotation );
	
				pCollGeom->tfAttachGeom( pCG );
			}
		}
		
		pParentObject->addCollisionGeometry( pCollGeom );
	}

	//------------------------------------------------------
	void XODEParser::readBox( SharedPtr<dom::INode> const& pNode, real& sizeX, real& sizeY, real& sizeZ )
	{
		sizeX = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "sizex" ) );
		sizeY = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "sizey" ) );
		sizeZ = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "sizez" ) );
	}
	
	//------------------------------------------------------
	void XODEParser::readCappedCylinder( SharedPtr<dom::INode> const& pNode, real& radius, real& length )
	{
		radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
		length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
	}

	//------------------------------------------------------
	void XODEParser::readCone( SharedPtr<dom::INode> const& pNode, real& radius, real& length )
	{
		radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
		length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
	}

	//------------------------------------------------------
	void XODEParser::readCylinder( SharedPtr<dom::INode> const& pNode, real& radius, real& length )
	{
		radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
		length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
	}

	//------------------------------------------------------
	void XODEParser::readPlane( SharedPtr<dom::INode> const& pNode, real& a, real& b, real& c, real& d )
	{
		a = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "a" ) );
		b = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "b" ) );
		c = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "c" ) );
		d = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "d" ) );
	}

	//------------------------------------------------------
	void XODEParser::readRay( SharedPtr<dom::INode> const& pNode, real& length )
	{
		length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
	}

	//------------------------------------------------------
	void XODEParser::readSphere( SharedPtr<dom::INode> const& pNode, real& radius )
	{
		radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
	}

	//------------------------------------------------------
	void XODEParser::readTrimesh( SharedPtr<dom::INode> const& pNode,
				  physics::ICollisionGeometry* pCollGeom )
	{
		// FIXME pointer is passed by value!!!
		YAKE_ASSERT( false ).error( "Foul implementation" ); //TODO
	}

} //xode
} //parser
} //data
} //yake
