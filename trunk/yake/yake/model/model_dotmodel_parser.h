/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#ifndef YAKE_MODEL_DOTMODELPARSER_H
#define YAKE_MODEL_DOTMODELPARSER_H

#include <yake/model/prerequisites.h>
#include <yake/data/yakeData.h>
#include <yake/model/model_component.h>

namespace yake {
namespace data {
namespace parser {
namespace dotmodel {

	/** DotModelParser base class providing default implementation.
	*/
	class YAKE_MODEL_API DotModelParser
	{
	public:
		typedef SharedPtr<dom::INode> NodeSharedPtr;
		typedef dom::NodeList::const_iterator NodeListIter;

		
		DotModelParser();
		virtual ~DotModelParser() {}

		String getName() const
		{ 
			return "yake.data.dotModel";
		}

		virtual Version getVersion() const = 0;

		/** Build physics model from DOM!
			\param file 
			\return 
		*/
		virtual bool load( const NodeSharedPtr docNode );

		/** Prepare for next run of load/store.
		*/
		virtual void reset();

		typedef model::ModelComponent::Desc ModelComponentDesc;
		struct ModelDesc
		{
			String	name_;
		};

		typedef SignalX< void( const ModelDesc& ) > ModelSignal;
		typedef ModelSignal ModelFinishedSignal;
		typedef SignalX< void( const ModelComponentDesc& ) > ModelComponentSignal;
		typedef SignalX< void ( void ) > EventSignal; // special event handler type
	
		SignalConnection subscribeToModelSignal( const ModelSignal::slot_type& slot )
		{ return sigModel_.connect(slot); } 
		SignalConnection subscribeToModelComponentSignal( const ModelComponentSignal::slot_type& slot )
		{ return sigModelComponent_.connect(slot); } 
		SignalConnection subscribeToModelFinishedSignal( const ModelFinishedSignal::slot_type& slot )
		{ return sigModelFinished_.connect(slot); } 

		SignalConnection subscribeToParseEndedSignal( const EventSignal::slot_type& slot )
		{ return sigParseEnded_.connect( slot ); } 

	protected:
		ModelSignal				sigModel_;
		ModelFinishedSignal		sigModelFinished_;
		ModelComponentSignal	sigModelComponent_;
		EventSignal				sigParseEnded_;
	
		/// Default implementations for common functions.
		virtual void readDotModel( const NodeSharedPtr pNode );
		virtual void readModels( const NodeSharedPtr pModelsNode );
		virtual void readModel( const NodeSharedPtr pModelNode );
		virtual void readModelComponent( const NodeSharedPtr pCompNode, ModelDesc& modelDesc );
		virtual void readModelComponentParameter( const NodeSharedPtr pParamNode, String& name, String& value );

	private:
		NodeSharedPtr		mDocNode;
	};
	
	class YAKE_MODEL_API DotModelParserV1 : public DotModelParser
	{
	public:
		virtual ~DotModelParserV1() {}
		
		virtual Version getVersion() const
		{ 
			return Version( 1, 0, 0 );
		}
	};

} // xode
} // parser
} // data
} // yake

#endif

