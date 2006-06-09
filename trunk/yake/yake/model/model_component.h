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
#ifndef YAKE_MODEL_COMPONENT_H
#define YAKE_MODEL_COMPONENT_H

#include "yake/model/prerequisites.h"

namespace yake {
namespace model {

	struct YAKE_MODEL_API ModelComponent //: public Clonable<ModelComponent>
	{
		ModelComponent() {}
		virtual ~ModelComponent() {}
	private:
		ModelComponent(const ModelComponent&);
		ModelComponent& operator=(const ModelComponent&);
	};
	typedef SharedPtr<ModelComponent> ModelComponentSharedPtr;
	typedef std::deque<ModelComponentSharedPtr> ModelComponentList;
	typedef std::string ComponentTag;
	struct YAKE_MODEL_API ModelComponentContainer
	{
		ModelComponentContainer();
		void addComponent(ModelComponent*);
		void addComponent(ModelComponent*, const ComponentTag&);
		ModelComponent* getComponentByTag(const ComponentTag&) const;
		ModelComponent* getComponentByIndex(const size_t) const;
		size_t numComponents() const;
		bool empty() const;
	private:
		ModelComponentContainer(const ModelComponentContainer&);
		ModelComponentContainer& operator=(const ModelComponentContainer&);
	protected:
		//void _cloneComponents(Model& cloned) const;
	protected:
		ModelComponentList		components_;
		typedef std::map<ComponentTag,ModelComponentSharedPtr> TagModelComponentMap;
		TagModelComponentMap	tag2components_;
	};

	////@todo move out:

	struct YAKE_MODEL_API Physical : public ModelComponent
	{
	private:
		Physical(const Physical&);
		Physical& operator=(const Physical&);
	public:
		//@todo register factory?
		Physical();
		virtual ~Physical();

		void addActor(physics::IActor*, const String&);
		void addBody(physics::IBody*, const String&);
		void addJoint(physics::IJoint*, const String&);

		physics::IActor* getActor(const String&) const;
		physics::IBody* getBody(const String&) const;
		physics::IJoint* getJoint(const String&) const;

		//getShape(const String&,const String&)

		//void setPosition(const Vector3&);
		//void setOrientation(const Quaternion&);
		//void translate(const Vector3&);
		//void rotate(const Quaternion&);
	private:
		template<typename T>
		struct EntryT
		{
			typedef T* pointer;
			pointer	p_;
			EntryT(pointer p) : p_(p)
			{}
		};
		typedef AssocVector<String,EntryT<physics::IActor> > TagActorMap;
		typedef AssocVector<String,EntryT<physics::IBody> > TagBodyMap;
		typedef AssocVector<String,EntryT<physics::IJoint> > TagJointMap;
		TagActorMap		actors_;
		TagBodyMap		bodies_;
		TagJointMap		joints_;

		template<typename T>
		void _add(T* obj,AssocVector<String,EntryT<T> >& ctr, const String& xpath)
		{
			YAKE_ASSERT( obj );
			YAKE_ASSERT( !xpath.empty() );
#ifdef YAKE_DEBUG_BUILD
			YAKE_ASSERT( ctr.end() == ctr.find(xpath) )(xpath).debug(_T("Item with this xpath already exists!"));
#endif
			ctr.insert( std::make_pair( xpath, EntryT<T>(obj) ) );
		}
		template<typename T>
		T* _get(const AssocVector<String,EntryT<T> >& ctr, const String& xpath) const
		{
			typename AssocVector<String,EntryT<T> >::const_iterator it = ctr.find( xpath );
			return ((it != ctr.end()) ? it->second.p_ : 0);
		}
	};
	struct YAKE_MODEL_API Graphical : public ModelComponent
	{
		Graphical();
		virtual ~Graphical();
		//@todo register factory?

		//virtual ModelComponent* clone() const;

		void addSceneNode(graphics::ISceneNode*, const String& xpath, const bool owned = true);
		void addEntity(graphics::IEntity*, const String& xpath, const bool owned = true);
		void addLight(graphics::ILight*, const String& xpath, const bool owned = true);
		//void addCamera()

		graphics::ISceneNode* getSceneNode(const String& xpath) const;
		graphics::IEntity* getEntity(const String& xpath) const;
		graphics::ILight* getLight(const String& xpath) const;

		//void setPosition(const Vector3&);
		//void setOrientation(const Quaternion&);

		///! translates root nodes only!
		void translate(const Vector3&);
		//void rotate(const Quaternion&);
	private:
		Graphical(const Graphical&);
		Graphical& operator=(const Graphical&);
	private:
		template<typename T>
		struct EntryT
		{
			typedef T* pointer;
			pointer	p_;
			bool	owned_;
			EntryT(pointer p, const bool owned) : p_(p), owned_(owned)
			{}
		};
		typedef AssocVector<String,EntryT<graphics::ISceneNode> > TagNodeMap;
		typedef AssocVector<String,EntryT<graphics::IEntity> > TagEntityMap;
		typedef AssocVector<String,EntryT<graphics::ILight> > TagLightMap;
		TagNodeMap		nodes_;
		TagEntityMap	entities_;
		TagLightMap		lights_;
		template<typename T>
		void _add(T* obj,AssocVector<String,EntryT<T> >& ctr, const String& xpath, const bool owned)
		{
			YAKE_ASSERT( obj );
			YAKE_ASSERT( !xpath.empty() );
#ifdef YAKE_DEBUG_BUILD
			YAKE_ASSERT( ctr.end() == ctr.find(xpath) )(xpath).debug(_T("Item with this xpath already exists!"));
#endif
			ctr.insert( std::make_pair( xpath, EntryT<T>(obj,owned) ) );
		}
		template<typename T>
		T* _get(const AssocVector<String,EntryT<T> >& ctr, const String& xpath) const
		{
			typename AssocVector<String,EntryT<T> >::const_iterator it = ctr.find( xpath );
			return ((it != ctr.end()) ? it->second.p_ : 0);
		}
	};
	//@todo move into private header
	struct ComponentCreationContext
	{
		graphics::IWorld*	gworld_;
		physics::IWorld*	pworld_;
		ComponentCreationContext() : gworld_(0), pworld_(0)
		{}
	};
	struct YAKE_MODEL_API ComponentCreator
	{
		YAKE_DECLARE_REGISTRY_0(ComponentCreator, String);
		virtual ~ComponentCreator() {}

		virtual ModelComponent* create(const ComponentCreationContext& ctx, const StringMap& params) = 0;
	};
	struct GraphicalFromDotSceneCreator : public ComponentCreator
	{
		YAKE_DECLARE_CONCRETE(GraphicalFromDotSceneCreator,"graphics/dotScene");

		virtual ModelComponent* create(const ComponentCreationContext& ctx, const StringMap& params);
	};
	struct YAKE_MODEL_API ComponentCreatorManager
	{
		ComponentCreatorManager();
		~ComponentCreatorManager();

		ModelComponent* create(const String& type, const ComponentCreationContext& ctx, const StringMap& params);
	private:
		ComponentCreatorManager(const ComponentCreatorManager&);
		ComponentCreatorManager& operator=(const ComponentCreatorManager);
	private:
		typedef AssocVector<String,SharedPtr<ComponentCreator> > TypeCreatorMap;
		TypeCreatorMap	creators_;
	};

} // namespace model
} // namespace yake

#endif
