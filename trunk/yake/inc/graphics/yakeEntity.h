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
#ifndef YAKE_GRAPHICSSYSTEM_H
#define YAKE_GRAPHICSSYSTEM_H

#include <yakeGraphics/inc/yakeGraphics.h>

namespace yake {

	using namespace base::math;

	namespace graphics {

		/** Represents a concrete instance of some kind of graphical entity,
			e.g. an instance of a mesh, a scene node, ...
		*/
		class YAKE_GRAPHICS_API GraphicsEntity
		{
		public:
			virtual ~GraphicsEntity() {}
		};

		class GraphicsSystem;
		/**
		*/
		class GraphicsPlugin : public yake::base::Plugin
		{
		public:
			virtual GraphicsSystem* createSystem() = 0;
		};

		class IEntity;

		/**
		*/
		class YAKE_GRAPHICS_API IParticleSystem : public GraphicsEntity
		{
		public:
			virtual void setVisible( bool visible ) = 0;
		};

		/**
		*/
		class YAKE_GRAPHICS_API ISkeleton : public GraphicsEntity
		{
		public:
			virtual void enableAnimation( const base::String & rAnimName, bool enable = true ) = 0;
			virtual void advanceAnimation( const base::String& rAnimName, real timeDelta ) = 0;
			virtual void advanceAllAnimations( real timeDelta ) = 0;
			virtual void setAnimationWeight( const base::String & rAnimName, real weight ) = 0;
			virtual void attachEntityToBone( const base::String & bone, IEntity* pEntity ) = 0;
		};

		/**
		*/
		class YAKE_GRAPHICS_API ILight : public GraphicsEntity
		{
		public:
			enum LightType 
			{
				LT_POINT,
				LT_DIRECTIONAL,
				LT_SPOT
			};

			virtual void setType( const LightType type ) = 0;
			virtual LightType getType() const = 0;

 			virtual void setCastsShadow( bool enabled ) = 0;

			virtual void setDiffuseColour( const Color & colour ) = 0;
			virtual void setSpecularColour( const Color & colour ) = 0;

			virtual void setAttenuation( real range, real constant, real linear, real quadratic ) = 0;
			virtual real getAttenuationRange() const = 0;
			virtual real getAttenuationConstFactor() const = 0;
			virtual real getAttenuationLinearFactor() const = 0;
			virtual real getAttenuationQuadraticFactor() const = 0;

			/** Spotlight spatial configuration
			*/
			virtual void setSpotlightRange( real innerAngle, real outerAngle, real falloff ) = 0;
			virtual real getSpotlightInnerAngle() const = 0; 
			virtual real getSpotlightOuterAngle() const = 0;			
			virtual real getSpotlightFalloff() const = 0;
			
			/** Synonims for set/getOrientation.  Meaningful only for directional and spot lights
			*/
			virtual void setDirection( const Vector3& rDirection ) = 0;
			virtual Vector3 getDirection() const = 0;
		};

		/** A concrete instance of a mesh in a graphical world.
		*/
		class YAKE_GRAPHICS_API IEntity : public GraphicsEntity
		{
		public:
			virtual ISkeleton* getSkeleton() const = 0;
			virtual void setVisible( bool visible ) = 0;
			virtual void setMaterial( const base::String & materialName ) = 0;
			virtual void setSubEntityMaterial( const base::String & subEntity, const base::String & materialName ) = 0;
			//virtual void setReceivesShadows( bool receivesShadows ) = 0;
			virtual void setCastsShadow( bool castsShadow ) = 0;
		};

		/** A concrete instance of a camera in a scene.
		*/
		class YAKE_GRAPHICS_API ICamera : public GraphicsEntity, public base::Movable
		{
		public:
			enum ProjectionType
			{
				PT_ORTHOGRAPHIC,
				PT_PERSPECTIVE
			};

			virtual void setProjectionType( const ProjectionType type ) = 0;
			virtual ProjectionType getProjectionType() const = 0;

			virtual void setNearClipDistance( real clipDistance ) = 0;
			virtual real getNearClipDistance() const = 0;
			virtual void setFarClipDistance( real clipDistance ) = 0;
			virtual real getFarClipDistance() const = 0;
			virtual void setFOV( real fov ) = 0;
			virtual real getFOV() const = 0;
			virtual void setAspectRatio( real aspectRatio ) = 0;
			virtual real getAspectRatio() const = 0;
		};

		/** A concrete node in a scene graph.
		*/
		class YAKE_GRAPHICS_API ISceneNode : public GraphicsEntity, public base::Movable
		{
		public:
			virtual void addChildNode( ISceneNode* pNode ) = 0;
			virtual void attachEntity( IEntity* pEntity ) = 0;
			virtual void attachCamera( ICamera* pCamera ) = 0;
			virtual void attachLight( ILight* pLight ) = 0;
			virtual void removeAndDestroyAllChildren() = 0;
		};

		/** Viewport abstract interface.
		*/
		class YAKE_GRAPHICS_API IViewport
		{
		public:
			virtual ~IViewport() {}
			virtual void attachCamera( ICamera* pCamera ) = 0;
			virtual ICamera* getAttachedCamera() const = 0;
			virtual void setDimensions( real left, real top, real width, real height ) = 0;
			virtual void setZ( int z ) = 0;
		};

		/** A graphics world.
		*/
		class YAKE_GRAPHICS_API GraphicalWorld
		{
		public:
			virtual ~GraphicalWorld() {}

			virtual ISceneNode* createSceneNode() = 0;
			virtual IEntity* createEntity( const base::String & mesh ) = 0;
			virtual ILight* createLight() = 0;

			virtual ICamera* createCamera() = 0;

			/** Creates viewport and assigns camera *pCamera to it.
				Viewport start offset and dimensions are in normalized window-relative coords [0..1].
			*/
			virtual IViewport* createViewport( ICamera * pCamera ) = 0;

			/** Globally activates the default shadowing method, often stencil shadows.
			*/
			virtual void setShadowsEnabled( bool enabled ) = 0;

			/** Render a single frame. */
			virtual void render( real timeElapsed ) = 0;
		};

		/** The graphics system interface.
		*/
		class YAKE_GRAPHICS_API GraphicsSystem// : public System
		{
		public:
			virtual ~GraphicsSystem() {}

			virtual GraphicalWorld* createWorld() = 0;
		};
	}
}


#endif

