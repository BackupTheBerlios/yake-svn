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
#ifndef INC_YAKE_GRAPHICOGRECAMERA_H
#define INC_YAKE_GRAPHICOGRECAMERA_H

namespace yake {
namespace graphics {
namespace ogre3d {

	class OgreCamera : public ICamera
	{
		OgreCamera();
		OgreCamera( const OgreCamera & );
	public:
		OgreCamera( Ogre::SceneManager * sceneMgr );
		virtual ~OgreCamera();

		// Movable
		virtual void setPosition( const Vector3 & position );
		virtual Vector3 getPosition() const;
		virtual void setOrientation( const Quaternion & orientation );
		virtual Quaternion getOrientation() const;

		// ICamera
		virtual void setProjectionType( const ProjectionType type );
		virtual ProjectionType getProjectionType() const;
		virtual void setNearClipDistance( real clipDistance );
		virtual real getNearClipDistance() const;
		virtual void setFarClipDistance( real clipDistance );
		virtual real getFarClipDistance() const;
		virtual void setFOV( real fov );
		virtual real getFOV() const;
		virtual void setAspectRatio( real aspectRatio );
		virtual real getAspectRatio() const;

		Ogre::Camera * getCamera_() const
		{ return mCam; }
	protected:
		Ogre::SceneManager	* mSceneMgr;
		Ogre::Camera		* mCam;
	};

}
}
}

#endif
