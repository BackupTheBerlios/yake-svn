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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#include <yake/data/yakePCH.h>
#include <yake/data/yakeStreamSerializer.h>
#include <yake/data/yakeXMLSerializer.h>
#include <dependencies/tinyxml/tinyxml.h>

namespace yake {
namespace data {
namespace dom {
namespace xml {

	XmlNode::XmlNode( TiXmlElement* pElem ) : mElem( pElem )
	{
		parse( pElem );
	}
	XmlNode::~XmlNode()
	{
		mNodes.clear();
	}
	const NodeList& XmlNode::getNodes() const
	{
		return mNodes;
	}
	const INode::AttributeMap& XmlNode::getAttributes() const
	{
		return mAttributes;
	}
	void XmlNode::addNode( SharedPtr<INode> pNode )
	{
		if (pNode)
			mNodes.push_back( pNode );
	}
	INode::ValueType XmlNode::getValue( const String & id ) const
	{
		if (id == "name")
			return mValue;
		else
			return 0;
	}
	INode::ValueType XmlNode::getAttributeValue( const String & name ) const
	{
		// iterate over node's attributes
		const char* val = mElem->Attribute( name.c_str() );
		if (!val)
			return String("");
		return String( val );
	}
	void XmlNode::setAttributeValue( const String & name, const ValueType & value )
	{
		YAKE_ASSERT( 1==0 );
	}
	void XmlNode::setValue( const String & name, const ValueType & value )
	{
		YAKE_ASSERT( 1==0 );
	}
	SharedPtr<INode> XmlNode::getNodeByName( const String & name ) const
	{
		for (NodeList::const_iterator it = mNodes.begin(); it != mNodes.end(); ++it)
		{
			if ( varGet<String>((*it)->getValue("name")) == name )
				return (*it);
		}
		return SharedPtr<INode>();
	}
	SharedPtr<INode> XmlNode::getNodeById( const String & id ) const
	{
		return SharedPtr<INode>();
	}
	void XmlNode::parse( TiXmlElement* pElem )
	{
		YAKE_ASSERT( pElem );
		mValue = String(pElem->Value());
		for (TiXmlAttribute* pAttr = mElem->FirstAttribute(); 
			pAttr != 0; 
			pAttr = pAttr->Next())
		{
			mAttributes.insert( std::make_pair( pAttr->Name(), String(pAttr->Value()) ) );
		}
	}

	XmlSerializer::XmlSerializer() : mXmlDoc(0), mRootNode(), mRootElem(0)
	{
	}
	XmlSerializer::~XmlSerializer()
	{
		reset();
	}
	SharedPtr<INode> XmlSerializer::getDocumentNode() const
	{
		return mRootNode;
	}
	void XmlSerializer::parse( const String & file, bool bFireSignals )
	{
		if (mXmlDoc)
			reset();

		mFireSignals = bFireSignals;

		mXmlDoc = new TiXmlDocument( file.c_str() );
		mXmlDoc->LoadFile();

		// read contents
		mRootElem = mXmlDoc->RootElement();
		mRootNode.reset( new XmlNode( mRootElem ) );

		// recursively traverse xml tree and fire events as needed
		parseNode( mRootNode );
	}
	void XmlSerializer::reset()
	{
		YAKE_SAFE_DELETE( mXmlDoc );
		mRootElem = 0;
		mRootNode.reset();
	}
	void XmlSerializer::parseNode( SharedPtr<XmlNode> pNode )
	{
		YAKE_ASSERT( pNode );

		TiXmlElement* pNodeElem = pNode->getXmlElem();
		YAKE_ASSERT( pNodeElem );

		// we only need to iterate over the attributes when firing signals,
		// there's no need for it when using the 'interrogating' approach
		// (no pre-processing need).
		if (mFireSignals)
		{
			// fire "new node" signal
			mNewNodeSignal( pNode );

			// iterate over node's attributes & fire signals
			for (TiXmlAttribute* pAttr = pNodeElem->FirstAttribute(); 
				pAttr != 0; 
				pAttr = pAttr->Next())
			{
				// fire "new attribute"
				mNewAttributeSignal( pAttr->Name() );
			}
		}

		// iterate over child xml elements, creating new nodes as we go
		// and recursively parsing them.
		for (TiXmlElement* pElem = pNodeElem->FirstChildElement();
			pElem != 0; pElem = pElem->NextSiblingElement() )
		{
			SharedPtr<XmlNode> pNewNode( new XmlNode( pElem ) );
			parseNode( pNewNode );
			pNode->addNode( pNewNode );
		}
	}


} // xml
} // parser
} // data
} // yake