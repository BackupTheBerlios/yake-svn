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
#include <yake/model/pch.h>
#include <yake/model/model.h>
#include <yake/model/model_dotmodel_parser.h>

namespace yake {
namespace data {
namespace parser {
namespace dotmodel {

    //------------------------------------------------------
	DotModelParser::DotModelParser()
    {}

    //------------------------------------------------------
    bool DotModelParser::load( const NodeSharedPtr docNode )
    {
		YAKE_LOG( "dotModel: load()" );

		YAKE_ASSERT( docNode.get() );

		mDocNode = docNode;

		YAKE_LOG( "***********************" );
		YAKE_LOG( "* Parsing dotModel... *" );
		YAKE_LOG( "***********************" );

		readDotModel( mDocNode );

		return true;
    }

    //------------------------------------------------------
    void DotModelParser::readDotModel( const NodeSharedPtr pNode )
    {
		YAKE_ASSERT( pNode.get() );

		readModels( pNode );

		// parse completed. Firing signal!
		sigParseEnded_();
    }

    //------------------------------------------------------
    void DotModelParser::readModels( const NodeSharedPtr pModelsNode )
    {
		const dom::NodeList& nodes = pModelsNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			readModel( *it );
		}
    }

    //------------------------------------------------------
    void DotModelParser::readModel( const NodeSharedPtr pModelNode )
    {
		ModelDesc desc;
		desc.name_ = yake::StringUtil::toLowerCase( pModelNode->getAttributeValueAs<String>("name") );
		YAKE_LOG("xode: model '" << desc.name_ << "' found!");

		this->sigModel_( desc );

		const dom::NodeList& nodes = pModelNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			const String name = yake::StringUtil::toLowerCase( (*it)->getName() );
			YAKE_LOG("xode: item '" << name << "' found!");
			if (name == "component")
			{
				this->readModelComponent( *it, desc );
			}
			else
				YAKE_LOG_WARNING("xode: unhandled item '" << name << "'!");
		}

		this->sigModelFinished_( desc );
    }

    //------------------------------------------------------
	void DotModelParser::readModelComponent( const NodeSharedPtr pCompNode, ModelDesc& modelDesc )
	{
		ModelComponentDesc desc;
		desc.name = pCompNode->getAttributeValueAs<String>("name");
		desc.type = pCompNode->getAttributeValueAs<String>("type");

		const dom::NodeList& nodes = pCompNode->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			const String name = yake::StringUtil::toLowerCase( (*it)->getName() );
			YAKE_LOG("xode: item '" << name << "' found!");
			if (name == "parameter")
			{
				String name;
				String value;
				this->readModelComponentParameter( *it, name, value );
				desc.params[name] = value;
			}
			else
				YAKE_LOG_WARNING("xode: unhandled item '" << name << "'!");
		}

		desc.params["name"] = desc.name;
		desc.params["type"] = desc.type;
		this->sigModelComponent_(desc);
	}

    //------------------------------------------------------
	void DotModelParser::readModelComponentParameter( const NodeSharedPtr pParamNode, String& name, String& value )
	{
		name = pParamNode->getAttributeValueAs<String>("name");
		value = pParamNode->getAttributeValueAs<String>("value");
	}

	//------------------------------------------------------
    void DotModelParser::reset()
    {
		//TODO is this really needed?
		mDocNode.reset();
    }

} //xode
} //parser
} //data
} //yake

