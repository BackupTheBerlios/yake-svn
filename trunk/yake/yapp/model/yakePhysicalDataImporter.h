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

#ifndef YAPP_MODEL_PHYSICAL_DATA_IMPORTER_H
#define YAPP_MODEL_PHYSICAL_DATA_IMPORTER_H

#include <yapp/base/yappPrerequisites.h>
#include <yapp/model/yakeModel.h>
#include <yapp/loader/yakeXODEParser.h>
#include <yake/physics/yakePhysics.h>

namespace yake {
namespace model {

    /** Class implements XODEParser's listener. Creates physical objects and places
     * them in model::Physical object.
     */
    
    using namespace yake::data::parser::xode;
    using namespace yake::physics;
    
    class XODEListener
    {

	public:
	    XODEListener( Physical& owner, physics::IWorld* physWorld ) : owner_( owner ), physicalWorld_( physWorld )
	    {}

	    virtual ~XODEListener() {}		

	    void processBody( const XODEParser::BodyDesc& desc );
	    void processGeom( const XODEParser::GeomDesc& desc );
	    void processMass( const XODEParser::MassDesc& desc );
	    void processJoint( const XODEParser::JointDesc& desc );
	    void processMaterial( const XODEParser::MaterialDesc& desc );
	    void postprocess();

	private:

	    struct ActorInfo
	    {
		IActor*	actor_;
		String	parentName_;
	    };
	    
	    struct GeomInfo
	    {
		IShape*	shape_;
		String	parentName_;
	    };

	    struct MassInfo
	    {
		IBody::MassDesc	mass_;
		String	parentName_;
	    };

	    struct JointInfo
	    {
		IJoint*	    joint_;
		String	    parentName_;
	    };

	    struct MaterialInfo
	    {
		IMaterial*  material_;
		String	    parentName_;
	    };

	    typedef AssocVector< String, ActorInfo > ActorMap;
	    typedef AssocVector< String, GeomInfo > GeomMap;
	    typedef AssocVector< String, MassInfo > MassMap;
	    typedef AssocVector< String, JointInfo > JointMap;
	    typedef AssocVector< String, MaterialInfo > MaterialMap;
	    typedef Vector< XODEParser::JointDesc > JointDescVector;

	    JointDescVector jointDescriptions_;

	    ActorMap	actors_;
	    GeomMap	geoms_;
	    MassMap	masses_;
	    JointMap	joints_;
	    MaterialMap materials_;

	    physics::IWorld*	physicalWorld_;

	    Physical&		owner_;
    };


} // model
} // yake

#endif 

