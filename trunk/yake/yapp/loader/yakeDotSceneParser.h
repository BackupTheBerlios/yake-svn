#pragma once

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

	/** Base class for .scene loaders. It provides default features and a consistent interface.
		It also provides default implementations for helper functions which can be overridden
		as necessary by derived parsers.
	*/
	class IDotSceneParser
	{
	public:
		typedef Signal1< void ( const SharedPtr<dom::INode> ) > NewNodeSignal;
		typedef Signal1< void ( const SharedPtr<dom::INode> ) > NewEntitySignal;
	protected:
		NewNodeSignal	mNewNodeSignal;
		NewEntitySignal	mNewEntitySignal;
	public:
		/** Parses the document stored in the DOM with pRootNode as the root document node.
		*/
		virtual bool parse( SharedPtr<dom::INode> pRootNode, graphics::IGraphicalWorld* pGWorld ) = 0;

		// data access

		virtual void reset() = 0;

		// versioning etc

		virtual String getName() const;
		virtual Version getVersion() const = 0;

		// signals

		virtual void subscribeToNewNodeSignal( const NewNodeSignal::slot_type & slot );
		virtual void subscribeToNewNewEntitySignalSignal( const NewEntitySignal::slot_type & slot );

	protected:
		// default implementations for helper functions... can be overridden if necessary.

		//virtual void readColour(...);

		virtual void readPosition( SharedPtr<dom::INode> pNode, Vector3 & position );
		virtual void readOrientation( SharedPtr<dom::INode> pNode, Quaternion & orientation );
	};


} // dotscene
} // parser
} // data
} // yake
