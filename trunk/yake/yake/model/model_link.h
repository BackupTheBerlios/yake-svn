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
#ifndef YAKE_MODEL_LINK_H
#define YAKE_MODEL_LINK_H

#include "yake/model/prerequisites.h"

namespace yake {
namespace model {

	struct YAKE_MODEL_API ModelLink
	{
		virtual ~ModelLink() {}
		YAKE_DECLARE_REGISTRY_0( ModelLink, ::yake::String );
	};
	struct YAKE_MODEL_API ModelLinkContainer
	{
		ModelLinkContainer();
		virtual ~ModelLinkContainer();
		void addModelLink(ModelLink*);
		void removeModelLink(ModelLink*);
		void destroyAllModelLinks();
	private:
		ModelLinkContainer(const ModelLinkContainer&);
		ModelLinkContainer& operator=(const ModelLinkContainer&);
	protected:
		//void _cloneLinks(ModelLinkContainer& cloned) const;
	protected:
		typedef std::deque<ModelLink*> ModelLinkList;
		ModelLinkList	links_;
	};
	template<typename Arg0Type>
	struct Updatable
	{
		virtual ~Updatable() {}
		virtual void update(const Arg0Type) = 0;
	};
	struct YAKE_MODEL_API ModelMovableLink : public ModelLink, public Updatable<real>
	{
	public:
		typedef Signal1< void(const Vector3&) > PositionSignal;
		typedef Signal1< void(const Quaternion&) > OrientationSignal;

		SignalConnection subscribeToPositionChanged( const PositionSignal::slot_type& slot );
		SignalConnection subscribeToOrientationChanged( const OrientationSignal::slot_type& slot );
		SignalConnection subscribeToPositionChanged( Movable* pMovable );
		SignalConnection subscribeToOrientationChanged( Movable* pMovable );

	protected:
		ModelMovableLink();
	protected:
		PositionSignal		mPositionSignal;
		OrientationSignal	mOrientationSignal;

		Vector3				mLastPosition;
		Quaternion			mLastOrientation;
	};
	/** A link controller designed to forward position and/or orientation to
		to other clients using signals. Factory item id is "yake.movable".
		@Remarks A controller only fires signals when the values have changed!
	*/
	struct ModelMovableDirectLink : public ModelMovableLink
	{
	public:
		YAKE_DECLARE_CONCRETE( ModelMovableDirectLink, "yake.movable" );

		ModelMovableDirectLink();
		virtual void update( const real timeElapsed );
	};
	/** A link controller designed to forward absolute (world space) position and/or orientation to
		to other clients using signals. Factory item id is "yake.movable".
		@Remarks A controller only fires signals when the values have changed!
	*/
	struct ModelMovableWorldLink : public ModelMovableLink
	{
	public:
		YAKE_DECLARE_CONCRETE( ModelMovableWorldLink, "yake.movable_world" ); // world coordinate system

		ModelMovableWorldLink();
		virtual void update( const real timeElapsed );
	};
	struct ModelMovableLink_decoupled : public ModelMovableLink
	{
	public:
		YAKE_DECLARE_CONCRETE( ModelMovableWorldLink, "yake.movable.decoupled" );

		ModelMovableLink_decoupled();
		virtual void update( const real timeElapsed );
		//@todo add history/interpolation specific code
	};

} // namespace model
} // namespace yake

#endif
