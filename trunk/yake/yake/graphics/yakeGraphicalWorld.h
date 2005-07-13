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
#ifndef YAKE_GRAPHICALWORLD_H
#define YAKE_GRAPHICALWORLD_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/graphics/yakeGraphics.h>
#include <yake/graphics/yakeGraphicsSystem.h>
#include <yake/graphics/yakeEntity.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================


namespace yake {
	using namespace math;
	using namespace base::templates;
namespace graphics {

	/**
	*/
	class YAKE_GRAPHICS_INTERFACE_API IParticleSystem : public GraphicsEntity
	{
	public:
		virtual ~IParticleSystem();
		virtual void setVisible( bool visible ) = 0;
	};

	class IEntity;
	/**
	*/
	class YAKE_GRAPHICS_INTERFACE_API ISkeleton : public GraphicsEntity
	{
	public:
		virtual ~ISkeleton();
		virtual void enableAnimation( const String& rAnimName, bool enable = true ) = 0;
		virtual void advanceAnimation( const String& rAnimName, real timeDelta ) = 0;
		virtual void advanceAllAnimations( real timeDelta ) = 0;
		virtual void setAnimationWeight( const String& rAnimName, real weight ) = 0;
		virtual void attachEntityToBone( const String& bone, IEntity* pEntity ) = 0;
	};

	/**
	*/
	class YAKE_GRAPHICS_INTERFACE_API ILight : public GraphicsEntity
	{
	public:
		enum LightType
		{
			LT_POINT,
			LT_DIRECTIONAL,
			LT_SPOT
		};

		virtual ~ILight();
		
		virtual void setType( const LightType type ) = 0;
		virtual LightType getType() const = 0;

		virtual void setEnabled( bool enabled ) = 0;
 		virtual void setCastsShadows( bool enabled ) = 0;

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
	class YAKE_GRAPHICS_INTERFACE_API IEntity : public GraphicsEntity
	{
	public:
		virtual ~IEntity();
		
		virtual ISkeleton* getSkeleton() const = 0;
		virtual void setVisible( bool visible ) = 0;
		virtual void setMaterial( const String & materialName ) = 0;
		virtual void setSubEntityMaterial( const String & subEntity, const String & materialName ) = 0;
		//virtual void setReceivesShadows( bool receivesShadows ) = 0;
		virtual void setCastsShadow( bool castsShadow ) = 0;

		//@todo move into base class!?
		virtual String getName() const = 0;
		virtual void getName(String& name) = 0;
	};

	/** A concrete instance of a camera in a scene.
	*/
	class YAKE_GRAPHICS_INTERFACE_API ICamera : public GraphicsEntity, public Movable
	{
	public:
		enum SceneDetailLevel
		{
		/// Only points are rendered.
			SDL_POINTS = 1,
		/// Wireframe models are rendered.
			SDL_WIREFRAME = 2,
		/// Solid polygons are rendered.
			SDL_SOLID = 3
		};

		enum ProjectionType
		{
			PT_ORTHOGRAPHIC,
			PT_PERSPECTIVE
		};

		virtual ~ICamera();
		
		virtual void setProjectionType( const ProjectionType type ) = 0;
		virtual ProjectionType getProjectionType() const = 0;

        /** Sets the level of rendering detail required from this camera.
		@remarks
		Each camera is set to render at full detail by default, that is
		with full texturing, lighting etc. This method lets you change
		that behaviour, allowing you to make the camera just render a
		wireframe view, for example.
		 */
		virtual void setDetailLevel(SceneDetailLevel sd) = 0;

        /** Retrieves the level of detail that the camera will render.
		 */
		virtual SceneDetailLevel getDetailLevel(void) const = 0;

		
		virtual void setNearClipDistance( real clipDistance ) = 0;
		virtual real getNearClipDistance() const = 0;
		virtual void setFarClipDistance( real clipDistance ) = 0;
		virtual real getFarClipDistance() const = 0;
		virtual void setFOV( real fov ) = 0;
		virtual real getFOV() const = 0;
		virtual void setAspectRatio( real aspectRatio ) = 0;
		virtual real getAspectRatio() const = 0;
		
        /** Sets the camera's direction vector.
		@remarks
		Note that the 'up' vector for the camera will automatically be recalculated based on the
		current 'up' vector (i.e. the roll will remain the same).
		 */
		virtual void setDirection( const Vector3& rVec ) = 0;

        /** Gets the camera's direction.
		*/
		virtual Vector3 getDirection() const = 0;

