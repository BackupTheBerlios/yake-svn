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
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#ifndef INC_YAKE_AUDIOSYSTEM_H
#define INC_YAKE_AUDIOSYSTEM_H

#include <yake/audio/yakeAudioPrerequisites.h>

namespace yake {
	using namespace base;
	using namespace base::templates;
namespace audio {

	/** 
	*/
	class YAKE_AUDIO_API ISoundData
	{
	public:
		/** */
		enum LoopMode
		{
			SLM_LOOP_OFF,
			SLM_LOOP_ON,
			SLM_LOOP_BIDI
		};

		/** */
		enum ChannelMode
		{
			SCM_MONO,
			SCM_STEREO,
			SCM_AUTOSELECT
		};

		/** */
		enum DataMode
		{
			SDM_8BITS,
			SDM_16BITS,
			SDM_AUTOSELECT
		};

		virtual ~ISoundData() {}

		virtual void setLoopMode( const LoopMode loop ) = 0;
		virtual LoopMode getLoopMode() const = 0;
		//virtual getChannelMode() const = 0;
		//virtual getDataMode() const = 0;
	};

	/** Represents an emitter of sound.
	*/
	class YAKE_AUDIO_API ISource : public Movable
	{
	public:
		virtual ~ISource() {}
		virtual void setVelocity( const Vector3 & velocity ) = 0;
		virtual Vector3 getVelocity() const = 0;

		virtual void setSoundData( ISoundData* pSoundData ) = 0;
		//virtual void setSoundData( const String & resourceName ) = 0;
		virtual void play() = 0;
		virtual void stop() = 0;
		virtual void pause() = 0;
	};

	/** A listener in a world.
	*/
	class YAKE_AUDIO_API IListener : public Movable
	{
	public:
		virtual ~IListener() {}
		virtual void setVelocity( const Vector3 & velocity ) = 0;
		virtual Vector3 getVelocity() const = 0;
	};

	/** Represents an environment in which an audio simulation takes place.
		It also is responsible for creating and managing audio sources and listeners.
	*/
	class YAKE_AUDIO_API IWorld
	{
	public:
		virtual ~IWorld() {}

		virtual ISoundData* createSoundDataFromFile( const String & filename ) = 0;
		/*virtual ISoundData* createSoundData(	bool b3D,
												ISoundData::LoopMode loopMode = ISoundData::SLM_LOOP_OFF, 
												ISoundData::ChannelMode channelMode = ISoundData::SCM_AUTOSELECT,
												ISoundData::DataMode dataMode = ISoundData::SDM_16BITS) = 0;*/

		/** Create a listener and return a pointer to it. Ownership remains with the creator.
		*/
		virtual IListener* createListener() = 0;

		/** Create an audio source. Ownership remains with the creator.
		*/
		virtual ISource* createSource() = 0;

		virtual void setActiveListener( IListener* pListener ) = 0;

		virtual void setDopplerFactor( real factor ) = 0;
		virtual void setSoundVelocity( real velocity ) = 0;
	};

	/** Audio system interface.
	*/
	class YAKE_AUDIO_API IAudioSystem
	{
	public:
		virtual ~IAudioSystem();
		virtual IWorld* createWorld() = 0;

		YAKE_DECLARE_REGISTRY_0( IAudioSystem, yake::String )
	};

}
}

#endif

