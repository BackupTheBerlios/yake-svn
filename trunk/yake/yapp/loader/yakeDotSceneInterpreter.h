#pragma once

#include <inc/data/yakeDotSceneParser.h>

namespace yake {
	using namespace base;
	using namespace templates;
namespace data {
namespace parser {
namespace dotscene {

	class IDotSceneInterpreter
	{
	private:
		SharedPtr<IDotSceneParser> mParser;
	public:
		virtual void onNewNode( const Pointer<dom::INode> pDOMNode ) = 0;
		virtual void onNewEntity( const Pointer<dom::INode> pDOMNode ) = 0;
		virtual void hookToParser( SharedPtr<IDotSceneParser> pParser )
		{
			mParser = pParser;
			YAKE_ASSERT( mParser.get() );
		}
	};

} // dotscene
} // parser
} // data
} // yake
