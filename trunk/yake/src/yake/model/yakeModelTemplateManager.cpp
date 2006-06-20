/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright (c) The YAKE Team
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
#include "yake/model/pch.h"
#include "yake/model/prerequisites.h"
#include "yake/model/model.h"
#include "yake/model/model_dotmodel_parser.h"

namespace yake {
namespace model {

	namespace detail {
		struct DotModelListener
		{
			DotModelListener() : currTpl_(0)
			{
			}
			void onBeginModel(const data::parser::dotmodel::DotModelParser::ModelDesc& desc)
			{
				YAKE_ASSERT( !desc.name_.empty() );
				currTpl_ = new ModelTemplate();
				tpls_.insert( std::make_pair(desc.name_,currTpl_) );
			}
			void onModelComponent(const data::parser::dotmodel::DotModelParser::ModelComponentDesc& desc)
			{
				YAKE_ASSERT( currTpl_ );
				currTpl_->addComponentDesc( desc );
			}
			void onEndModel(const data::parser::dotmodel::DotModelParser::ModelDesc&)
			{
				currTpl_ = 0;
			}

			typedef std::map<String,ModelTemplate*> TplList;
			TplList& getTemplates()
			{ 
				return tpls_; 
			}
			void reset()
			{
				currTpl_ = 0;
				tpls_.clear();
			}
			void destroyTemplates()
			{
				ConstVectorIterator<TplList> it(tpls_);
				while (it.hasMoreElements())
					delete it.getNextRef().second;
				tpls_.clear();
			}
		private:
			TplList			tpls_;
			ModelTemplate*	currTpl_;
		};
	} // namespace detail

	bool ModelManager::loadTemplatesFromDotModel(const String& fn)
	{
		// Read dotscene file into DOM

		yake::data::dom::xml::XmlSerializer ser;
		ser.parse( fn, false );
		YAKE_ASSERT( ser.getDocumentNode() )( fn ).error("Could not parse dotModel document!");

		// Parse DOM and create objects

		data::parser::dotmodel::DotModelParserV1 parser;

		detail::DotModelListener listener;

		parser.subscribeToModelSignal( Bind1( &detail::DotModelListener::onBeginModel,&listener ) );
		parser.subscribeToModelComponentSignal( Bind1( &detail::DotModelListener::onModelComponent,&listener ) );
		parser.subscribeToModelFinishedSignal( Bind1( &detail::DotModelListener::onEndModel,&listener ) );

		if (!parser.load( ser.getDocumentNode() ))
		{
			listener.destroyTemplates();
			return false;
		}

		// store templates
		detail::DotModelListener::TplList& tpls = listener.getTemplates();
		VectorIterator<detail::DotModelListener::TplList> itTpl(tpls);
		while (itTpl.hasMoreElements())
		{
			detail::DotModelListener::TplList::value_type tpl = itTpl.getNext();
			this->modelTemplates_[tpl.first] = SharedPtr<ModelTemplate>(tpl.second);
		}
		listener.reset();

		return true;
	}
	Model* ModelManager::createModelFromTemplate(const String& modelName, const String& tplName)
	{
		YAKE_ASSERT( !modelName.empty() )(modelName)(tplName);
		YAKE_ASSERT( !tplName.empty() )(modelName)(tplName);

		// lookup template
		ModelTemplateList::const_iterator it = this->modelTemplates_.find( tplName );
		YAKE_ASSERT( it != this->modelTemplates_.end() )(modelName)(tplName);

		// bail out if template could not be found
		if (it == this->modelTemplates_.end())
			return 0;

		// create model using template
		YAKE_ASSERT( it->second.get() )(modelName)(tplName);
		return this->createModel(modelName, it->second->getComponentDescriptions());
	}

} // namespace model
} // namespace yake
