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
#ifndef INC_YAKE_AUDIBLE_H
#define INC_YAKE_AUDIBLE_H

#include <yake/audio/yakeAudioPrerequisites.h>

namespace yake {
	using namespace base;
	using namespace base::templates;
namespace audio {
	/** High level audio object
	*/
	class YAKE_AUDIO_API Audible : public Movable
	{
	private:
		Audible( const Audible & );
	public:
		Audible();
		virtual ~Audible();

		virtual void setSource( SharedPtr<audio::ISource> & pSource ) = 0;

		virtual void startPlaying();
		virtual bool isPlaying();
		virtual void stopPlaying();
	};
}
}
#endif

