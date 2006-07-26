/*
   ------------------------------------------------------------------------------------
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
#include <yake/data/yakeXMLSerializer.h>

#define STRICT
#define WIN32_LEAN_AND_MEAN
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

	String XmlNode::getName() const
	{
		YAKE_ASSERT( mElem );
		if (mElem)
			return String( mElem->Value() );
		return String("");
	}

	void XmlNode::setName(const String& name)
	{
		YAKE_ASSERT( mElem );
		if (mElem)
			mElem->SetValue( name.c_str() );
	}

	void XmlNode::setAttributeValue( String const& rName, ValueType const& rValue )
	{
		YAKE_ASSERT( mElem );
		if (mElem)
		{
			if (rValue.which() == VTID_STRING)
				mElem->SetAttribute( rName.c_str(), varGet<String>(rValue).c_str() );
			else
			{
				YAKE_ASSERT( 0 == 1 );
			}
		}
	}

	INode::ValueType XmlNode::getAttributeValue( String const& rName ) const
	{
		YAKE_ASSERT( mElem );
		if (!mElem)
			return NullType();
		const char* val = mElem->Attribute( rName.c_str() );
		return val ? String( val ) : String("");
	}

	SharedPtr<INode> XmlNode::getNodeByName( String const& rName ) const
	{
		for ( NodeList::const_iterator it = mNodes.begin(); it != mNodes.end(); ++it )
		{
			data::dom::INode::ValueType value = (*it)->getName();

			if ( varGet<String>( value ) == rName )
				return ( *it );
		}

		return SharedPtr<INode>();
	}

	SharedPtr<INode> XmlNode::getNodeById( String const& rId ) const
	{
		// still NYI?
		YAKE_ASSERT( 0==1 );
		return SharedPtr<INode>();
	}

	void XmlNode::parse( TiXmlElement* pElem )
	{
		YAKE_ASSERT( pElem );

		for ( TiXmlAttribute* pAttr = pElem->FirstAttribute();
			pAttr != 0;
			pAttr = pAttr->Next() )
		{
			mAttributes.insert( std::make_pair( pAttr->Name(), String( pAttr->Value() ) ) );
		}
	}

	XmlSerializer::XmlSerializer() : mRootNode(), mRootElem(0), mXmlDoc(0)
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

	void XmlSerializer::parse( String const& rFile, bool fireSignals )
	{
		if ( mXmlDoc )
			reset();

		mFireSignals = fireSignals;

		mXmlDoc = new TiXmlDocument( rFile.c_str() );
		bool ret = mXmlDoc->LoadFile();
		YAKE_ASSERT( ret && "Could not load xml file.")( rFile )( mXmlDoc->ErrorDesc() ).debug("Could not load xml file.");
		if (!ret)
		{
			reset();
			return;
		}

		// read contents
		mRootElem = mXmlDoc->RootElement();
		YAKE_ASSERT( mRootElem ).debug("No root element found in document!");
		if (!mRootElem)
		{
			reset();
			return;
		}
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
		if ( mFireSignals )
		{
			// fire "new node" signal
			mNewNodeSignal( pNode );

			// iterate over node's attributes & fire signals
			for ( TiXmlAttribute* pAttr = pNodeElem->FirstAttribute();
				pAttr != 0;
				pAttr = pAttr->Next() )
			{
				// fire "new attribute"
				mNewAttributeSignal( pAttr->Name() );
			}
		}

		// iterate over child xml elements, creating new nodes as we go
		// and recursively parsing them.
		for ( TiXmlElement* pElem = pNodeElem->FirstChildElement();
			pElem != 0;
			pElem = pElem->NextSiblingElement() )
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
