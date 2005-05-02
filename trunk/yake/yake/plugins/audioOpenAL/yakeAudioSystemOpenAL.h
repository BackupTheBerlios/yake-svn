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
#ifndef INC_YAKE_AUDIOSYSTEMOPENAL_H
#define INC_YAKE_AUDIOSYSTEMOPENAL_H

#ifdef YAKE_AUDIO_OPENAL_EXPORTS
#	pragma message("Exporting yake::audio::openalpp")
#	define YAKE_AUDIO_OPENAL_API DLLEXPORT
#else
#	pragma message("Importing yake::audio::openalpp")
#	define YAKE_AUDIO_OPENAL_API DLLIMPORT
#endif

namespace yake {
	using namespace base;
	using namespace base::templates;
	using namespace math;
namespace audio {

	class SoundDataOpenALBase : public ISoundData
	{
	public:
		enum Type {
			kSample,
			kStream
		};
	protected:
		SoundDataOpenALBase( Type type, LoopMode loop ) :
			 mSoundData(0), mType(type), mLoopMode(loop)
			{
			}
	public:
		virtual ~SoundDataOpenALBase()
		{
		}

		virtual void setLoopMode( const LoopMode loop )
		{ mLoopMode = loop; }
		virtual LoopMode getLoopMode() const
		{ return mLoopMode; }

		Type getType_() const
		{ return mType; }

		openalpp::ref_ptr<openalpp::SoundData> getSoundData_() const
		{ 
			YAKE_ASSERT( mSoundData.valid() );
			return mSoundData; 
		}

	protected:
		openalpp::ref_ptr<openalpp::SoundData>	mSoundData;
		Type					mType;
		LoopMode				mLoopMode;
	};

	class SoundDataFile : public SoundDataOpenALBase
	{
	public:
		SoundDataFile( const String & filename );
	};

	class SourceOpenAL : public ISource
	{
	public:
		SourceOpenAL();
		virtual ~SourceOpenAL();
		virtual void setVelocity( const Vector3 & velocity );
		virtual Vector3 getVelocity() const;

		virtual void setSoundData( ISoundData* pSoundData );
		//virtual void setSoundData( const String & resourceName );
		virtual void play();
		virtual void stop();
		virtual void pause();

		virtual void setOrientation( const Quaternion & orientation );
		virtual Quaternion getOrientation() const;
		virtual void setPosition( const Vector3 & position );
		virtual Vector3 getPosition() const;
	protected:
		openalpp::ref_ptr<openalpp::SourceBase>	mSource;
		SoundDataOpenALBase*	mSoundData;
		Quaternion				mOrientation;
	};

	class ListenerOpenAL : public IListener
	{
	public:
		ListenerOpenAL();
		virtual ~ListenerOpenAL();
		virtual void setVelocity( const Vector3 & velocity );
		virtual Vector3 getVelocity() const;

		virtual void setOrientation( const Quaternion & orientation );
		virtual Quaternion getOrientation() const;
		virtual void setPosition( const Vector3 & position );
		virtual Vector3 getPosition() const;

	protected:
		openalpp::ref_ptr<openalpp::Listener>		mListener;
		Quaternion				mCachedOrientation;
	};

	class WorldOpenAL : public IWorld
	{
	public:
		WorldOpenAL();
		virtual ~WorldOpenAL();

		virtual ISoundData* createSoundDataFromFile( const String & filename );
		/*virtual ISoundData* createSoundData(	bool b3D,
												ISoundData::LoopMode loopMode = ISoundData::SLM_LOOP_OFF, 
												ISoundData::ChannelMode channelMode = ISoundData::SCM_AUTOSELECT,
												ISoundData::DataMode dataMode = ISoundData::SDM_16BITS);*/

		virtual IListener* createListener();

		virtual ISource* createSource();

		virtual void setActiveListener( IListener* pListener );

		virtual void setDopplerFactor( real factor );
		virtual void setSoundVelocity( real velocity );

	protected:
		openalpp::ref_ptr<openalpp::AudioEnvironment> mEnv;
	};

	class AudioSystemOpenAL : public IAudioSystem
	{
	public:
		AudioSystemOpenAL();
		virtual ~AudioSystemOpenAL();
		virtual IWorld* createWorld();

		YAKE_DECLARE_CONCRETE( AudioSystemOpenAL, "openalpp" )
	};

}
}

#endif