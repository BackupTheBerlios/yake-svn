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
#ifndef YAKE_DATA_DOM_XMLSERIALIZER_H
#define YAKE_DATA_DOM_XMLSERIALIZER_H

#include <yake/data/yakeDataPrerequisites.h>
#include <yake/data/yakeDataSerializer.h>

class TiXmlElement;
class TiXmlDocument;

namespace yake {
	using namespace base::templates;
	using namespace base;
namespace data {
namespace dom {
namespace xml {

	class XmlSerializer;
	class YAKE_DATA_API XmlNode : public INode
	{
		friend class XmlSerializer;
	protected:
		XmlNode( TiXmlElement* pElem );
	public:
		XmlNode();
		virtual ~XmlNode();

		// yake::data::parser::INode overrides

		virtual void addNode( SharedPtr<INode> pNode );
		virtual SharedPtr<INode> getNodeByName( String const& rName ) const;
		virtual SharedPtr<INode> getNodeById( String const& rId ) const;
		virtual const NodeList& getNodes() const;

		// node values
		virtual ValueType getValue( String const& rName ) const;
		virtual void setValue( String const& rName, ValueType const& rValue );

		virtual const AttributeMap& getAttributes() const;
		virtual ValueType getAttributeValue( String const& rName ) const;
		virtual void setAttributeValue( String const& rName, ValueType const& rValue );

	protected:
		// XmlNode specific

		TiXmlElement* getXmlElem() const
		{ 
			return mElem; 
		}
		
		void parse( TiXmlElement* pElem );

	private:
		TiXmlElement*			mElem;
		NodeList			mNodes;
		ValueType			mValue;
		INode::AttributeMap		mAttributes;
	};

	class YAKE_DATA_API XmlSerializer : public ISerializer
	{
	public:
		XmlSerializer();
		virtual ~XmlSerializer();

		// yake::data::parser::IParser overrides

		virtual String getName() const
		{ 
			return "yake.data.dom.xml.tinyxml";
		}
		
		virtual Version getVersion() const
		{ 
			return Version( 0, 1, 0 ); 
		}

		/** // \ToDo adjust to DataChunk loading, -> yake::data::parser::IParser
		*/
		virtual void parse( String const& rFile, bool fireSignals );

		virtual void reset();

		virtual SharedPtr<INode> getDocumentNode() const;

	private:
		void parseNode( SharedPtr<XmlNode> pNode );

	private:
		SharedPtr<XmlNode>	mRootNode; // only a single node for XML files, generally yake allows for more than one.
		TiXmlElement*	mRootElem;
		TiXmlDocument*	mXmlDoc;
		bool		mFireSignals;
	};

} // xml
} // dom
} // data
} // yake

#endif
