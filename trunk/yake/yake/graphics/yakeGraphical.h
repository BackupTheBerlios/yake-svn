/**------------------------------------------------------------------------------------
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
*/
#ifndef YAKE_GRAPHICAL_H
#define YAKE_GRAPHICAL_H

#include <yakeGraphics/yake/yakeGraphics.h>

#ifndef YAKE_BASE_MOVABLE_H
#include <yakeBase/yake/yakeMovable.h>
#endif

#ifndef YAKE_PHYSICS_PHYSICSSYSTEM_H
#include <yakeGraphics/yake/yakeGraphicsSystem.h>
#endif

namespace yake {
namespace graphics {

	/** High level graphical object in a hierarchy. Can hold any number of graphics entities.
		Graphics entities can be associated with names which are valid within
		a single Graphical object.
		\see GraphicsEntity
		\see RefPtr
	*/
	class YAKE_GRAPHICS_API Graphical : public base::Movable
	{
	public:
		Graphical();
		virtual ~Graphical();

		virtual void setPosition( const Vector3 & position );
		virtual void setOrientation( const Quaternion & orientation );
		virtual Vector3 getPosition() const;
		virtual Quaternion getOrientation() const;

		virtual void setBaseMovable( Movable & pBaseObj );
		virtual void removeBaseMovable();

		/** Add a Graphical child object.
			\param pGraphical a weak pointer to a Graphical object.
		*/
		virtual void addChild( Graphical* pGraphical );

		/** Add a graphics entity.
			Graphics entities are passed and stored using ref pointers.
			\see associate
			\see getGraphicsEntity
			\param pEnt a weak pointer to a GraphicsEntity object pointer.
		*/
		virtual void addGraphicsEntity( graphics::GraphicsEntity* pEnt );

		/** Associate a graphics entity with a name.
			An association is only valid within a single Graphical object.
			If pEnt is 0 or name is empty no association will be created.
			The association uses a raw pointer to the associated object.
			\see getGraphicsEntity
			\see addGraphicsEntity
			\param name the name for graphics entity.
		*/
		virtual void associate( const base::String & name, graphics::GraphicsEntity* pEnt );
		
		/** Returns a graphics entity for a given name.
			Prior to calling this method the graphics entity must have been associated
			with this name using associate().
			\see associate
			\see addGraphicsEntity
			\param name name of the graphics entity to return.
			\return a weak pointer to the graphics entity.
		*/
		virtual graphics::GraphicsEntity* getGraphicsEntity( const base::String & name );

	protected:
		typedef std::list< graphics::GraphicsEntity* > GraphicsEntityList;
		GraphicsEntityList		mGraphicsEntities;

		typedef base::templates::AssocVector< base::String, graphics::GraphicsEntity* > GraphicsEntityNameMap;
		GraphicsEntityNameMap	mGraphicsEntityNames;

		typedef std::list< Graphical* > GraphicalList;
		GraphicalList			mChildren;

		Movable*				mBaseMovable;
	};

}
}

#endif

