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
#ifndef YAKE_ENT_ACTOR_H
#define YAKE_ENT_ACTOR_H

namespace yake {
namespace ent {

	class ActorBehaviour;
	class SensoryInputHistory;

	class YAKE_ENT_API Actor : public Pawn
	{
		YAKE_DECLARE_ENTITY(Actor,"Actor")
	public:
		enum LifeState {
			kSpawning,
			kAlive,
			kDying,
			kDead
		};
		OBJECT_PROPS_BEGIN(Actor)
			OBJECT_PROP("lifeState", LifeState, kDead)
			OBJECT_PROP("enabled", bool, true)
			OBJECT_PROP("position", Vector3, Vector3(0,0,0))
		OBJECT_PROPS_END()

		void addBehaviour(ActorBehaviour*);

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

		YAKE_MEMBERSIGNAL( public, void(void), OnSense );
		YAKE_MEMBERSIGNAL( public, void(void), OnThink );
		YAKE_MEMBERSIGNAL( public, void(void), OnAct );
	protected:
		virtual void onInitialise(object_creation_context& creationCtx);
		virtual void onTick();

		virtual void onSense() {}
		virtual void onThink() {}
		virtual void onAct() {}
	protected:
		Actor();
	};


} // namespace yake
} // namespace ent
#endif
