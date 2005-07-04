/**------------------------------------------------------------------------------------
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
*/
#include <yake/base/yake.h>
#include <yake/plugins/graphicsLuaBindings/yakePrerequisites.h>
#include <yake/scripting/yakeScriptingSystem.h>
#include <yake/plugins/scriptingLua/ScriptingSystemLua.h>
#include <yake/plugins/graphicsLuaBindings/yakeLuaBinder.h>
#include <luabind/luabind.hpp>

#include <yake/graphics/yakeGraphics.h>
#include <yake/graphics/yakeGeometryAccess.h>

namespace yake
{
	namespace graphics
	{
		namespace lua
		{

			void Binder::bind( scripting::IVM* pVM )
			{
				scripting::LuaVM* pL = static_cast<scripting::LuaVM*>(pVM);
				YAKE_ASSERT( pL );
				if (!pL)
					return;

				using namespace luabind;
				using namespace yake::graphics;

#define YAKE_GRAPHICS_MODULE pL->getLuaState(), "yake"

				module( YAKE_GRAPHICS_MODULE )
				[
				    class_<IParticleSystem>( "IParticleSystem" )
				    .def( "setVisible", &IParticleSystem::setVisible )
				];

				module( YAKE_GRAPHICS_MODULE )
				[
				    class_<ISkeleton>( "ISkeleton" )
				    .def( "enableAnimation", &ISkeleton::enableAnimation )
				    .def( "advanceAnimation", &ISkeleton::advanceAnimation )
				    .def( "advanceAllAnimations", &ISkeleton::advanceAllAnimations )
				    .def( "setAnimationWeight", &ISkeleton::setAnimationWeight )
				    .def( "attachEntityToBone", &ISkeleton::attachEntityToBone )
				];

				module( YAKE_GRAPHICS_MODULE )
				[
				    class_<ILight>( "ILight" )
				    .enum_( "LightType" )
				    [
				        value("LT_POINT", ILight::LT_POINT ),
				        value("LT_SPOT", ILight::LT_SPOT ),
				        value("LT_DIRECTIONAL", ILight::LT_DIRECTIONAL )
				    ]
				    .def( "setType", &ILight::setType )
				    .def( "getType", &ILight::getType )
				    .def( "setEnabled", &ILight::setEnabled )
				    .def( "setCastsShadows", &ILight::setCastsShadows )
				    .def( "setDiffuseColour", &ILight::setDiffuseColour )
				    .def( "setSpecularColour", &ILight::setSpecularColour )
				    .def( "setAttenuation", &ILight::setAttenuation )
				    .def( "getAttenuationRange", &ILight::getAttenuationRange )
				    .def( "getAttenuationConstFactor", &ILight::getAttenuationConstFactor )
				    .def( "getAttenuationLinearFactor", &ILight::getAttenuationLinearFactor )
				    .def( "getAttenuationQuadraticFactor", &ILight::getAttenuationQuadraticFactor )
				    .def( "setSpotlightRange", &ILight::setSpotlightRange )
				    .def( "getSpotlightInnerAngle", &ILight::getSpotlightInnerAngle )
				    .def( "getSpotlightOuterAngle", &ILight::getSpotlightOuterAngle )
				    .def( "getSpotlightFalloff", &ILight::getSpotlightFalloff )
				    .def( "setDirection", &ILight::setDirection )
				    .def( "getDirection", &ILight::getDirection )
				];

				module( YAKE_GRAPHICS_MODULE )
				[
				    class_<IEntity>( "IEntity" )
				    .def( "getSkeleton", &IEntity::getSkeleton )
				    .def( "setVisible", &IEntity::setVisible )
				    .def( "setMaterial", &IEntity::setMaterial )
				    .def( "setSubEntityMaterial", &IEntity::setSubEntityMaterial )
				    .def( "setCastsShadow", &IEntity::setCastsShadow )
				    .def( "getName", (String(IEntity::*)() const)&IEntity::getName )

				];

				module( YAKE_GRAPHICS_MODULE )
				[
				    class_<ICamera, Movable>( "ICamera" )
				    .enum_( "SceneDetailLevel" )
				    [
				        value( "SDL_POINTS", ICamera::SDL_POINTS ),
				        value( "SDL_WIREFRAME", ICamera::SDL_WIREFRAME ),
				        value( "SDL_SOLID", ICamera::SDL_SOLID )
				    ]
				    .enum_( "ProjectionType" )
				    [
				        value( "PT_ORTHOGRAPHIC", ICamera::PT_ORTHOGRAPHIC ),
				        value( "PT_PERSPECTIVE", ICamera::PT_PERSPECTIVE )
				    ]
				    .def( "setProjectionType", &ICamera::setProjectionType )
				    .def( "getProjectionType", &ICamera::getProjectionType )
				    .def( "setDetailLevel", &ICamera::setDetailLevel )
				    .def( "getDetailLevel", &ICamera::getDetailLevel )
				    .def( "setNearClipDistance", &ICamera::setNearClipDistance )
				    .def( "getNearClipDistance", &ICamera::getNearClipDistance )
				    .def( "setFarClipDistance", &ICamera::setFarClipDistance )
				    .def( "setFOV", &ICamera::setFOV )
				    .def( "getFOV", &ICamera::getFOV )
				    .def( "setAspectRatio", &ICamera::getAspectRatio )
				    .def( "setDirection", &ICamera::setDirection )
				    .def( "getDirection", &ICamera::getDirection )
				    .def( "getUp", &ICamera::getUp )
				    .def( "getRight", &ICamera::getRight )
				    .def( "setFixedYawEnabled", &ICamera::setFixedYawEnabled )
				    .def( "setFixedYawAxis", &ICamera::setFixedYawAxis )
				    .def( "moveRelative", &ICamera::moveRelative )
				    .def( "lookAt", &ICamera::lookAt )
				    .def( "rotate", (void(ICamera::*)(const Quaternion&))&ICamera::rotate )
				    .def( "rotateAxisAngle", (void(ICamera::*)(const Vector3&, real))&ICamera::rotate )
				    .def( "pitch", &ICamera::pitch )
				    .def( "yaw", &ICamera::yaw )
				    .def( "createCameraToViewportRay", &ICamera::createCameraToViewportRay )
				    .def( "getProjectionMatrix", &ICamera::getProjectionMatrix )
				    .def( "getViewMatrix", &ICamera::getViewMatrix )
				];

				module( YAKE_GRAPHICS_MODULE )
				[
				    class_<ISceneNode, Movable>( "ISceneNode" )
				    .def( "addChildNode", &ISceneNode::addChildNode )
				    .def( "createChildNode", &ISceneNode::createChildNode )
				    .def( "attachEntity", &ISceneNode::attachEntity )
				    .def( "attachCamera", &ISceneNode::attachCamera )
				    .def( "attachLight", &ISceneNode::attachLight )
				    .def( "attachParticleSystem", &ISceneNode::attachParticleSystem )
				    .def( "removeAndDestroyAllChildren", &ISceneNode::removeAndDestroyAllChildren )
				    .def( "setScale", &ISceneNode::setScale )
				    .def( "getScale", &ISceneNode::getScale )
				    .def( "getChildren", (SceneNodePtrList(ISceneNode::*)(bool)const)&ISceneNode::getChildren )
				    .def( "getChildByName", &ISceneNode::getChildByName )
				    .def( "getAttachedEntities", &ISceneNode::getAttachedEntities )
				    .def( "getAttachedLights", &ISceneNode::getAttachedLights )
				    .def( "detachEntity", (void(ISceneNode::*)(IEntity*))&ISceneNode::detach )
				    .def( "detachLight", (void(ISceneNode::*)(ILight*))&ISceneNode::detach )
				    .def( "getName", (String(ISceneNode::*)()const)&ISceneNode::getName )
				];

				module( YAKE_GRAPHICS_MODULE )
				[
				    class_<IViewport>( "IViewport" )
				    .def( "attachCamera", &IViewport::attachCamera )
				    .def( "getAttachedCamera", &IViewport::getAttachedCamera )
				    .def( "setDimensions", &IViewport::setDimensions )
				    .def( "setZ", &IViewport::setZ )
				];

				module( YAKE_GRAPHICS_MODULE )
				[
				    class_<IWorld>( "IWorld" )
				    .def( "createLight", &IWorld::createLight )
				    .def( "createCamera", &IWorld::createCamera )
				    .def( "createSceneNode", &IWorld::createSceneNode )
				    .def( "createParticleSystem", &IWorld::createParticleSystem )
				    .def( "createEntity", &IWorld::createEntity )
				    .def( "createProceduralMesh", &IWorld::createProceduralMesh )
				    .def( "destroyProceduralMesh", &IWorld::destroyProceduralMesh )
				    .def( "getProceduralMesh", &IWorld::getProceduralMesh )
				    .def( "createViewport", &IWorld::createViewport )
				    .def( "getShadowTechniques", &IWorld::getShadowTechniques )
				    .def( "selectShadowTechnique", &IWorld::selectShadowTechnique )
				    .def( "setShadowsEnabled", &IWorld::setShadowsEnabled )
				    .def( "render", &IWorld::render )
				    .def( "getRenderWindowWidth", &IWorld::getRenderWindowWidth )
				    .def( "getRenderWindowHeight", &IWorld::getRenderWindowHeight )
				    .def( "pickEntity", &IWorld::pickEntity )
				];

				module( YAKE_GRAPHICS_MODULE )
				[
				    class_<IGraphicsSystem>( "IGraphicsSystem" )
				    .def( "shutdown", &IGraphicsSystem::shutdown )
				    .def( "createWorld", &IGraphicsSystem::createWorld )
				];

			}


		} // namespace lua
	} // namespace graphics
} // namespace yake
