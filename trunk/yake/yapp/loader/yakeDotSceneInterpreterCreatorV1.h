#pragma once

#include <inc/data/yakeDotSceneInterpreter.h>

namespace yake {
	using namespace base;
	using namespace templates;
	namespace graphics {
		class ISceneNode;
		class IGraphicalWorld;
	}
namespace data {
namespace parser {
namespace dotscene {

	class DotSceneInterpreterCreator_v1 : public IDotSceneInterpreter
	{
	private:
		graphics::ISceneNode*		mRootSN;
		graphics::IGraphicalWorld*	mGWorld;
	public:
		DotSceneInterpreterCreator_v1( graphics::IGraphicalWorld & graphicalWorld );
		virtual ~DotSceneInterpreterCreator_v1();

		// IDotSceneInterpreter overrides

		virtual void onNewNode( const SharedPtr<dom::INode> pDOMNode );
		virtual void onNewEntity( const SharedPtr<dom::INode> pDOMNode );

		//only if necessary: virtual void hookToParser( IDotSceneParser );

		// created data

		virtual graphics::ISceneNode* getRootSceneNode() const;
	};

} // dotscene
} // parser
} // data
} // yake
