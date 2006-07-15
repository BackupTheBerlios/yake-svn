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
	namespace data {
	namespace parser {
	namespace dotscene {
		class DotSceneParser;
	}
	namespace xode {
		class XODEParser;
	}
	}
	}
namespace model {
	//@todo move into private header:
	struct CentralControllerBase;
	/** @remarks Do *NOT* rely on the existence of the members of this structure!
				Always access them through ModelManager!
	*/
	struct ComponentCreationContext
	{
		Model*					model_;
		graphics::IWorld*		gworld_;
		physics::IWorld*		pworld_;

		CentralControllerBase*	centralController_;

		data::parser::dotscene::DotSceneParser*		dotSceneParser_;
		data::parser::xode::XODEParser*				xodeParser_;

		typedef AssocVector<String,boost::any> ParamMap;
		ParamMap				params_;

		typedef Signal2<void(const ComponentCreationContext&,ModelComponent&)> ComponentPreInitializeSignal;
		typedef ComponentPreInitializeSignal ComponentPostInitializeSignal;
		ComponentPreInitializeSignal	sigPreInit_;
		ComponentPostInitializeSignal	sigPostInit_;

		ComponentCreationContext() : 
			model_(0),
			gworld_(0),
			pworld_(0),
			centralController_(0),
			dotSceneParser_(0),
			xodeParser_(0)
		{}
	};

	struct YAKE_MODEL_API ModelComponent //: public Clonable<ModelComponent>
	{
		ModelComponent() {}
		virtual ~ModelComponent() {}
		//Model* getOwner() const;

		/** @name Types */
		//@{
		struct Desc {
			String		name;
			String		type;
			StringMap	params;
			Desc() {}
			Desc(const String& n, const String& t, const StringMap& p = StringMap()) :
				name(n), type(t), params(p)
			{}
		};
		typedef std::deque<Desc> DescList;
		//@}
	private:
		//ModelComponent();
		ModelComponent(const ModelComponent&);
		ModelComponent& operator=(const ModelComponent&);
	protected:
		//ModelComponent(Model& owner) : owner_(&owner) {}
	private:
		//Model*	owner_;
	};
	typedef ModelComponent::DescList ModelComponentDescList;
	typedef SharedPtr<ModelComponent> ModelComponentSharedPtr;
	typedef std::deque<ModelComponentSharedPtr> ModelComponentList;
	typedef std::string ComponentTag;
	struct YAKE_MODEL_API ModelComponentContainer
	{
		ModelComponentContainer();
		~ModelComponentContainer();
		/** @name Component container interface */
		//@{
		void addComponent(ModelComponent*);
		void addComponent(ModelComponent*, const ComponentTag&);
		ModelComponent* getComponentByTag(const ComponentTag&) const;
		ModelComponent* getComponentByIndex(const size_t) const;
		size_t numComponents() const;
		bool empty() const;
		//@}
	private:
		ModelComponentContainer(const ModelComponentContainer&);
		ModelComponentContainer& operator=(const ModelComponentContainer&);
	protected:
		//void _cloneComponents(Model& cloned) const;
	protected:
		ModelComponentList		components_;
		typedef std::map<ComponentTag,ModelComponent*> TagModelComponentMap;
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
		//Physical(Model& owner);
		Physical();
		virtual ~Physical();

		void addActor(physics::IActor*, const String&);
		void addBody(physics::IBody*, const String&);
		void addJoint(physics::IJoint*, const String&);
		void addAffector(physics::IBodyAffector*, const String&);
		void addAffectorTargetBody(physics::IBodyAffector*,physics::IBody*);
		void addAffectorTargetBody(physics::IBodyAffector*,const String& bodyXPath);
		void addAffectorTargetActor(physics::IBodyAffector*,const String& actorXPath);

		physics::IActor* getActor(const String&) const;
		physics::IBody* getBody(const String&) const;
		physics::IJoint* getJoint(const String&) const;
		physics::IBodyAffector* getAffector(const String&) const;

		void updateAffectors(const real dt);

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
		template<typename T>
		struct EntrySharedT
		{
			typedef SharedPtr<T> pointer;
			pointer	p_;
			EntrySharedT(pointer p) : p_(p)
			{}
		};
		typedef AssocVector<String,EntryT<physics::IActor> > TagActorMap;
		typedef AssocVector<String,EntryT<physics::IBody> > TagBodyMap;
		typedef AssocVector<String,EntryT<physics::IJoint> > TagJointMap;
		typedef AssocVector<String,EntrySharedT<physics::IBodyAffector> > TagAffectorMap;
		TagActorMap		actors_;
		TagBodyMap		bodies_;
		TagJointMap		joints_;
		TagAffectorMap	affectors_;

		typedef AssocVector<physics::IBodyAffector*, physics::BodyGroup> AffectorTargetMap;
		AffectorTargetMap	affectorTargetMap_;

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
		void _add(T* obj,AssocVector<String,EntrySharedT<T> >& ctr, const String& xpath)
		{
			YAKE_ASSERT( obj );
			YAKE_ASSERT( !xpath.empty() );
#ifdef YAKE_DEBUG_BUILD
			YAKE_ASSERT( ctr.end() == ctr.find(xpath) )(xpath).debug(_T("Item with this xpath already exists!"));
#endif
			ctr.insert( std::make_pair( xpath, typename EntrySharedT<T>::pointer(obj) ) );
		}
		template<typename T>
		T* _get(const AssocVector<String,EntryT<T> >& ctr, const String& xpath) const
		{
			typename AssocVector<String,EntryT<T> >::const_iterator it = ctr.find( xpath );
			return ((it != ctr.end()) ? it->second.p_ : 0);
		}
		template<typename T>
		T* _get(const AssocVector<String,EntrySharedT<T> >& ctr, const String& xpath) const
		{
			typename AssocVector<String,EntrySharedT<T> >::const_iterator it = ctr.find( xpath );
			return ((it != ctr.end()) ? it->second.p_.get() : 0);
		}
	};
	struct YAKE_MODEL_API Graphical : public ModelComponent
	{
		//Graphical(Model& owner);
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
		//Graphical();
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

	struct YAKE_MODEL_API ComponentCreator
	{
		YAKE_DECLARE_REGISTRY_0(ComponentCreator, String);
		virtual ~ComponentCreator() {}

		virtual void create(const ComponentCreationContext& ctx, const StringMap& params) = 0;
	};
	struct YAKE_MODEL_API ComponentCreatorManager
	{
		ComponentCreatorManager();
		~ComponentCreatorManager();

		void create(const String& type, const ComponentCreationContext& ctx, const StringMap& params);
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
