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
#ifndef OBJECTCONTAINER_H
#define OBJECTCONTAINER_H

namespace yake {
namespace OBJECTS_NAMESPACE {

	template<typename object_type,typename objectid_type = ObjectId<default_objectid_traits> >
	struct ObjectContainer
	{
	public:
		typedef objectid_type ObjectId;

		ObjectContainer();
		~ObjectContainer();

		bool addObject(const ObjectId&, object_type*);
		object_type* removeObject(const ObjectId&);
		//void removeObject(object_type*);
		//bool destroyObject(const ObjectId&); //<= needs destruction policy?
		object_type* getObject(const ObjectId&) const;
	private:
		typedef std::map<ObjectId,object_type*> IdObjMap;
		IdObjMap	id2obj_;
	};
	template<typename object_type,typename objectid_type>
	ObjectContainer<object_type,objectid_type>::ObjectContainer()
	{}
	template<typename object_type,typename objectid_type>
	ObjectContainer<object_type,objectid_type>::~ObjectContainer()
	{
		assert( id2obj_.empty() );
	}
	template<typename object_type,typename objectid_type>
	bool ObjectContainer<object_type,objectid_type>::addObject(const ObjectId& objId, object_type* obj)
	{
		assert( obj );
		if (!obj)
			return false;
		assert( !objId.isNull() );
		if (objId.isNull())
			return false;
		const bool exists = util::map_contains( id2obj_, objId );
		assert( !exists );
		if (exists)
			return false;
		id2obj_.insert( typename IdObjMap::value_type(objId,obj) );
		return true;
	}
	template<typename object_type,typename objectid_type>
	object_type* ObjectContainer<object_type,objectid_type>::removeObject(const ObjectId& objId)
	{
		assert( !objId.isNull() );
		if (objId.isNull())
			return 0;
		typename IdObjMap::iterator it = id2obj_.find( objId );
		assert( it != id2obj_.end() );
		if (it == id2obj_.end())
			return 0;
		object_type* obj = it->second;
		id2obj_.erase( it );
		return obj;
	}
	template<typename object_type,typename objectid_type>
	object_type* ObjectContainer<object_type,objectid_type>::getObject(const ObjectId& objId) const
	{
		assert( !objId.isNull() );
		if (objId.isNull())
			return 0;
		typename IdObjMap::const_iterator it = id2obj_.find( objId );
		assert( it != id2obj_.end() );
		if (it == id2obj_.end())
			return 0;
		return it->second;
	}
} // namespace object
} // namespace yake

#endif
