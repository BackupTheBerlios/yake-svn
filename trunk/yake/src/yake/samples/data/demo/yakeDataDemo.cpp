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

int main()
{
	try
	{
		// load a XML file into a DOM tree
		SharedPtr<dom::ISerializer> ser( new dom::xml::XmlSerializer() );
		ser->parse("../../media/gui0.xml", true);

		// get some data from the DOM tree
		SharedPtr<dom::INode> node = ser->getDocumentNode();
		cout << "root node: " << varGet<String>(node->getValue("name")) << endl;
		const dom::NodeList & nodes = node->getNodes();
		for (dom::NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			cout << varGet<String>((*it)->getValue("name")) << endl;
			// get attribute names
			cout << "  > attributes: ";
			const dom::INode::AttributeMap& attributes = (*it)->getAttributes();
			for (dom::INode::AttributeMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
			{
				cout << " " << (*it).first << " ";
			}
			cout << endl;
		}
	}
	catch (const yake::base::Exception & e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif
	return 0;
}