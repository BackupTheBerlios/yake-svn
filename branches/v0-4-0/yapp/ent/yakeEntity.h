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
#ifndef YAKE_ENT_ENTITY_H
#define YAKE_ENT_ENTITY_H

namespace yake {
namespace ent {

	class Entity;

	typedef YAKE_ENT_API ::yake::model::Graphical GraphicalModel;

	/**
	@todo use yake::reflection instead of custom object_class stuff.
	*/
	class YAKE_ENT_API Entity : public Object
	{
		friend class sim;
		DECLARE_OBJECT(Entity)
	private:
		Entity(const Entity&);
	protected:
		Entity();
		virtual void onInitialise(object_creation_context& creationCtx);
		virtual void onTick();
		virtual void onGetDefaultEventParams(ParamList& params);
	public:
		simtime getAge() const;

		void addVM(scripting::IVM* pVM);
		scripting::IVM* getVM(const size_t index) const;
		size_t getVMCount() const;
		scripting::IVM* getDefaultVM() const;

		EntityMachine* ceateMachine(const String& id);
		EntityMachine* getDefaultMachine() const;
		EntityMachine* getMachine(const String& id);

		OBJECT_PROPS_NONE(Entity);

		void addComponent( const String& tag, const SharedPtr<EntityComponent>& pComponent, size_t prio = 0 );
		SharedPtr<EntityComponent> removeComponent( EntityComponent* pComponent );
		SharedPtr<EntityComponent> removeComponent( const String& tag );
		EntityComponent* getComponent( const String& tag );

		//YAKE_MEMBERSIGNAL( public, void(Entity*,EntityMachine*,EntityMachineState*), StateAdded)
		//YAKE_MEMBERSIGNAL( public, void(Entity*,EntityMachine*,const String&), ExitState)
		//YAKE_MEMBERSIGNAL( public, void(Entity*,EntityMachine*,const String&), EnterState)
	private:
		//MsgResultCode onCmdMachineEnterState(const Message& msg);
		MsgResultCode onCmdMachineChangeTo(const Message& msg);
	private:
		simtime						mAge;

		typedef Vector<scripting::IVM*>	VMList;
		VMList						mVMs;

		typedef std::map<String,SharedPtr<EntityMachine> > MachineMap;
		MachineMap					mMachines;
		SharedPtr<EntityMachine>	mDefaultMachine;

		typedef std::map<String,SharedPtr<EntityComponent> > ComponentList;
		ComponentList				mComponents;

		void _tickComponents();
		void _initialiseComponents(object_creation_context& creationCtx);
	};

#define YAKE_DECLARE_ENTITY( ENTITYCLASS ) DECLARE_OBJECT( ENTITYCLASS )
#define YAKE_DEFINE_ENTITY( ENTITYCLASS ) DEFINE_OBJECT( ENTITYCLASS )
#define YAKE_DEFINE_ENTITY_1( ENTITYCLASS, PARENT0 ) DEFINE_OBJECT_1( ENTITYCLASS, PARENT0 )

	class YAKE_ENT_API PawnVisual : public EntityComponent
	{
		YAKE_DECLARE_ENTITY_COMPONENT( PawnVisual, "pawn.visual" );
	protected:
		virtual void onInitialise(object_creation_context& creationCtx);
		virtual void onTick();
	public:
		void setGraphical( GraphicalModel* pModel );
		GraphicalModel* getGraphical() const;
	private:
		void onPositionChanged(Property& prop);
	private:
		SharedPtr<GraphicalModel>	mGraphical;
	};

	class YAKE_ENT_API Pawn : public Entity
	{
		YAKE_DECLARE_ENTITY(Pawn)
	public:
		OBJECT_PROPS_BEGIN(Pawn)
			OBJECT_PROP("position", Vector3, Vector3(0,0,0))
		OBJECT_PROPS_END()
	protected:
		virtual void onTick();
	protected:
		Pawn();
	};

	class YAKE_ENT_API LightVisual : public EntityComponent
	{
		YAKE_DECLARE_ENTITY_COMPONENT( LightVisual, "light.visual" );
	protected:
		virtual void onInitialise(object_creation_context& creationCtx);
		virtual void onTick();
	private:
		SharedPtr<graphics::ISceneNode>		mpSN;
		graphics::ILight*					mpLight;
		bool								mLightEnabled;
	};

	class YAKE_ENT_API Light : public Entity
	{
		YAKE_DECLARE_ENTITY(Light)
	public:
		void enableLight( bool yes );
		bool isLightEnabled() const;

		OBJECT_PROPS_BEGIN(Light)
			OBJECT_PROP("diffuseColour", Color, Color(1,1,1,1))
			OBJECT_PROP("position", Vector3, Vector3(0,0,0))
			OBJECT_PROP("enabled", bool, true)
		OBJECT_PROPS_END()
	protected:
		Light();
		virtual void onInitialise(object_creation_context& creationCtx);
		virtual void onTick();
	};

} // namespace yake
} // namespace ent
#endif
