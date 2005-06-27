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
#include <yake/plugins/audioOpenAL/yakePCH.h>
#include <yake/plugins/audioOpenAL/yakeAudioSystemOpenAL.h>

namespace yake {
namespace audio {

	//FIXME: Move this away from here:
	YAKE_REGISTER_CONCRETE( AudioSystemOpenAL )

	AudioSystemOpenAL::AudioSystemOpenAL()
	{
	}
	AudioSystemOpenAL::~AudioSystemOpenAL()
	{
	}
	IWorld* AudioSystemOpenAL::createWorld()
	{
		return new WorldOpenAL();
	}

	WorldOpenAL::WorldOpenAL() : mEnv(0)
	{
		mEnv = new openalpp::AudioEnvironment();
		YAKE_ASSERT( mEnv.valid() ).debug("Could not instantiate OpenAL++ audio environment. Out of memory ?");
		setDopplerFactor( 1. );
		setSoundVelocity( 300. );
		mEnv->setDistanceModel( openalpp::None );
	}

	WorldOpenAL::~WorldOpenAL()
	{
	}

	ISoundData* WorldOpenAL::createSoundDataFromFile( const String & filename )
	{
		return new SoundDataFile( filename );
	}

	/*
	ISoundData* WorldOpenAL::createSoundData(	bool b3D,
												ISoundData::LoopMode loopMode, 
												ISoundData::ChannelMode channelMode,
												ISoundData::DataMode dataMode)
	{
		YAKE_ASSERT( 1==0 ).error("Disabled! SoundData is not yet completely implemented! Use Source to play files!");
		//return new SoundDataOpenAL();
		return 0;
	}*/

	IListener* WorldOpenAL::createListener()
	{
		return new ListenerOpenAL();
	}

	ISource* WorldOpenAL::createSource()
	{
		return new SourceOpenAL();
	}

	void WorldOpenAL::setActiveListener( IListener* pListener )
	{
	}

	void WorldOpenAL::setDopplerFactor( real factor )
	{
		YAKE_ASSERT( mEnv.valid() ).debug("Need an instantiated OpenAL++ audio environment.");
		try {
			mEnv->setDopplerFactor( factor );
		} catch ( const openalpp::Error & e )
		{
			//FIXME:
		}
	}

	void WorldOpenAL::setSoundVelocity( real velocity )
	{
		YAKE_ASSERT( mEnv.valid() ).debug("Need an instantiated OpenAL++ audio environment.");
		try {
			mEnv->setSoundVelocity( velocity );
		} catch ( const openalpp::Error & e )
		{
			//FIXME:
		}
	}

}
}