#ifndef YAKE_DATA_DOTSCENE_H
#define YAKE_DATA_DOTSCENE_H

#include <yake/data/yakeData.h>

#pragma warning(disable: 4244)

namespace yake {
namespace data {
namespace serializer {
namespace dotscene {

	typedef ::yake::base::templates::Vector< graphics::ISceneNode* > SceneNodeList;

	/** DotSceneReader base class providing default implementation for DOM parsing.
	* \todo read lights, environment, external references ..
	*/
	class DotSceneSerializer
	{
	public:
		~DotSceneSerializer() {}

		String getName() const
		{ return "yake.data.dotScene"; }

		virtual Version getVersion() const = 0;

		/** Build graphics scene from DOM!
			\param file 
			\return 
		*/
		virtual bool load(	const SharedPtr<dom::INode> & docNode,
							graphics::IGraphicalWorld* pGWorld);

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

		virtual void readScene( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN );
		virtual void readNodes( const SharedPtr<dom::INode> & pNodes, graphics::ISceneNode* pParentSN );
		virtual void readNode( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN );
		virtual void readEntity( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN );
		virtual void readOrientation( const SharedPtr<dom::INode> & pNode, Quaternion & orientation );
		virtual void readPosition( const SharedPtr<dom::INode> & pNode, Vector3 & position );
	private:
		SharedPtr<dom::INode>					mDocNode;
		graphics::IGraphicalWorld*				mGWorld;
		SceneNodeList							mRootNodes;

		typedef templates::AssocVector<graphics::ISceneNode*, String> SceneNodeNameMap;
		SceneNodeNameMap						mNodeNames;
	};

	//------------------------------------------------------
	SceneNodeList DotSceneSerializer::getRootLevelSceneNodes() const
	{
		return mRootNodes;
	}
	//------------------------------------------------------
	void DotSceneSerializer::reset()
	{
		mDocNode.reset();
	}

	//------------------------------------------------------
	bool DotSceneSerializer::load(
							const SharedPtr<dom::INode> & docNode,
							graphics::IGraphicalWorld* pGWorld)
	{
		std::cout << "load()" << std::endl;
		YAKE_ASSERT( docNode.get() );
		if (!docNode.get())
			return false;
		YAKE_ASSERT( pGWorld );
		if (!pGWorld)
			return false;
		mDocNode = docNode;
		mGWorld = pGWorld;
		//
		std::cout << "parsing scene..." << std::endl;
		readScene( mDocNode, 0 );

		//
		return true;
	}

	//------------------------------------------------------
	void DotSceneSerializer::readScene( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN )
	{
		std::cout << "readScene() [" << varGet<String>(pNode->getValue("name")) << "]" << std::endl;
		YAKE_ASSERT( pNode );
		SharedPtr<dom::INode> pNodes = pNode->getNodeByName("nodes");
		std::cout << "scene: found nodes = " << (pNodes.get() ? "yes" : "no") << std::endl;
		if (pNodes.get())
			readNodes( pNodes, pParentSN );
	}

	//------------------------------------------------------
	void DotSceneSerializer::readNodes( const SharedPtr<dom::INode> & pNodes, graphics::ISceneNode* pParentSN )
	{
		std::cout << "readNodes()" << std::endl;
		YAKE_ASSERT( pNodes );
		const dom::NodeList & nodes = pNodes->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			readNode( (*it), pParentSN );
		}
	}

	//------------------------------------------------------
	void DotSceneSerializer::readNode( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN )
	{
		std::cout << "readNode() [name=" << varGet<String>(pNode->getAttributeValue("name")) << "]" << std::endl;
		YAKE_ASSERT( pNode );
		graphics::ISceneNode* pChildSN = mGWorld->createSceneNode();
		YAKE_ASSERT( pChildSN );
		mNodeNames.insert( std::make_pair( pChildSN, pNode->getAttributeValueAs<String>("name") ) );
		if (pParentSN)
			pParentSN->addChildNode( pChildSN );
		else
			mRootNodes.push_back( pChildSN );
		const dom::NodeList & nodes = pNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String childNodeName = varGet<String>((*it)->getValue("name"));
			childNodeName = ::yake::base::StringUtil::toLowerCase(childNodeName);
			std::cout << "node child: " <<  childNodeName << std::endl;

			const SharedPtr<dom::INode> & pChild = (*it);
			if (childNodeName == "entity")
				readEntity( pChild, pChildSN );
			else if (childNodeName == "position")
			{
				Vector3 position;
				readPosition( pChild, position );
				pChildSN->setPosition( position );
			}
			else if (childNodeName == "orientation")
			{
				Quaternion orientation;
				readOrientation( pChild, orientation );
				pChildSN->setOrientation( orientation );
			}
		}
	}

	//------------------------------------------------------
	void DotSceneSerializer::readEntity( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN )
	{
		std::cout << "readEntity() [name=" << varGet<String>(pNode->getAttributeValue("name")) << "]" << std::endl;
		YAKE_ASSERT( pNode );
		YAKE_ASSERT( pParentSN );
		String name = varGet<String>(pNode->getAttributeValue("name"));
		String meshName = varGet<String>(pNode->getAttributeValue("meshfile"));
		String castsShadow = varGet<String>(pNode->getAttributeValue("castsShadow"));
		graphics::IEntity* pEnt = mGWorld->createEntity( meshName );
		YAKE_ASSERT( pEnt );
		pEnt->setCastsShadow( (castsShadow == "yes") );
		pParentSN->attachEntity( pEnt );
	}

	//------------------------------------------------------
	void DotSceneSerializer::readPosition( const SharedPtr<dom::INode> & pNode, Vector3 & position )
	{
		YAKE_ASSERT( pNode );
		position.x = atof( pNode->getAttributeValueAs<String>("x").c_str() );
		position.y = atof( pNode->getAttributeValueAs<String>("y").c_str() );
		position.z = atof( pNode->getAttributeValueAs<String>("z").c_str() );
	}

	//------------------------------------------------------
	void DotSceneSerializer::readOrientation( const SharedPtr<dom::INode> & pNode, Quaternion & orientation )
	{
		YAKE_ASSERT( pNode );
		if (pNode->getAttributeValueAs<String>("qx") != "")
		{
			orientation.x = atof( pNode->getAttributeValueAs<String>("qx").c_str() );
			orientation.y = atof( pNode->getAttributeValueAs<String>("qy").c_str() );
			orientation.z = atof( pNode->getAttributeValueAs<String>("qz").c_str() );
			orientation.w = atof( pNode->getAttributeValueAs<String>("qw").c_str() );
		}
		if (pNode->getAttributeValueAs<String>("axisx") != "")
		{
			Vector3 axis;
			axis.x = atof( pNode->getAttributeValueAs<String>("axisx").c_str() );
			axis.y = atof( pNode->getAttributeValueAs<String>("axisy").c_str() );
			axis.z = atof( pNode->getAttributeValueAs<String>("axisz").c_str() );
			orientation.FromAxes( &axis );
		}
		if (pNode->getAttributeValueAs<String>("anglex") != "")
		{
			Vector3 axis;
			axis.x = atof( pNode->getAttributeValueAs<String>("anglex").c_str() );
			axis.y = atof( pNode->getAttributeValueAs<String>("anglex").c_str() );
			axis.z = atof( pNode->getAttributeValueAs<String>("anglex").c_str() );
			real angle = atof( pNode->getAttributeValueAs<String>("angle").c_str() );;
			orientation.FromAngleAxis( angle, axis );
		}
	}

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
