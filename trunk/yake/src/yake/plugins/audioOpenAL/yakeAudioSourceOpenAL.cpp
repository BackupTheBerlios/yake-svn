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
#include <yake/plugins/audioOpenAL/yakePCH.h>
#include <yake/plugins/audioOpenAL/yakeAudioSystemOpenAL.h>

namespace yake {
namespace audio {

	SoundDataFile::SoundDataFile(const String & filename) :
			SoundDataOpenALBase(kSample,SLM_LOOP_ON)
	{
		mSoundData = new openalpp::Sample( filename.c_str() );
		YAKE_ASSERT( mSoundData );
	}

	SourceOpenAL::SourceOpenAL() : mSoundData( 0 )
	{
		mSource = new openalpp::Source(); //TODO: OpenAL++ source groups!
	}

	SourceOpenAL::~SourceOpenAL()
	{
		YAKE_SAFE_DELETE( mSource );
		mSoundData = 0;
	}
	
	void SourceOpenAL::setOrientation( const Quaternion & orientation )
	{
		YAKE_ASSERT( mSource );
		mOrientation = orientation;
		Vector3 direction = mOrientation * Vector3(0,0,1);
		mSource->setDirection( direction.x, direction.y, direction.z );
	}
	Quaternion SourceOpenAL::getOrientation() const
	{
		YAKE_ASSERT( mSource );
		return mOrientation;
	}
	void SourceOpenAL::setPosition( const Vector3 & position )
	{
		YAKE_ASSERT( mSource );
		mSource->setPosition( position.x, position.y, position.z );
	}
	Vector3 SourceOpenAL::getPosition() const
	{
		YAKE_ASSERT( mSource );
		float x,y,z;
		mSource->getPosition(x,y,z);
		return Vector3(x,y,z);
	}

	void SourceOpenAL::setVelocity( const Vector3 & velocity )
	{
		YAKE_ASSERT( mSource );
		mSource->setVelocity( velocity.x, velocity.y, velocity.z );
	}

	Vector3 SourceOpenAL::getVelocity() const
	{
		YAKE_ASSERT( mSource );
		float x,y,z;
		mSource->getVelocity( x, y, z );
		return Vector3( x, y, z );
	}

	void SourceOpenAL::setSoundData( ISoundData* pSoundData )
	{
		YAKE_ASSERT( pSoundData ).debug("Need a valid pointer to sound data!");
		if (!pSoundData)
			return;
		mSoundData = static_cast< SoundDataOpenALBase* >( pSoundData );
		YAKE_ASSERT( mSoundData ).debug("Very bad! Need sound data matching to audio system!");
		if (!mSoundData)
			return;

		if (mSoundData->getType_() == SoundDataOpenALBase::kStream)
			static_cast<openalpp::Source*>(mSource)->setSound( * static_cast< openalpp::Stream* >( mSoundData->getSoundData_() ) );
		else
			if (mSoundData->getType_() == SoundDataOpenALBase::kSample)
				static_cast<openalpp::Source*>(mSource)->setSound( * static_cast< openalpp::Sample* >( mSoundData->getSoundData_() ) );

		if (mSoundData->getLoopMode() == ISoundData::SLM_LOOP_ON)
			mSource->setLooping( true );
		else
			mSource->setLooping( false );
	}

	/*void SourceOpenAL::setSoundData( const String & resourceName )
	{
		YAKE_SAFE_DELETE( mSoundData );
		setSoundData( 0 );
		static_cast<openalpp::Source*>(mSource)->setSound(resourceName.c_str());
	}*/

	void SourceOpenAL::play()
	{
		YAKE_ASSERT( mSource );
		mSource->play();
	}

	void SourceOpenAL::stop()
	{
		YAKE_ASSERT( mSource );
		mSource->stop();
	}

	void SourceOpenAL::pause()
	{
		YAKE_ASSERT( mSource );
		mSource->pause();
	}

}
}