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
#ifndef YAPP_MODEL_COMPLEX_H
#define YAPP_MODEL_COMPLEX_H

#include <yapp/base/yappPrerequisites.h>
#include <yapp/model/yakeGraphical.h>
#include <yapp/model/yakePhysical.h>


namespace yake {
	using namespace base::templates;
namespace app {
namespace model {

	/*class YAPP_BASE_API Entity : public DynamicReflectionObject
	{
	};*/

	/*class YAPP_BASE_API ModelBase
	{
	public:
	};*/

	class ModelLink;

namespace complex {

	/** Represents a complex high-level model.
		It's both a container and a minimal controller of high-level graphical
		and physical models.
		@Remarks This is work-in-progress.
	*/
	class YAPP_BASE_API Model
	{
	protected:
		void setParentModel( Model* pModel );
	public:
		Model();
		virtual ~Model();

		/** @todo Exchange "Model" with "Entity" (which has dynamic properties etc) ?
		*/
		Model* getParentModel() const;
		void addChildModel( SharedPtr<Model> & rModel );
		SharedPtr<Model> getChildModelByName( const String & rName ) const;
		SharedPtr<Model> removeChildModel( const String & rName );
		SharedPtr<Model> removeChildModel( const SharedPtr<Model>& rModel );

		/**
			"graphical://model.model...model/graphicalname"
			"physical://model.model...model/physicalname"
		*/
		Physical* queryPhysical( const String & rQuery ) const;
		Graphical* queryGraphical( const String & rQuery ) const;

		Physical* getPhysicalByName( const String & rName ) const;
		Graphical* getGraphicalByName( const String & rName ) const;

		/** Adds a "Physical" object to this model.
			The model references it using a shared pointer.
			If it holds the only reference to the object it effectively
			owns the "Physical" object.
			A name can optionally be supplied. It can then be used for retrieving
			objects by name.
		*/
		void addPhysical( SharedPtr<Physical> & pPhysical, const String & rName = ""/*, bool bManaged*/ );

		/** Adds a "Graphical" object to this model.
			The model references it using a shared pointer.
			If it holds the only reference to the object it effectively
			owns the "Graphical" object.
			A name can optionally be supplied. It can then be used for retrieving
			objects by name.
		*/
		void addGraphical( SharedPtr<Graphical> & pGraphical, const String & rName = "" );

		void addController( SharedPtr<IObjectController> & pController, const String & rName = "" );
		void addLink( SharedPtr<ModelLink> & pModelLink, const String & rName = "" );

		void updatePhysics( real timeElapsed );
		void updateControllers( real timeElapsed );
	private:
		typedef AssocVector< String, SharedPtr<Physical> > PhysicalMap;
		PhysicalMap			mPhysicals;

		typedef AssocVector< String, SharedPtr<Graphical> > GraphicalMap;
		GraphicalMap		mGraphicals;

		typedef Vector< SharedPtr<Model> > ModelList;
		ModelList			mChildren;

		Model*				mParent;

		typedef Vector< SharedPtr<IObjectController> > CtrlrList;
		CtrlrList			mControllers;
	};
} // complex

} // model
} // app
} // yake

#endif
