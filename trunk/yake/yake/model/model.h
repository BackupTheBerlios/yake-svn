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
#ifndef YAKE_MODEL_H
#define YAKE_MODEL_H

#ifndef YAKE_MODEL_INCLUDE_LOADERS
//@todo enable #	define YAKE_MODEL_INCLUDE_LOADERS 1
#endif

#include "yake/model/prerequisites.h"
#include "yake/model/model_link.h"
#include "yake/model/model_component.h"
#if YAKE_MODEL_INCLUDE_LOADERS == 1
#	include "yake/model/loader/loader_xode.h"
#	include "yake/model/loader/loader_dotscene.h"
#endif

namespace yake {
namespace model {
	///@todo move out!!!
	struct CentralControllerBase
	{
		virtual ~CentralControllerBase() {}

		// parameters: frame (int32), timeElapsed/dt (real)
		typedef Signal2<void(const uint32, const real)> PhysicsUpdateSignal;
		typedef Signal2<void(const uint32, const real)> GraphicsUpdateSignal;
		SignalConnection subscribeToPhysicsUpdate(const PhysicsUpdateSignal::slot_type& slot)
		{ return physicsUpdateSignal_.connect(slot); }
		SignalConnection subscribeToGraphicsUpdate(const GraphicsUpdateSignal::slot_type& slot)
		{ return graphicsUpdateSignal_.connect(slot); }
		void triggerPhysicsUpdateSignal(const uint32 si, const real dt)
		{ physicsUpdateSignal_(si,dt); }
		void triggerGraphicsUpdateSignal(const uint32 si, const real dt)
		{ graphicsUpdateSignal_(si,dt); }
	protected:
		PhysicsUpdateSignal		physicsUpdateSignal_;
		GraphicsUpdateSignal	graphicsUpdateSignal_;
	};

	template<typename T>
	struct Clonable
	{
		virtual ~Clonable() {}
		virtual T* clone() const = 0;
	};

	struct YAKE_MODEL_API Model
	{
		Model();
		//virtual ~Model() {} //@todo virtual?

		/** @name basic properties */
		//@{
		void setName(const String&);
		const String& getName() const;
		//@}

		/** @name component management */
		//@{
		void addComponent(ModelComponent*);
		void addComponent(ModelComponent*, const ComponentTag&);
		ModelComponent* removeComponent(ModelComponent*);
		ModelComponent* getComponentByTag(const ComponentTag&) const;
		ModelComponent* getComponentByIndex(const size_t) const;
		size_t numComponents() const;
		//@}

		/** @name link management */
		//@{
		void addLink(ModelLink*);
		void removeLink(ModelLink*);
		void destroyAllLinks();
		//@}

		/** @name signals */
		//@{
		//@}

		/** @name helpers (for convencience only) */
		//@{
		/** For convenience! This function creates a ModelMovableLink between
			source and target movable objects. Both position and rotation
			are marked for updates.
			@Remarks This function is optimized to cache the created links
				for each source. That means that for each source there's
				only ever a single link (instead of having one for each
				pair of source/target objects).
		*/
		ModelMovableLink* createLink(Movable*,Movable*,const String& linkType = _T("yake.movable"));
		ModelMovableLink* createDirectLink(Movable*,Movable*); // uses "yake.movable"
		ModelMovableLink* createWorldSpaceLink(Movable*,Movable*); // uses "yake.movable_world"
		//@}

		//Model* clone() const;
	private:
		Model(const Model&);
		Model& operator=(const Model&);
	private:
		String					name_;
		String					internalName_;

		typedef AssocVector< Movable*, ModelMovableLink* > MovableLinkMap;
		MovableLinkMap			movableLinkMap_;

		ModelLinkContainer		links_;
		ModelComponentContainer	components_;
	};
	struct ModelTemplate;
	struct YAKE_MODEL_API ModelManager
	{
	public:
		ModelManager();
		~ModelManager();

