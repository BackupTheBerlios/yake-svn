#pragma once

#include <inc/data/yakeDotScene.h>

namespace yake {
	using namespace base;
	using namespace templates;
namespace data {
namespace parser {
namespace dotscene {

	/** Implementation v1 for the .scene format.
	*/
	class DotSceneParser_v1 : public IDotSceneParser
	{
	private:
		SharedPtr<graphics::IGraphicalWorld>	mGWorld;
	public:
		virtual Version getVersion() const;

		virtual bool parse( SharedPtr<data::dom::INode> pRootNode, graphics::IGraphicalWorld* pGWorld );
		virtual void reset();

	protected:
		void readEntity( SharedPtr<dom::INode> pNode );
		void readNode( SharedPtr<dom::INode> pNode );
		void readNodes( SharedPtr<dom::INode> pNode );
	};

} // dotscene
} // parser
} // data
} // yake