        /** Gets the camera's up vector.
		 */
		virtual Vector3 getUp() const = 0;

        /** Gets the camera's right vector.
		 */
		virtual Vector3 getRight() const = 0;
		
		virtual void setFixedYawEnabled( bool enabled ) = 0;
		virtual void setFixedYawAxis( const Vector3& yawAxis ) = 0;
	
		/** Moves movable's position by the vector offset provided along it's own axes (relative to orientation).
	 	*/
		virtual void moveRelative( const Vector3& rVec ) = 0;
		virtual void lookAt( const Vector3& target ) = 0;

		inline void rotate( const Quaternion& q );
		inline void rotate( const Vector3& axis, real degrees );
		inline void pitch( const real degrees );
		virtual void yaw( const real degrees ) = 0;

		/** Return a ray in world space coordinates as cast from the camera through a viewport
			position.
			@param screenX, screenY position where the ray intersects the viewport, in normalised screen coords [0,1]
		*/
		virtual Ray createCameraToViewportRay(const real screenX, const real screenY) const;

		/** Retrieve the projection matrix. The matrix conforms to the right-handed rules.
		*/
		virtual Matrix4 getProjectionMatrix() const = 0;
		virtual Matrix4 getViewMatrix() const = 0;
	};

	void ICamera::rotate(const Vector3 & axis, real degrees)
	{
		Quaternion q;
		q.FromAngleAxis(Math::AngleUnitsToRadians(degrees),axis);
		rotate(q);
	}
	void ICamera::rotate(const Quaternion & q)
	{
		setOrientation( q * getOrientation() );
	}
	void ICamera::pitch( const real degrees )
	{
		const Vector3 xAxis = getOrientation() * Vector3::kUnitX;
		rotate(xAxis, degrees);
	}

	class ISceneNode;
	typedef std::deque<ISceneNode*> SceneNodePtrList;
	typedef std::deque<IEntity*> EntityPtrList;
	typedef std::deque<ILight*> LightPtrList;
	typedef std::deque<ICamera*> CameraPtrList;
	typedef std::deque<IParticleSystem*> ParticleSystemPtrList;
	/** A concrete node in a scene graph.
	*/
	class YAKE_GRAPHICS_INTERFACE_API ISceneNode : public GraphicsEntity, public Movable
	{
	public:
		virtual ~ISceneNode();

		// Shamelessly ripped from Ogre...
		enum TransformSpace
		{
			TS_LOCAL,
			TS_PARENT,
			TS_WORLD
		};
		/** Adds a node as a child to this node.
		*/
		virtual void addChildNode( ISceneNode* pNode ) = 0;

		/** Creates a new scene (optionally named) node as a child to this node. 
			@param name Optional name of the new child scene node.
		*/
		virtual ISceneNode* createChildNode( const String& name = "" ) = 0;

		/** Attaches an entity to this node.
			Ownership is transferred to this node.
		*/
		virtual void attachEntity( IEntity* pEntity ) = 0;

		/** Attaches a camera to this node.
			Ownership is transferred to this node.
		*/
		virtual void attachCamera( ICamera* pCamera ) = 0;

		/** Attaches a light to this node.
			Ownership is transferred to this node.
		*/
		virtual void attachLight( ILight* pLight ) = 0;

		/** Attaches a particle system to this node.
			Ownership is transferred to this node.
		*/
		virtual void attachParticleSystem( IParticleSystem* pParticleSys ) = 0;

		/** Removes and destroys all child scene nodes and all their attached objects
			like cameras, entities, lights and particle systems.
		*/
		virtual void removeAndDestroyAllChildren() = 0;

		/** Set the scale for this node relative to the local coordinate system.
		*/
		virtual void setScale( const Vector3 & scale ) = 0;

		virtual Vector3 getScale() const = 0;

		/** Returns a list of all child scene nodes of this scene node.
		*/
		virtual void getChildren( SceneNodePtrList& ret, bool bRecursive = false ) const = 0;

		/** Returns a list of all child scene nodes of this scene node.
		*/
		SceneNodePtrList getChildren(bool bRecursive = false) const
		{ SceneNodePtrList ret; getChildren(ret, bRecursive); return ret; }

		/** Find & return a child scene node by its name. 
		*/
		virtual ISceneNode* getChildByName( const String& name, bool bRecursive = false ) = 0;

		/** Return a list of all attached entities. 
		*/
		virtual const EntityPtrList& getEntities() const = 0;

		/** Return a list of all attached lights. 
		*/
		virtual const LightPtrList& getLights() const = 0;

