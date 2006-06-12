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

#ifndef YAKE_MODEL_PHYSICAL_XODE_LOADER_H
#define YAKE_MODEL_PHYSICAL_XODE_LOADER_H

#include <yake/model/prerequisites.h>
#include <yake/physics/yakePhysics.h>
#include <yake/loader/yakeXODEParser.h>

namespace yake {
namespace model {

    /** Class implements DotSceneParser's listener. Creates graphical objects and places
     * them in model::Graphical object.
     */
    class YAKE_MODEL_API XODEListener
    {
	public:
	    XODEListener( Physical& owner, physics::IWorld* physWorld, const String& namePrefix = _T("") ) : 
			owner_( owner ), 
			physicalWorld_( physWorld ),
			namePrefix_(namePrefix)
	    {}

	    virtual ~XODEListener() {}		

		typedef yake::data::parser::xode::XODEParser parser_t;

	    virtual void processBody( const parser_t::BodyDesc& desc );
	    virtual void processGeom( const parser_t::GeomDesc& desc );
	    virtual void processMass( const parser_t::MassDesc& desc );
	    virtual void processJoint( const parser_t::JointDesc& desc );
	    virtual void processMaterial( const parser_t::MaterialDesc& desc );
	    virtual void postprocess();

		physics::IJoint* getJointByName( const String& name )
	    {
			JointMap::const_iterator it = joints_.find( name );
			return (it == joints_.end()) ? 0 : it->second.joint_;
	    }

	protected:

	    struct ActorInfo
	    {
		physics::IActor*		actor_;
		String					parentName_;
	    };
	    
	    struct GeomInfo
	    {
		physics::IShape*		shape_;
		String					parentName_;
	    };

	    struct MassInfo
	    {
		physics::IBody::MassDesc	mass_;
		String						parentName_;
	    };

	    struct JointInfo
	    {
		physics::IJoint*		joint_;
		String					parentName_;
	    };

	    struct MaterialInfo
	    {
		physics::IMaterial*		material_;
		String					parentName_;
	    };

	    typedef AssocVector< String, ActorInfo > ActorMap;
	    typedef AssocVector< String, GeomInfo > GeomMap;
	    typedef AssocVector< String, MassInfo > MassMap;
	    typedef AssocVector< String, JointInfo > JointMap;
	    typedef AssocVector< String, MaterialInfo > MaterialMap;
	    typedef Vector< typename parser_t::JointDesc > JointDescVector;

	    JointDescVector jointDescriptions_;

	    ActorMap	actors_;
	    GeomMap	geoms_;
	    MassMap	masses_;
	    JointMap	joints_;
	    MaterialMap materials_;

	    physics::IWorld*	physicalWorld_;

	    Physical&			owner_;
		String				namePrefix_;
    };


} // model
} // yake

#endif 

