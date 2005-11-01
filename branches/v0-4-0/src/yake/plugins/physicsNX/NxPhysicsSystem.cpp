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
#include <yake/plugins/physicsNX/yakePCH.h>
#include <yake/plugins/physicsNX/yakeWorldNx.h>
#include <yake/plugins/physicsNX/NxPhysicsSystem.h>
#include "NxMaterial.h"

class ErrorStream : public NxUserOutputStream
	{
	public:
	void reportError(NxErrorCode e, const char* message, const char* file, int line)
		{
		printf("%s (%d) :", file, line);
		switch (e)
			{
			case NXE_INVALID_PARAMETER:
				printf( "invalid parameter");
				break;
			case NXE_INVALID_OPERATION:
				printf( "invalid operation");
				break;
			case NXE_OUT_OF_MEMORY:
				printf( "out of memory");
				break;
			case NXE_DB_INFO:
				printf( "info");
				break;
			case NXE_DB_WARNING:
				printf( "warning");
				break;
			default:
				printf("unknown error");
			}

		printf(" : %s\n", message);
		}

	NxAssertResponse reportAssertViolation(const char* message, const char* file, int line)
		{
			printf("access violation : %s (%s line %d)\n", message, file, line);
			//return NX_AR_CONTINUE;
			return NX_AR_BREAKPOINT;
		}

	void print(const char* message)
		{
		printf(message);
		}
	};
ErrorStream gErrorStream;

namespace yake {
namespace physics {

	YAKE_REGISTER_CONCRETE( NxPhysicsSystem );

	NxPhysicsSDK* getNxSDK()
	{
		return NxPhysicsSystem::mpNxSDK;
	}

	NxExtensions* getNxExt()
	{
		return NxPhysicsSystem::mpNxExt;
	}

	NxPhysicsSDK* NxPhysicsSystem::mpNxSDK = 0;
	NxExtensions* NxPhysicsSystem::mpNxExt = 0;

	//------------------------------------------------------
	NxPhysicsSystem::NxPhysicsSystem() : IPhysicsSystem()
	{
		YAKE_ASSERT(!mpNxSDK);
		if (mpNxSDK)
			return;
		mpNxSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, &gErrorStream);
		if (!mpNxSDK)
			return;

		YAKE_ASSERT(!mpNxExt);
		if (mpNxExt)
			return;
		mpNxExt = GetNxExtensions(0, &gErrorStream, 0);

		// default settings
		mpNxSDK->setParameter(NX_MIN_SEPARATION_FOR_PENALTY, -0.05f);


	}

	//------------------------------------------------------
	NxPhysicsSystem::~NxPhysicsSystem()
	{
		//YAKE_SAFE_DELETE( mpNxSDK );
	}

	//------------------------------------------------------
	SharedPtr<IWorld> NxPhysicsSystem::createWorld()
	{
		YAKE_ASSERT( mpNxSDK ).error("Novodex SDK isn't initialised!");

		SharedPtr<WorldNx> pWorld( new WorldNx() );
		YAKE_ASSERT( pWorld ).debug("Out of memory ?");
		if (!pWorld->init_())
		{
			pWorld.reset();
			YAKE_ASSERT( 1==0 ).debug("World could not be initialized!");
			return SharedPtr<IWorld>();
		}
		return pWorld;
	}

}
}