		/** Return a list of all attached cameras. 
		*/
		virtual const CameraPtrList& getCameras() const = 0;

		/** Return a list of all attached particle systems 
		*/
		virtual const ParticleSystemPtrList& getParticleSystems() const = 0;

		/** Detaches a previously attached entity.
			Ownership of the object is transferred to the caller.
		*/
		virtual void detach( IEntity* pEntity ) = 0;

		/** Detaches a previously attached light.
			Ownership of the object is transferred to the caller.
		*/
		virtual void detach( ILight* pLight ) = 0;

		/** Detaches a previously attached camera.
			Ownership of the object is transferred to the caller.
		*/
		virtual void detach( ICamera* pCamera ) = 0;

		/** Detaches a previously attached particle system.
			Ownership of the object is transferred to the caller.
		*/
		virtual void detach( IParticleSystem* pPS ) = 0;

		/** Return the name of this scene node.
		*/
		virtual String getName() const = 0;

		/** Return the name of this scene node.
		*/
		virtual void getName(String& name) = 0;

		//virtual void setName(const String& name) = 0;

		/** Return the position as transformed through the hierarchy of nodes.
		*/
		virtual Vector3 getDerivedPosition() const = 0;

		/** Return the position as transformed through the hierarchy of nodes.
		*/
		virtual void getDerivedPosition( Vector3& retPos ) const = 0;

		/** Return the orientation as transformed through the hierarchy of nodes.
		*/
		virtual Quaternion getDerivedOrientation() const = 0;

		/** Return the orientation as transformed through the hierarchy of nodes.
		*/
		virtual void getDerivedOrientation( Quaternion& retRot ) const = 0;

		/** Translate this scene node by a given amount.
			@param rDelta amount of translation.
			@param relativeTo TransformSpace to use for the translation.
		*/
		virtual void translate( const Vector3& rDelta, const TransformSpace relativeTo = TS_PARENT ) = 0;

		/** Rotate this scene node by a given amount.
			@param rDelta amount of rotation.
			@param relativeTo TransformSpace to use for the rotation.
		*/
		virtual void rotate( const Quaternion& rDelta, const TransformSpace relativeTo = TS_PARENT ) = 0;
	};

	/** Viewport abstract interface.
	*/
	class YAKE_GRAPHICS_INTERFACE_API IViewport
	{
	public:
		virtual ~IViewport();
		virtual void attachCamera( ICamera* pCamera ) = 0;
		virtual ICamera* getAttachedCamera() const = 0;
		virtual void setDimensions( real left, real top, real width, real height ) = 0;
		virtual void setZ( int z ) = 0;
	};

	class IMeshGeometryAccess;

	typedef Deque<String> StringVector;
	typedef std::map<String,String> StringMap;

	/** A graphics world.
	*/
	class YAKE_GRAPHICS_INTERFACE_API IWorld
	{
	//{ //: public AbstractFactory< list< ISceneNode, ILight, ICamera > >
	public:
		virtual ~IWorld();

		virtual ILight* createLight() = 0;
		virtual ICamera* createCamera() = 0;

		virtual ISceneNode* createSceneNode( const String& name = "" ) = 0;

		virtual IParticleSystem* createParticleSystem( const String& rPSTemplateName ) = 0;

		/** Create an (placeable) instance of a mesh.
			\param mesh Can either be a mesh file loaded from a resource or a procedural mesh.
		*/
		virtual IEntity* createEntity( const String& mesh ) = 0;

		virtual IMeshGeometryAccess* createProceduralMesh(const String& name) = 0;
		virtual void destroyProceduralMesh(const String& name) = 0;
		virtual IMeshGeometryAccess* getProceduralMesh(const String& name) = 0;

		/** Creates viewport and assigns camera *pCamera to it.
				Viewport start offset and dimensions are in normalized window-relative coords [0..1].
		*/
		virtual IViewport* createViewport( ICamera* pCamera ) = 0;

		virtual StringVector getShadowTechniques() const = 0;
		virtual bool selectShadowTechnique(const String& name, const StringMap& params) = 0;
		/** Globally activates the default shadowing method, often stencil shadows.	*/
		virtual void setShadowsEnabled( bool enabled ) = 0;

		/** Render a single frame. */
		virtual void render( real timeElapsed ) = 0;

		virtual real getRenderWindowWidth() const = 0;
		virtual real getRenderWindowHeight() const = 0;

		virtual IEntity* pickEntity(const Ray& ray) = 0;
	};

} // graphics
} // yake

#endif // YAKE_GRAPHICALWORLD_H