		bool loadTemplatesFromDotModel(const String& fn);

		Model* createModelFromTemplate(const String& modelName, const String& tplName);

		Model* createModel(const String& modelName, const ModelComponentDescList&);
		Model* createModel(const String& modelName, const String& def);

		void clear();

		/** @name Model/Component creation context manipulation */
		//@{
		void setCreationContext_GraphicalWorld(graphics::IWorld*);
		void setCreationContext_PhysicalWorld(physics::IWorld*);
		void setCreationContext_CentralController(CentralControllerBase*);
		void setCreationContext_DotSceneParser(data::parser::dotscene::DotSceneParser*);
		void setCreationContext_XODEParser(data::parser::xode::XODEParser*);
		void setCreationContext_NamedParameter(const String& name, const boost::any& value);
		//@}

		/** @name Signals (in order that they are called in) */
		//@{
		typedef Signal2<void(Model&,const ModelCreationContext&)> ModelCreatedSignal;
		typedef Signal3<void(Model&,const ComponentCreationContext&,const String&)> PreCreateModelComponent;
		typedef ModelCreationContext::ComponentPreInitializeSignal PreInitializeComponentSignal;
		typedef ModelCreationContext::ComponentPostInitializeSignal PostInitializeComponentSignal;
		typedef Signal3<void(Model&,const ComponentCreationContext&,const String&)> PostCreateModelComponent;
		typedef Signal2<void(Model&,const ModelCreationContext&)> ModelInitializedSignal;

		SignalConnection subscribeToModelCreatedSignal(const ModelCreatedSignal::slot_type&);
		 SignalConnection subscribeToPreCreateComponent(const PreCreateModelComponent::slot_type&);
		  SignalConnection subscribeToPreInitializeComponentSignal(const PreInitializeComponentSignal::slot_type&);
		  SignalConnection subscribeToPostInitializeComponentSignal(const PostInitializeComponentSignal::slot_type&);
		 SignalConnection subscribeToPostCreateComponent(const PostCreateModelComponent::slot_type&);
		SignalConnection subscribeToModelInitializedSignal(const ModelInitializedSignal::slot_type&);
		//@}

	private:
		ModelManager(const ModelManager&);
		ModelManager& operator=(const ModelManager&);
	private:
		ComponentCreatorManager		creatorMgr_;
		ModelCreationContext		modelCtx_;

		typedef AssocVector<String,SharedPtr<Model> > ModelList;
		ModelList					models_;

		ModelCreatedSignal			sigModelCreatedSignal_;
		PreCreateModelComponent		sigPreCreateModelComponent_;
		PostCreateModelComponent	sigPostCreateModelComponent_;
		ModelInitializedSignal		sigModelInitializedSignal_;

		typedef AssocVector<String,SharedPtr<ModelTemplate> > ModelTemplateList;
		ModelTemplateList			modelTemplates_;
	};
	/*
	struct ModelTemplateManager
	{
		ModelTemplateManager();
		~ModelTemplateManager();

		bool loadTemplatesFromDotModel(const String& fn);
		ModelTemplate* getTemplate(const String&) const;
		void destroyTemplate(const String&);
	private:
		ModelTemplateManager(const ModelTemplateManager&);
		ModelTemplateManager& operator=(const ModelTemplateManager&);
	private:
		typedef AssocVector<String,SharedPtr<ModelTemplate> > ModelTemplateList;
		ModelTemplateList	tplMap_;
	};
	*/
	/**@todo Move into private impl file. */
	struct ModelTemplate
	{
		ModelTemplate();
		~ModelTemplate();

		void addComponentDesc(const ModelComponent::Desc&);
		const ModelComponentDescList& getComponentDescriptions() const;
	private:
		ModelTemplate(const ModelTemplate&);
		ModelTemplate& operator=(const ModelTemplate&);
	private:
		ModelComponentDescList	compDescList_;
	};

} // namespace model
} // namespace yake

#endif
