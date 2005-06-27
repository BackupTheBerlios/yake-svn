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
#include <yapp/ent/yakePCH.h>
#include <yapp/ent/yakeCommon.h>
#include <yapp/ent/yakeEvent.h>
#include <yapp/ent/yakeMessaging.h>
#include <yapp/ent/yakeObject.h>
#include <yapp/ent/yakeEntity.h>
#include <yapp/ent/yakeSim.h>

namespace yake {
namespace ent {

	DEFINE_OBJECT( light )

	light::light() : mpLight(0), mLightEnabled(false)
	{
	}
	void light::onInitialise(object_creation_context& creationCtx)
	{
		Entity::onInitialise(creationCtx);
		//if (!isServer())
		{
			YAKE_ASSERT( creationCtx.mpGWorld );
			mpLight = creationCtx.mpGWorld->createLight();
			YAKE_ASSERT( mpLight );
			mpSN.reset( creationCtx.mpGWorld->createSceneNode() );
			YAKE_ASSERT( mpSN.get() );
			mpSN->attachLight( mpLight );

			mpLight->setType( graphics::ILight::LT_POINT );
			//mpLight->setDirection( Vector3(1,1,0) );
			mpLight->setDiffuseColour( Color(1,0,0) );
			mpLight->setEnabled( true );

			mLightEnabled = true;

			mpSN->setPosition( Vector3(0,20,0) );
		}
	}
	void light::enableLight( bool yes )
	{
		//if (!isServer())
		{
			mLightEnabled = yes;
			if (mpLight)
				mpLight->setEnabled( yes );
		}
	}
	bool light::isLightEnabled() const
	{
		return mLightEnabled;
	}

} // namespace yake
} // namespace ent
