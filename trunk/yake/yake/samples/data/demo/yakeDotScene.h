

namespace yake {
namespace data {
namespace serializer {
namespace dotscene {

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
							SharedPtr<graphics::IGraphicalWorld> & pGWorld,
							graphics::ISceneNode* pParentSN);

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
		SharedPtr<graphics::IGraphicalWorld>	mGWorld;
	};

	//------------------------------------------------------
	void DotSceneSerializer::reset()
	{
		mDocNode.reset();
		mGWorld.reset();
	}

	//------------------------------------------------------
	bool DotSceneSerializer::load(
							const SharedPtr<dom::INode> & docNode,
							SharedPtr<graphics::IGraphicalWorld> & pGWorld,
							graphics::ISceneNode* pParentSN)
	{
		std::cout << "load()" << std::endl;
		YAKE_ASSERT( docNode.get() );
		if (!docNode.get())
			return false;
		YAKE_ASSERT( pGWorld.get() );
		if (!pGWorld.get())
			return false;
		YAKE_ASSERT( pParentSN );
		if (!pParentSN)
			return false;
		mDocNode = docNode;
		mGWorld = pGWorld;
		//
		std::cout << "parsing scene..." << std::endl;
		readScene( mDocNode, pParentSN );

		//
		return true;
	}

	//------------------------------------------------------
	void DotSceneSerializer::readScene( const SharedPtr<dom::INode> & pNode, graphics::ISceneNode* pParentSN )
	{
		std::cout << "readScene() [" << varGet<String>(pNode->getValue("name")) << "]" << std::endl;
		YAKE_ASSERT( pNode );
		YAKE_ASSERT( pParentSN );
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
		YAKE_ASSERT( pParentSN );
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
		YAKE_ASSERT( pParentSN );
		const dom::NodeList & nodes = pNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			String childNodeName = varGet<String>((*it)->getValue("name"));
			childNodeName.toLowerCase();
			std::cout << "node child: " <<  childNodeName << std::endl;

			const SharedPtr<dom::INode> & pChild = (*it);
			if (childNodeName == "entity")
				readEntity( pChild, pParentSN );
			else if (childNodeName == "position")
			{
				Vector3 position;
				readPosition( pChild, position );
			}
			else if (childNodeName == "orientation")
			{
				Quaternion orientation;
				readOrientation( pChild, orientation );
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
		graphics::IEntity* pEnt = mGWorld->createEntity( meshName );
		YAKE_ASSERT( pEnt );
		pEnt->setCastsShadow( true );
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
