/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
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
#ifndef YAKE_APP_MODEL_ACTOR_H
#define YAKE_APP_MODEL_ACTOR_H

#include <yapp/base/yappPrerequisites.h>

namespace yake {
namespace app {
namespace model {

	/** Base class for Actor components.
		@see Actor
	*/
	class YAPP_BASE_API Component
	{
	public:
		virtual ~Component()
		{}
	};
	class YAPP_BASE_API PhysicalComponent : public Component
	{
	};
	class YAPP_BASE_API VisualComponent : public Component
	{
	};
	class YAPP_BASE_API InputComponent : public Component
	{
	public:
		virtual void update() = 0;
	};

	class YAPP_BASE_API ActorControl : public Component
	{
	public:
		virtual void update() = 0;
	};

/*
	class YAPP_BASE_API Behaviour
	{
	public:
		virtual ~Behaviour() {}
		//@{ name behaviour
		void addOnSenseScriptCallback( IScriptCallback* pCallback );
		void addOnThinkScriptCallback( IScriptCallback* pCallback );
		void addOnActScriptCallback( IScriptCallback* pCallback );

		void sense();
		void think();
		void act();
	protected:
		virtual void onSense() = 0;
		virtual void onThink() = 0;
		virtual void onAct() = 0;
		//@}
	};
*/

	class IScriptCallback;

	/** Actor represents a high-level entity in an application/game.
		It's appearance (graphical, physical, audible, etc) and its behaviour
		is defined by various attachable components.
		@Remarks The actor code is still very much experimental and in early stages!
			You've been warned! Lots of it is still in flux.
		@todo use rx and rx::events (incl. scripted callbacks, class inheritance...)
	*/
	class YAPP_BASE_API Actor
	{
	protected:
		Actor();
	public:
		virtual ~Actor();

		//@{ name components
		void addComponent( Component* pComponent, const String & rTag );
		Component* getComponent( const String & rTag ) const;
		PhysicalComponent* getPhysicalComponent( const String & rTag ) const;
		VisualComponent* getVisualComponent( const String & rTag ) const;
		InputComponent* getInputComponent( const String & rTag ) const;
		ActorControl* getControlComponent( const String & rTag ) const;
		//@}

		//todo merge this into Behaviour ?
		//@{ name behaviour
		void addOnSenseScriptCallback( IScriptCallback* pCallback );
		void addOnThinkScriptCallback( IScriptCallback* pCallback );
		void addOnActScriptCallback( IScriptCallback* pCallback );

		/** Gather input, be it from AI senses (Sight, Hearing, ...) or
			input from a human player.
			The behaviour can be overriden in onSense().
			@see think
			@see act
			@see onSense
		*/
		void sense();

		/** Make decision depending on inputs.
			The behaviour can be overriden in onThink().
			@see sense()
			@see act()
			@see onThink()
		*/
		void think();

		/** Act depending on the decisions made in sense().
			The behaviour can be overriden in onAct().
			@see sense()
			@see think()
			@see onAct()
		*/
		void act();
	protected:
		virtual void onSense()
		{}
		virtual void onThink()
		{}
		virtual void onAct()
		{}
		//@}
	private:
		typedef ::yake::AssocVector< String, Component* > ComponentList;
		ComponentList	mComponents;
	};

}
}
}


#endif
