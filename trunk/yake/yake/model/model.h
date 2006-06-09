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
	struct CentralController
	{
		// parameters: frame (int32), timeElapsed/dt (real)
		typedef Signal2<void(const uint32, const real)> PhysicsUpdateSignal;
		typedef Signal2<void(const uint32, const real)> GraphicsUpdateSignal;
		SignalConnection subscribeToPhysicsUpdate(const PhysicsUpdateSignal::slot_type&); 
		SignalConnection subscribeToGraphicsUpdate(const PhysicsUpdateSignal::slot_type&);
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

		// basic properties
		void setName(const String&);
		const String& getName() const;

		// component management
		void addComponent(ModelComponent*);
		void addComponent(ModelComponent*, const ComponentTag&);
		ModelComponent* removeComponent(ModelComponent*);
		ModelComponent* getComponentByTag(const ComponentTag&) const;
		ModelComponent* getComponentByIndex(const size_t) const;
		size_t numComponents() const;

		// link management
		void addLink(ModelLink*);
		void removeLink(ModelLink*);
		void destroyAllLinks();

		// signal

		// helpers (for convencience only)
		/** For convenience! This function creates a ModelMovableLink between
			source and target movable objects. Both position and rotation
			are marked for updates.
			@Remarks This function is optimized to cache the created links
				for each source. That means that for each source there's
				only ever a single link (instead of having one for each
				pair of source/target objects).
		*/
		ModelMovableLink* createLink(Movable*,Movable*,const String& linkType = "yake.movable");
		ModelMovableLink* createDirectLink(Movable*,Movable*); // uses "yake.movable"
		ModelMovableLink* createWorldSpaceLink(Movable*,Movable*); // uses "yake.movable_world"

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
	struct YAKE_MODEL_API ModelManager
	{
	public:
		ModelManager();
		~ModelManager();

		Model* createModel(const String&);

		void setCreationContext_GraphicalWorld(graphics::IWorld*);
		void setCreationContext_PhysicalWorld(physics::IWorld*);
	private:
		ModelManager(const ModelManager&);
		ModelManager& operator=(const ModelManager&);
	private:
		ComponentCreatorManager		creatorMgr_;
		ComponentCreationContext	ctx_;

		typedef Vector<SharedPtr<Model> > ModelList;
		ModelList					models_;
	};
	/**@todo Move into private impl file. */
	struct ModelTemplate : public ModelComponentContainer
	{
		ModelTemplate();
		~ModelTemplate();

		/// Example 1: from=physical:"p_mine":actor:"mineActor" to=graphical:"g_mine":scenenode:"mine_root"
		/// Example 2: from=physical:"p_mine":shape:"mineActor/sphere1" to=graphical:"g_mine":scenenode:"mine_root"
		/// Example params for ModelMovableLink: "position,orientation"
		void addLink(const String& from, const String& to, const String& params);

		/// Clones model (including components) and recreates links for the cloned model.
		Model* createInstance(const String& name) const;
	private:
		ModelTemplate(const ModelTemplate&);
		ModelTemplate& operator=(const ModelTemplate&);
	private:
		struct link_t
		{
			String	fromComponentType_;
			String	fromComponentName_;
			String	fromElementType_;
			String	fromElementName_;
			String	toComponentType_;
			String	toComponentName_;
			String	toElementType_;
			String	toElementName_;
		};
		typedef std::deque<link_t> LinkList;
		LinkList	links_;

		Model*		modelWithoutLinks_;
	private:
		void _createLinks(Model&);
	};

} // namespace model
} // namespace yake

#endif
