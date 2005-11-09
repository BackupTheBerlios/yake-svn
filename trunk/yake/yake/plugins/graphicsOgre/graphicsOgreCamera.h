/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright  2004 The YAKE Team
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
#ifndef INC_YAKE_GRAPHICOGRECAMERA_H
#define INC_YAKE_GRAPHICOGRECAMERA_H

namespace yake {
namespace graphics {
namespace ogre3d {

	class OgreCamera : public ICamera, public OgreWrappedObject
	{
		OgreCamera();
		OgreCamera( const OgreCamera & );
	public:
		OgreCamera( GraphicalWorld& owningWorld, Ogre::SceneManager * sceneMgr );
		virtual ~OgreCamera();

		// Movable
		virtual void setPosition( const Vector3 & position );
		virtual Vector3 getPosition() const;
		virtual void setOrientation( const Quaternion & orientation );
		virtual Quaternion getOrientation() const;
		
		void setDetailLevel(SceneDetailLevel sd);

        /** Retrieves the level of detail that the camera will render.
		 */
		SceneDetailLevel getDetailLevel(void) const;

        /** Sets the camera's direction vector.
		@remarks
		Note that the 'up' vector for the camera will automatically be recalculated based on the
		current 'up' vector (i.e. the roll will remain the same).
				 */
		virtual void setDirection( const Vector3& rVec );

        /** Gets the camera's direction.
		 */
		virtual Vector3 getDirection() const;

        /** Gets the camera's up vector.
		 */
		virtual Vector3 getUp() const;

        /** Gets the camera's right vector.
		 */
		virtual Vector3 getRight() const;
		
		virtual void setFixedYawEnabled( bool enabled );
		virtual void setFixedYawAxis( const Vector3& yawAxis );
	
		/** Moves movable's position by the vector offset provided along it's own axes (relative to orientation).
		 */
		virtual void moveRelative( const Vector3& rVec );
		virtual void lookAt( const Vector3& target );
		virtual void yaw( const real degrees );
		virtual void roll( const  real degrees );


		// ICamera
		virtual void setProjectionType( const ProjectionType type );
		virtual ProjectionType getProjectionType() const;
		virtual Matrix4 getProjectionMatrix() const;
		virtual Matrix4 getViewMatrix() const;
		virtual void setNearClipDistance( real clipDistance );
		virtual real getNearClipDistance() const;
		virtual void setFarClipDistance( real clipDistance );
		virtual real getFarClipDistance() const;
		virtual void setFOV( real fov );
		virtual real getFOV() const;
		virtual void setAspectRatio( real aspectRatio );
		virtual real getAspectRatio() const;

		Ogre::Camera* getCamera_() const
		{ return mCam; }
	protected:
		Ogre::SceneManager*	mSceneMgr;
		Ogre::Camera*		mCam;
	};

}
}
}

#endif
