// STL
#include <iostream>
// Yake
#include <yake/base/yake.h>
#include <yake/base/yakeDebugOutputStream.h>
#include <yake/base/yakeLog.h>
#include <yake/data/yakeData.h>

using namespace yake::base;
using namespace yake::base::templates;
using namespace yake::data;
using namespace std;

/**
 * Produces indentation on standard output (using cout)
 * Just to make it a bit nicer
 * @param numChars 
 * @param character // indentation character
 */
void indent( unsigned numChars, char character = '\t' )
{
	for ( unsigned indent = 0; indent < numChars; ++indent )
		cout << character;
}

/**
 * Print out XML node to standard output with indentation according to node level
 * @param pNode 
 * @param level 
 */
void printNode( SharedPtr<dom::INode> pNode, unsigned level = 0)
{
	const dom::NodeList& rNodes = pNode->getNodes();
	for ( dom::NodeList::const_iterator it = rNodes.begin(); it != rNodes.end(); ++it )
	{
		SharedPtr<dom::INode> pCurrentNode = *it;
		dom::INode::ValueType value = pCurrentNode->getValue( "name" );
		
		indent( level );
		cout << "XML node: " << varGet<String>( value ) << endl;
		
		// get attribute names
		indent( level );
		cout << "  > attributes: ";
		
		const dom::INode::AttributeMap& rAttributes = pCurrentNode->getAttributes();
		for ( dom::INode::AttributeMap::const_iterator it = rAttributes.begin();
			it != rAttributes.end();
			++it )
		{
			cout << " " << (*it).first << " ";
		}
		
		cout << endl;
		
		printNode( pCurrentNode, level + 1 );
	}
}

int main()
{
	try
	{
		// load a XML file into a DOM tree
		SharedPtr<dom::ISerializer> ser( new dom::xml::XmlSerializer() );
		ser->parse("../../media/gui0.xml", true);

		// get some data from the DOM tree
		SharedPtr<dom::INode> pNode = ser->getDocumentNode();

		dom::INode::ValueType value = pNode->getValue( "name" );
		cout << "root node: " << varGet<String>( value ) << endl;
		
		printNode( pNode );
	}
	
	catch ( yake::base::Exception const& e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif
	return 0;
}
