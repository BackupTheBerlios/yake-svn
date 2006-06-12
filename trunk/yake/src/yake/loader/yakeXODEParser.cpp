/*
	 ------------------------------------------------------------------------------------
	 This file is part of YAKE
	 Copyright	2004 The YAKE Team
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
#include <yake/loader/pch.h>
#include <yake/loader/loader.h>

namespace yake {
namespace data {
namespace parser {
namespace xode {
    using namespace physics;

    //------------------------------------------------------
    XODEParser::XODEParser() 
    {}

    //------------------------------------------------------
    bool XODEParser::load( const NodeSharedPtr docNode )
    {
		YAKE_LOG( "XODE: load()" );

		YAKE_ASSERT( docNode.get() );

		mDocNode = docNode;

		YAKE_LOG( "*******************" );
		YAKE_LOG( "* Parsing xode... *" );
		YAKE_LOG( "*******************" );

		readXODE( mDocNode );

		return true;
    }

    //------------------------------------------------------
    void XODEParser::readXODE( const NodeSharedPtr pNode )
    {
		YAKE_ASSERT( pNode );

		const String name = pNode->getAttributeValueAs<String>( "name" );
		YAKE_LOG( "XODE: parsing " + name );

		// Assuming single world in .xode
		NodeSharedPtr pWorldNode = pNode->getNodeByName( "world" );
		YAKE_ASSERT( pWorldNode );

		YAKE_LOG( "XODE: finding world ... " + String( pWorldNode.get() ? "Yes!" : "No :(" ) );

		readWorld( pWorldNode );

		// parse completed. Firing signal!
		sigParseEnded_();
    }

    //------------------------------------------------------
    void XODEParser::readWorld( const NodeSharedPtr pWorldNode )
    {
	// Assuming single space in <world>
	NodeSharedPtr pSpaceNode = pWorldNode->getNodeByName( "space" );
	YAKE_ASSERT( pSpaceNode );

	YAKE_LOG( "XODE: finding space... " + String( pSpaceNode.get() ? "Yes!" : "No :(" ) );

	readSpace( pSpaceNode, "world" );
    }

    //------------------------------------------------------
    void XODEParser::reset()
    {
	//TODO is this really needed?
	mDocNode.reset();
    }

    //------------------------------------------------------
    void XODEParser::readSpace( const NodeSharedPtr pSpaceNode, const String& parent )
    {
	YAKE_LOG( "XODE: reading space ..." );

	const dom::NodeList& nodes = pSpaceNode->getNodes();
	for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
	    String nodeName =
		yake::StringUtil::toLowerCase( (*it)->getName() );

	    if ( nodeName == "body" )
	    {
		readBody( *it, parent );
	    }
	    else if ( nodeName == "geom" )
	    {
		readGeom( *it, parent );
	    }
	    else if ( nodeName == "bodygroup" )
	    {
		//TODO bodygroup reading here
		YAKE_LOG( "XODE: Bodygroup is not yet implemented in parser" ); 
	    }
	    else if ( nodeName == "joint" )
	    {
		readJoint( *it, parent );
	    }
	}
    }

    /// Utility functor for use with std::find_if
    //------------------------------------------------------
    class NodeHunter : public std::unary_function<XODEParser::NodeSharedPtr&,bool>
    {
	public:
	    NodeHunter( const String& rNodeName ) : nodeName_( rNodeName )
	    {}

	    bool operator()( const XODEParser::NodeSharedPtr& pNode )
	    {
		return pNode->getName() == nodeName_;
	    }

	private:
	    String nodeName_;
    };

    //------------------------------------------------------
    void XODEParser::readVector3( const NodeSharedPtr pVecNode, math::Vector3& rVec )
    {
	real x = StringUtil::parseReal( pVecNode->getAttributeValueAs<String>( "x" ) );
	real y = StringUtil::parseReal( pVecNode->getAttributeValueAs<String>( "y" ) );
	real z = StringUtil::parseReal( pVecNode->getAttributeValueAs<String>( "z" ) );

	rVec = math::Vector3( x, y, z );
    }

    //------------------------------------------------------
    void XODEParser::readScale( const NodeSharedPtr pScaleNode, math::Vector3& rScale )
    {
	readVector3( pScaleNode, rScale );
    }

    //------------------------------------------------------
    void XODEParser::readPosition( const NodeSharedPtr pPosNode, math::Vector3& rPos )
    {
	readVector3( pPosNode, rPos );
    }

    //------------------------------------------------------
    /**
     * Reads quaternion from pQuatNode to rQuat
     * Ignores aformat attribute
     * @param pQuatNode
     * @param rQuat
     */
    void XODEParser::readQuaternion( const NodeSharedPtr pQuatNode, math::Quaternion& rQuat )
    {
	real x = StringUtil::parseReal( pQuatNode->getAttributeValueAs<String>( "x" ) );
	real y = StringUtil::parseReal( pQuatNode->getAttributeValueAs<String>( "y" ) );
	real z = StringUtil::parseReal( pQuatNode->getAttributeValueAs<String>( "z" ) );
	real w = StringUtil::parseReal( pQuatNode->getAttributeValueAs<String>( "w" ) );

	rQuat = math::Quaternion( w, x, y, z );
    }

    //------------------------------------------------------
    /**
     *	Reads angle and axis x, y, z attributes and spawns quaternion with them in rRot
     * @param pRotNode
     * @param rRot
     */
    void XODEParser::readAxisAngleRot( const NodeSharedPtr pRotNode, math::Quaternion& rRot )
    {
	real x = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "x" ) );
	real y = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "y" ) );
	real z = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "z" ) );
	real angle = StringUtil::parseReal( pRotNode->getAttributeValueAs<String>( "angle" ) );

	rRot.FromAngleAxis( angle, math::Vector3( x, y, z ) );
    }

    //------------------------------------------------------
    void XODEParser::readRotation( const NodeSharedPtr pRotNode, math::Quaternion& rRot )
    {
	const dom::NodeList& nodes = pRotNode->getNodes();

	for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
	    String nodeName = StringUtil::toLowerCase( (*it)->getName() );

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
    void XODEParser::readTransform( const NodeSharedPtr pTransformNode, Transform& rTrans )
    {
	const dom::NodeList& nodes = pTransformNode->getNodes();

	for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
	    String nodeName =
		StringUtil::toLowerCase( (*it)->getName() );

	    if ( nodeName == "position" )
	    {
		readPosition( *it, rTrans.position_ );
	    }
	    else if ( nodeName == "rotation" )
	    {
		readRotation( *it, rTrans.rotation_ );
	    }
	    else if ( nodeName == "scale" )
	    {
		readScale( *it, rTrans.scale_ );
	    }
	    else if ( nodeName == "matrix4f" )
	    {
		YAKE_ASSERT( false ).error( "matrix4f is NYI" );
	    }
	}
    }

    //------------------------------------------------------
    void XODEParser::readMassShape( const NodeSharedPtr pMShapeNode, const String& parent,
	    const Transform& rParentTransform )
    {
	real density = 0; //default ...

	// if density is provided...
	String strDensity = pMShapeNode->getAttributeValueAs<String>( "density" );
	if ( !strDensity.empty() ) 
	{
	    density = StringUtil::parseReal( strDensity );
	    YAKE_LOG( "XODE: reading mass_shape with density = " + strDensity + " " );
	}

	const dom::NodeList& nodes = pMShapeNode->getNodes();
	for ( NodeListIter it = nodes.begin(); it != nodes.end(); ++it)
	{
	    String nodeName = StringUtil::toLowerCase( (*it)->getName() );

	    YAKE_LOG( "XODE: reading mass_shape that is " + nodeName + " " );

	    IBody::MassDesc* physicsMassDesc = NULL;

	    if ( nodeName == "cylinder" )
	    {
		real radius, length;
		readCylinder( *it, radius, length );

		physicsMassDesc = new IBody::CylinderMassDesc( radius, length, density,
			rParentTransform.position_,
			IBody::QT_DENSITY );
	    }
	    else if ( nodeName == "sphere" )
	    {
		real radius; readSphere( *it, radius );

		physicsMassDesc = new IBody::SphereMassDesc(radius, density,
			rParentTransform.position_,
			IBody::QT_DENSITY );
	    }
	    else if ( nodeName == "box" )
	    {
		real sX, sY, sZ;
		readBox( *it, sX, sY, sZ );

		physicsMassDesc = new IBody::BoxMassDesc( sX, sY, sZ, density,
			rParentTransform.position_,
			IBody::QT_DENSITY );
	    }
	    else if ( nodeName == "cappedCylinder" )
	    {
		real radius, length;
		readCappedCylinder( *it, radius, length );

		physicsMassDesc = new IBody::CapsuleMassDesc( radius, length, density,
			rParentTransform.position_,
			IBody::QT_DENSITY );
	    }

	    if ( physicsMassDesc != NULL )
	    {

		MassDesc desc;
		desc.mass_.reset( physicsMassDesc );
		desc.parentName_ = parent;

		// TODO set the other mass params!
		// parent name, etc.
		sigMass_( desc );
		// TODO needed?? massDescriptions_[ name ] = desc;
	    }
	}
    }

    //------------------------------------------------------
    void XODEParser::readMassAdjust( const NodeSharedPtr pMAdjustNode, const String& parent )
    {
	real total = StringUtil::parseReal( pMAdjustNode->getAttributeValueAs<String>( "total" ) );

	YAKE_LOG_INFORMATION( "XODE: read mass adjust - " + pMAdjustNode->getAttributeValueAs<String>( "total" ) );

	MassDesc desc;
	desc.parentName_ = parent;
	desc.mass_.reset( new IBody::MassDesc( total, math::Vector3::kZero, IBody::QT_MASS ) );

	sigMass_( desc );
    }

    //------------------------------------------------------
    void XODEParser::readMass( const NodeSharedPtr pMassNode, const String& parent, const Transform& rParentTransform )
    {
	const dom::NodeList& nodes = pMassNode->getNodes();

	// Seek transform
	Transform massTransform;

	NodeHunter hunter( "transform" );
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
		StringUtil::toLowerCase( (*it)->getName() );

	    if ( nodeName == "mass_shape" )
	    {
		readMassShape( *it, parent, massTransform );
	    }
	    else if ( nodeName == "adjust" )
	    {
		readMassAdjust( *it, parent );
	    }
	    else if ( nodeName == "mass" )
	    {
		readMass( *it, parent, massTransform );
	    }
	}

    }

    //------------------------------------------------------
    // This method is called on any unknown extension
    void XODEParser::readExtension( const NodeSharedPtr, 
	    const String& parent, 
	    const Transform& parentTransformIfAny )
    {
	// this virtual method should be overriden by any parser that wants to parse xode extensions
    }

    //------------------------------------------------------
    void XODEParser::readBody( const NodeSharedPtr pBodyNode, const String& parent )
    {
	const String name = pBodyNode->getAttributeValueAs<String>( "name" );
	YAKE_LOG( "XODE: reading body " + name );

	// Setting up body description...
	BodyDesc desc;
	desc.name_ = name;
	desc.parentName_ = parent;

	const dom::NodeList& nodes = pBodyNode->getNodes();

	// Seek transform
	NodeHunter hunter( "transform" );
	NodeListIter iTrans =
	    std::find_if<NodeListIter,NodeHunter>( nodes.begin(), nodes.end(), hunter );

	Transform bodyTransform;
	if ( iTrans != nodes.end() )
	{
	    YAKE_LOG( "XODE: body " + name + " has transform " );

	    readTransform( *iTrans, bodyTransform );

	    if ( !bodyTransform.isIdentity() )
	    {
		desc.transform_ = bodyTransform;
	    }
	}

	// fire signal!
	sigBody_( desc );
	// TODO add to collection?

	for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
	    String nodeName = StringUtil::toLowerCase( (*it)->getName() );

	    YAKE_LOG( "XODE: body " + name + " has node #" + nodeName );

	    if ( nodeName == "geom" )
	    {
		readGeom( *it, name, bodyTransform );
	    }
	    else if ( nodeName == "mass" )
	    {
		readMass( *it, name, bodyTransform );
	    }
	    else if ( nodeName == "joint" )
	    {
		readJoint( *it, name, bodyTransform );
	    }
	    else if ( nodeName == "ext" )
	    {
		readExtension( *it, name, bodyTransform );
	    }
	}
    }

    //------------------------------------------------------
    /**
     *	 Searches for <ext name="material"> node, parses it, creates material and returns ptr to created material
     * If no node, returns NULL
     * @param pNode - node to search
     */
    void XODEParser::readMaterialExt( const NodeSharedPtr pNode, const String& parent )
    {
	YAKE_LOG( "XODE: entered readMaterialExt..." );

	const dom::NodeList& nodes = pNode->getNodes();

	// Looking for ext
	// TODO maybe we need some advanced searching here?
	// like finding all ext nodes or finding <ext name="material">?
	// For now finding ext would be enough
	NodeHunter hunter( "ext" );
	NodeListIter iExt =
	    std::find_if<NodeListIter,NodeHunter>( nodes.begin(), nodes.end(), hunter );

	if ( iExt == nodes.end() )
	{
	    YAKE_LOG( "XODE: didn't find extension... " );
	    return;
	}

	YAKE_LOG( "XODE: found extension... processing... :) " );

	if ( (*iExt)->getAttributeValueAs<String>( "name" ) != "material" )
	{
	    YAKE_LOG( "XODE: extension is not material" );
	}

	YAKE_LOG( "XODE: the extension IS material! Parsing..." );

	const dom::NodeList& extNodes = (*iExt)->getNodes();
	//HACK assuming first node is "params"
	NodeSharedPtr pParamsNode = *extNodes.begin();

	// parsing
	IMaterial::Desc desc;

	String strFriction = pParamsNode->getAttributeValueAs<String>( "friction" );
	String strRestitution = pParamsNode->getAttributeValueAs<String>( "bouncecoeff" );
	String strSoftness = pParamsNode->getAttributeValueAs<String>( "soften" );

	YAKE_LOG( "XODE: read friction = " + strFriction );
	YAKE_LOG( "XODE: read restitution = " + strRestitution );
	YAKE_LOG( "XODE: read softness = " + strSoftness );

	desc.mFriction = StringUtil::parseReal( strFriction );
	desc.mRestitution = StringUtil::parseReal( strRestitution );
	desc.mSoftness = StringUtil::parseReal( strSoftness );

	//TODO fill in all other description params!
	// parent name, etc
	MaterialDesc matDesc;
	matDesc.name_ = parent + "material"; // TODO make something totally yake::Unique here?
	matDesc.material_ = desc;
	matDesc.parentName_ = parent;

	sigMaterial_( matDesc );
    }

    //------------------------------------------------------
    /**
     * Reads geom xode node
     * No folded bodies, geoms, spaces processed.
     * @param pGeomNode
     * @param pParentObject
     */
    void XODEParser::readGeom(	const NodeSharedPtr pGeomNode, const String& parent, 
	    const Transform& rParentTransform, bool absoluteTransform )
    {
	const String name = pGeomNode->getAttributeValueAs<String>( "name" );
	YAKE_LOG( "XODE: reading geom " + name );

	const dom::NodeList& nodes = pGeomNode->getNodes();

	// Looking for transform
	NodeHunter hunter( "transform" );
	NodeListIter iTrans =
	    std::find_if<NodeListIter,NodeHunter>( nodes.begin(), nodes.end(), hunter );

	Transform geomTransform;

	if ( iTrans != nodes.end() )
	{
	    YAKE_LOG( "XODE: readGeom() [" + name + "] : geom has transform " );

	    readTransform( *iTrans, geomTransform );
	}

	// if transform is absolute, we need to provide derived transform
	if ( absoluteTransform )
	    geomTransform = geomTransform.getDerivedTransform( rParentTransform );

	YAKE_LOG( "XODE: scanning other geom properties..." );

	for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
	    String nodeName = StringUtil::toLowerCase( (*it)->getName() );

	    YAKE_LOG( "XODE: geom [" + name + "] has node #" + nodeName );

	    IShape::Desc* shapeDesc = NULL;

	    if ( nodeName == "geom" )
	    {
		// reading folded geom -> absolute transform is provided
		// parent ( actor ) remains the same
		readGeom( *it, parent, geomTransform, true );
	    }
	    if ( nodeName == "box" )
	    {
		real sizeX, sizeY, sizeZ;
		readBox( *it, sizeX, sizeY, sizeZ );

		sizeX *= rParentTransform.scale_.x;
		sizeY *= rParentTransform.scale_.y;
		sizeZ *= rParentTransform.scale_.z;

		String strSizeX; strSizeX << sizeX;
		String strSizeY; strSizeY << sizeY;
		String strSizeZ; strSizeZ << sizeZ;

		YAKE_LOG( "XODE: box with sizes: " + strSizeX + ", " + strSizeY + ", " + strSizeZ );

		shapeDesc = new IShape::BoxDesc( math::Vector3( sizeX, sizeY, sizeZ ), "",
			geomTransform.position_, geomTransform.rotation_ );
	    }
	    else if ( nodeName == "cappedCylinder" )
	    {
		real radius, length;
		readCappedCylinder( *it, radius, length );

		real scale = ( rParentTransform.scale_.x
			+ rParentTransform.scale_.y
			+ rParentTransform.scale_.z ) /3;

		radius *= scale;
		length *= scale;

		shapeDesc = new IShape::CapsuleDesc( length,
			radius,
			"",
			geomTransform.position_,
			geomTransform.rotation_ );
	    }
	    else if ( nodeName == "cone" )
	    {
		real radius, length;
		readCone( *it, radius, length );

		real scale = ( rParentTransform.scale_.x
			+ rParentTransform.scale_.y
			+ rParentTransform.scale_.z ) /3;

		radius *= scale;
		length *= scale;

		YAKE_ASSERT( false ).error( "NOT implemented in Yake" ); //TODO
	    }
	    else if ( nodeName == "cylinder" )
	    {
		real radius, length;
		readCylinder( *it, radius, length );

		real scale = ( rParentTransform.scale_.x
			+ rParentTransform.scale_.y
			+ rParentTransform.scale_.z ) /3;

		radius *= scale;
		length *= scale;

		shapeDesc = new IShape::CapsuleDesc( length,
			radius,
			"",
			geomTransform.position_,
			geomTransform.rotation_ );
	    }
	    else if ( nodeName == "plane" )
	    {
		real a, b, c, d;
		readPlane( *it, a, b, c, d );
		shapeDesc = new IShape::PlaneDesc( math::Vector3( a, b, c ),
			d,
			"",
			geomTransform.position_,
			geomTransform.rotation_ );
	    }
	    else if ( nodeName == "ray" )
	    {
		real length;
		readRay( *it, length );

		real scale = ( rParentTransform.scale_.x
			+ rParentTransform.scale_.y
			+ rParentTransform.scale_.z ) /3;

		length *= scale;

		YAKE_ASSERT( false ).error( "NOT implemented in Yake" ); //TODO
	    }
	    else if ( nodeName == "sphere" )
	    {
		real radius;
		readSphere( *it, radius );

		real scale = ( rParentTransform.scale_.x
			+ rParentTransform.scale_.y
			+ rParentTransform.scale_.z ) /3;

		radius *= scale;

		shapeDesc = new IShape::SphereDesc(
			radius,
			"",
			geomTransform.position_,
			geomTransform.rotation_ );
	    }
	    else if ( nodeName == "trimesh" )
	    {
		TriangleMeshDesc::VertexVector vertices;
		TriangleMeshDesc::IndexVector indices;

		readTrimesh( *it, vertices, indices );

		TriangleMeshDesc::VertexVector scaledVertices;

		ConstVectorIterator< TriangleMeshDesc::VertexVector > iV( vertices.begin(), vertices.end() );

		while ( iV.hasMoreElements() )
		{
		    const math::Vector3& vertex = iV.getNext();
		    scaledVertices.push_back( vertex * rParentTransform.scale_ );
		}

		TriangleMeshDesc dataDesc( vertices, indices );

		shapeDesc = new IShape::TriMeshDesc( dataDesc, "",
			geomTransform.position_, geomTransform.rotation_ );
	    }
	    else if ( nodeName == "ext" )
	    {
		readExtension( *it, name, rParentTransform );
	    }

	    if ( shapeDesc != NULL )
	    {
		//TODO fill in all other description params!
		// parent name, etc
		GeomDesc geomDesc;
		geomDesc.name_ = name;
		geomDesc.parentName_ = parent;
		geomDesc.shape_.reset( shapeDesc );

		sigGeom_( geomDesc );

		// read material after geom definition was found.
		// TODO is this a right place to read material?
		// 
		readMaterialExt( *it, name );
	    }
	}

	YAKE_LOG( "XODE: Leaving readGeom()" );
    }

    //------------------------------------------------------
    void XODEParser::readBox( const NodeSharedPtr pNode, real& sizeX, real& sizeY, real& sizeZ )
    {
	sizeX = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "sizex" ) );
	sizeY = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "sizey" ) );
	sizeZ = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "sizez" ) );
    }

    //------------------------------------------------------
    void XODEParser::readCappedCylinder( const NodeSharedPtr pNode, real& radius, real& length )
    {
	radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
	length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
    }

    //------------------------------------------------------
    void XODEParser::readCone( const NodeSharedPtr pNode, real& radius, real& length )
    {
	radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
	length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
    }

    //------------------------------------------------------
    void XODEParser::readCylinder( const NodeSharedPtr pNode, real& radius, real& length )
    {
	radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
	length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
    }

    //------------------------------------------------------
    void XODEParser::readPlane( const NodeSharedPtr pNode, real& a, real& b, real& c, real& d )
    {
	a = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "a" ) );
	b = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "b" ) );
	c = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "c" ) );
	d = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "d" ) );
    }

    //------------------------------------------------------
    void XODEParser::readRay( const NodeSharedPtr pNode, real& length )
    {
	length = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "length" ) );
    }

    //------------------------------------------------------
    void XODEParser::readSphere( const NodeSharedPtr pNode, real& radius )
    {
	radius = StringUtil::parseReal( pNode->getAttributeValueAs<String>( "radius" ) );
    }

    //------------------------------------------------------
    void XODEParser::readTrimesh( const NodeSharedPtr pMeshNode,
	    TriangleMeshDesc::VertexVector& rVertices,
	    TriangleMeshDesc::IndexVector& rIndices )
    {
	const dom::NodeList& nodes = pMeshNode->getNodes();

	for ( NodeListIter it = nodes.begin(); it != nodes.end(); ++it )
	{
	    String nodeName = StringUtil::toLowerCase( (*it)->getName() );

	    YAKE_LOG( "XODE: reading " + nodeName + " trimesh now..." );

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
    void XODEParser::readVertices( const NodeSharedPtr pNode, TriangleMeshDesc::VertexVector& rVertices )
    {
	const dom::NodeList& nodes = pNode->getNodes();

	for ( NodeListIter it = nodes.begin(); it != nodes.end(); ++it )
	{
	    String nodeName = StringUtil::toLowerCase( (*it)->getName() );

	    if ( nodeName == "v" )
	    {
		real x = StringUtil::parseReal( (*it)->getAttributeValueAs< String >( "x" ) );
		real y = StringUtil::parseReal( (*it)->getAttributeValueAs< String >( "y" ) );
		real z = StringUtil::parseReal( (*it)->getAttributeValueAs< String >( "z" ) );

		rVertices.push_back( math::Vector3( x, y, z ) );
	    }
	}
    }

    //------------------------------------------------------
    void XODEParser::readIndices( const NodeSharedPtr pNode, TriangleMeshDesc::IndexVector& rIndices )
    {
	const dom::NodeList& nodes = pNode->getNodes();

	typedef int32 IndexT;

	for ( NodeListIter it = nodes.begin(); it != nodes.end(); ++it )
	{
	    String nodeName =
		StringUtil::toLowerCase( (*it)->getName() );

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
    void XODEParser::readJoint( const NodeSharedPtr pJointNode,
	    const String& parent,
	    const Transform& rParentTransform )
    {
	JointDesc desc;
	desc.parentName_ = parent;
	desc.transform_ = rParentTransform;

	// reading joint... creating description.

	const dom::NodeList& nodes = pJointNode->getNodes();
	const String name = pJointNode->getAttributeValueAs<String>( "name" );

	desc.name_ = name;

	YAKE_LOG( "XODE: reading joint '" + name + "' ..." );

	// Looking for link1
	NodeHunter link1Hunter( "link1" );
	NodeListIter iLink1 =
	    std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), link1Hunter );

	String body1Name;
	// <link1 .. can be omitted. Parent is then considered 
	// to be the first body.
	// See .xode description for explanation.
	if ( iLink1 == nodes.end() )
	    body1Name = parent;
	else
	    body1Name = (*iLink1)->getAttributeValueAs<String>( "body" );

	desc.body1_ = body1Name;

	// Looking for link2
	NodeHunter linkHunter( "link2" );
	NodeListIter iLink =
	    std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), linkHunter );

	YAKE_ASSERT( iLink != nodes.end() ).error( "2nd body name is not present in joint definition!" );

	String body2Name = (*iLink)->getAttributeValueAs<String>( "body" );

	desc.body2_ = body2Name;

	YAKE_LOG( "XODE: the joint connects two actors:" );
	YAKE_LOG( "XODE: 1:" + body1Name );
	YAKE_LOG( "XODE: 2:" + body2Name );

	// Looking for transform
	NodeHunter hunter( "transform" );
	NodeListIter iTrans =
	    std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), hunter );

	Transform jointTransform;
	if ( iTrans != nodes.end() )
	{
	    YAKE_LOG( "XODE: joint has transform " );

	    readTransform( *iTrans, jointTransform );
	}

	if ( !rParentTransform.isIdentity() )
	{
	    jointTransform = jointTransform.getDerivedTransform( rParentTransform );
	}

	for ( NodeListIter it = nodes.begin(); it != nodes.end(); ++it )
	{
	    String nodeName = StringUtil::toLowerCase( (*it)->getName() );

	    IJoint::DescBase* jointDesc = NULL;

	    YAKE_LOG( "XODE: reading " + nodeName + " now..." );

	    if ( nodeName == "ball" )
	    {
		jointDesc = readBall( *it, jointTransform );
	    }
	    else if ( nodeName == "fixed" )
	    {
		jointDesc = readFixed( *it, jointTransform );
	    }
	    else if ( nodeName == "hinge" )
	    {
		jointDesc = readHinge( *it, jointTransform );
	    }
	    else if ( nodeName == "hinge2" )
	    {
		jointDesc = readHinge2( *it, jointTransform );
	    }
	    else if ( nodeName == "slider" )
	    {
		jointDesc = readSlider( *it, jointTransform );
	    }
	    else if ( nodeName == "universal" )
	    {
		jointDesc = readUniversal( *it, jointTransform );
	    }
	    else if ( nodeName == "amotor" )
	    {
		YAKE_ASSERT( false ).error( "amotor joint type is not supported!" );
	    }

	    if ( jointDesc != NULL )
	    {
		desc.joint_.reset( jointDesc );

		sigJoint_( desc );
	    }
	}

    }

    //------------------------------------------------------
    IJoint::DescBase* XODEParser::readBall( const NodeSharedPtr pJointNode, const Transform& rJointTransform )
    {
	YAKE_LOG( "XODE: reading ball joint description..." );

	const dom::NodeList& nodes = pJointNode->getNodes();

	typedef dom::NodeList::const_iterator NodeListIter;

	// Looking for anchor
	NodeHunter anchorHunter( "anchor" );
	NodeListIter iAnchor =
	    std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), anchorHunter );

	YAKE_ASSERT( iAnchor != nodes.end() ).error( "Anchor was not defined for ball joint!" );

	math::Vector3 anchor;

	readAnchor( *iAnchor, anchor, rJointTransform );

	IJoint::DescBase* desc = new IJoint::DescBall( NULL, NULL, anchor );

	YAKE_LOG( "XODE: Finished processing ball joint description." );

	return desc;
    }

    //------------------------------------------------------
    IJoint::DescBase* XODEParser::readFixed( const NodeSharedPtr pJointNode, const Transform& rJointTransform )
    {
	YAKE_LOG( "Reading fixed joint description..." );

	IJoint::DescBase* desc = new IJoint::DescFixed( NULL, NULL );

	YAKE_LOG( "Finished processing fixed joint description." );

	return desc;
    }

    //------------------------------------------------------
    IJoint::DescBase* XODEParser::readHinge( const NodeSharedPtr pJointNode, const Transform& rJointTransform )
    {
	YAKE_LOG( "Reading hinge joint description..." );

	const dom::NodeList& nodes = pJointNode->getNodes();

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

	math::Vector3 anchor;
	math::Vector3 axis;

	readAnchor( *iAnchor, anchor, rJointTransform );
	readAxis( *iAxis, axis, rJointTransform );

	IJoint::DescBase* desc = new IJoint::DescHinge( NULL, NULL, axis, anchor );

	YAKE_LOG( "Finished processing hinge joint description." );

	return desc;
    }

    //------------------------------------------------------
    IJoint::DescBase* XODEParser::readHinge2( const NodeSharedPtr pJointNode, const Transform& rJointTransform )
    {
	YAKE_LOG( "Reading hinge2 joint description..." );

	const dom::NodeList& nodes = pJointNode->getNodes();

	// Looking for anchor
	NodeHunter anchorHunter( "anchor" );
	NodeListIter iAnchor =
	    std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), anchorHunter );

	YAKE_ASSERT( iAnchor != nodes.end() ).error( "Anchor was not defined for hinge2 joint!" );

	// Looking for axis
	NodeHunter axisHunter( "axis" );
	NodeListIter iAxis0 =
	    std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), axisHunter );

	YAKE_ASSERT( iAxis0 != nodes.end() ).error( "Axis0 was not defined for hinge2 joint!" );

	NodeListIter iAxis1 =
	    std::find_if<NodeListIter, NodeHunter>( iAxis0 + 1, nodes.end(), axisHunter );

	YAKE_ASSERT( iAxis1 != nodes.end() ).error( "Axis1 was not defined for hinge2 joint!" );

	math::Vector3 anchor;
	math::Vector3 axis0;
	math::Vector3 axis1;

	readAnchor( *iAnchor, anchor, rJointTransform );
	readAxis( *iAxis0, axis0, rJointTransform );
	readAxis( *iAxis1, axis1, rJointTransform );

	IJoint::DescBase* desc = new IJoint::DescHinge2( NULL, NULL, axis0, axis1, anchor );

	YAKE_LOG( "XODE: finished processing hinge2 joint description." );

	return desc;
    }

    //------------------------------------------------------
    IJoint::DescBase* XODEParser::readSlider( const NodeSharedPtr pJointNode, const Transform& rJointTransform )
    {
	YAKE_LOG( "XODE: reading slider joint description..." );

	const dom::NodeList& nodes = pJointNode->getNodes();

	// Looking for axis
	NodeHunter axisHunter( "axis" );
	NodeListIter iAxis =
	    std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), axisHunter );

	YAKE_ASSERT( iAxis != nodes.end() ).error( "Axis was not defined for slider joint!" );

	math::Vector3 axis;

	readAxis( *iAxis, axis, rJointTransform );

	IJoint::DescBase* desc = new IJoint::DescSlider( NULL, NULL, axis );

	YAKE_LOG( "XODE: finished processing slider joint description." );

	return desc;
    }

    //------------------------------------------------------
    IJoint::DescBase* XODEParser::readUniversal( const NodeSharedPtr pJointNode, const Transform& rJointTransform )
    {
	YAKE_LOG( "XODE: reading universal joint description..." );

	const dom::NodeList& nodes = pJointNode->getNodes();

	// Looking for anchor
	NodeHunter anchorHunter( "anchor" );
	NodeListIter iAnchor =
	    std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), anchorHunter );

	YAKE_ASSERT( iAnchor != nodes.end() ).error( "Anchor was not defined for universal joint!" );

	// Looking for axis
	NodeHunter axisHunter( "axis" );
	NodeListIter iAxis0 =
	    std::find_if<NodeListIter, NodeHunter>( nodes.begin(), nodes.end(), axisHunter );

	YAKE_ASSERT( iAxis0 != nodes.end() ).error( "Axis0 was not defined for universal joint!" );

	NodeListIter iAxis1 =
	    std::find_if<NodeListIter, NodeHunter>( iAxis0 + 1, nodes.end(), axisHunter );

	YAKE_ASSERT( iAxis1 != nodes.end() ).error( "Axis1 was not defined for universal joint!" );

	math::Vector3 anchor;
	math::Vector3 axis0;
	math::Vector3 axis1;

	readAnchor( *iAnchor, anchor, rJointTransform );
	readAxis( *iAxis0, axis0, rJointTransform );
	readAxis( *iAxis1, axis1, rJointTransform );

	IJoint::DescBase* desc = new IJoint::DescUniversal( NULL, NULL, axis0, axis1, anchor );

	YAKE_LOG( "XODE: finished processing universal joint description." );

	return desc;
    }

    //------------------------------------------------------
    void XODEParser::readAnchor( const NodeSharedPtr pAnchorNode,
	    math::Vector3& rAnchor,
	    const Transform& rJointTransform )
    {
	YAKE_LOG( "XODE: reading anchor..." );

	rAnchor.x = StringUtil::parseReal( pAnchorNode->getAttributeValueAs<String>( "x" ) );
	rAnchor.y = StringUtil::parseReal( pAnchorNode->getAttributeValueAs<String>( "y" ) );
	rAnchor.z = StringUtil::parseReal( pAnchorNode->getAttributeValueAs<String>( "z" ) );

	rAnchor = rJointTransform.position_ + rJointTransform.rotation_*( rJointTransform.scale_*rAnchor );
    }

    //------------------------------------------------------
    void XODEParser::readAxis(	const NodeSharedPtr pAxisNode,
	    math::Vector3& rAxis,
	    const Transform& rJointTransform )
    {
	YAKE_LOG( "XODE: reading axis..." );

	rAxis.x = StringUtil::parseReal( pAxisNode->getAttributeValueAs<String>( "x" ) );
	rAxis.y = StringUtil::parseReal( pAxisNode->getAttributeValueAs<String>( "y" ) );
	rAxis.z = StringUtil::parseReal( pAxisNode->getAttributeValueAs<String>( "z" ) );

	/// TODO What about all other axis params?

	rAxis = rJointTransform.rotation_*rAxis;
    }

} //xode
} //parser
} //data
} //yake

