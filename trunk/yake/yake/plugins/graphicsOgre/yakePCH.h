/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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
#ifndef __INC_PCH_H__
#define __INC_PCH_H__

#include <yake/base/yake.h>

#include <yake/graphics/yakeGraphicsSystem.h>

#include <Ogre.h>
#include <OgreStringConverter.h>
#include <OgreRoot.h>
#include <OgreConfigFile.h>
#include <OgreRenderSystem.h>
#include <OgreSceneManager.h>
#include <OgreNoMemoryMacros.h>

#define VEC_YAKE2OGRE( YV ) Ogre::Vector3( YV.x, YV.y, YV.z )
#define VEC_OGRE2YAKE( YV ) yake::Vector3( YV.x, YV.y, YV.z )

#define QUAT_YAKE2OGRE( q ) Ogre::Quaternion( q.w, q.x, q.y, q.z )
#define QUAT_OGRE2YAKE( q ) yake::Quaternion( q.w, q.x, q.y, q.z )

#define COL_YAKE2OGRE( c ) Ogre::ColourValue( c.r, c.g, c.b, c.a )
#define COL_OGRE2YAKE( c ) yake::Color( c.r, c.g, c.b, c.a )

#define MATRIX4_OGRE2YAKE( m ) yake::Matrix4( \
				m[0][0], m[0][1], m[0][2], m[0][3], \
				m[1][0], m[1][1], m[1][2], m[1][3], \
				m[2][0], m[2][1], m[2][2], m[2][3], \
				m[3][0], m[3][1], m[3][2], m[3][3] )

#endif // __INC_PCH_H__
