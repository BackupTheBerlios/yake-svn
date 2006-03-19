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

	template<typename classdata_type,typename objectid_type>
	typename objectid_type::SerialNo ClassEntryT<classdata_type,objectid_type>::nextSerialNo()
	{
		if (lastSerialNo_ == std::numeric_limits<typename objectid_type::SerialNo>::max())
			return lastSerialNo_;
		return ++lastSerialNo_;
	}
	template<typename classdata_type,typename objectid_type>
	ClassEntryT<classdata_type,objectid_type>::ClassEntryT() 
		: lastSerialNo_(0)
	{
	}
	template<typename classdata_type,typename objectid_type>
	ClassEntryT<classdata_type,objectid_type>::~ClassEntryT()
	{
	}
	template<typename classdata_type,typename objectid_type>
	ResultCode ClassEntryT<classdata_type,objectid_type>::addObjectId(const objectid_type& objId)
	{
	#ifdef _DEBUG // set::insert() doesn't hurt...
		if (ids_.find( objId ) != ids_.end())
			return RC_ALREADY_REGISTERED;
	#endif
		ids_.insert( objId );
		return RC_OK;
	}
	template<typename classdata_type,typename objectid_type>
	ResultCode ClassEntryT<classdata_type,objectid_type>::removeObjectId(const objectid_type& objId)
	{
		typename objectid_type::ObjectIdSet::iterator itId = ids_.find( objId );
		if (itId != ids_.end())
		{
			ids_.erase( objId );
			return RC_OK;
		}
		return RC_NOT_FOUND;
	}
	template<typename classdata_type,typename objectid_type>
	typename objectid_type::ObjectIdSet ClassEntryT<classdata_type,objectid_type>::getObjectIds() const
	{
		return ids_;
	}
	template<typename classdata_type,typename objectid_type>
	void ClassEntryT<classdata_type,objectid_type>::setData(const classdata_type& data)
	{
		data_ = data;
	}
	template<typename classdata_type,typename objectid_type>
	const classdata_type& ClassEntryT<classdata_type,objectid_type>::getData() const
	{
		return data_;
	}

	//--------

	template<typename classentry_type,typename objectid_type>
	ClassAndObjectIdManager<classentry_type,objectid_type>::ClassAndObjectIdManager()
	{
	}
	template<typename classentry_type,typename objectid_type>
	ClassAndObjectIdManager<classentry_type,objectid_type>::~ClassAndObjectIdManager()
	{
		clear();
	}
	template<typename classentry_type,typename objectid_type>
	void ClassAndObjectIdManager<classentry_type,objectid_type>::clear()
	{
		classes_.clear();
		classIds_.clear();
	}
	template<typename classentry_type,typename objectid_type>
	bool ClassAndObjectIdManager<classentry_type,objectid_type>::hasClass(const ClassId clsId) const
	{
		return util::contains(classIds_,clsId);
	}
	template<typename classentry_type,typename objectid_type>
	ResultCode ClassAndObjectIdManager<classentry_type,objectid_type>::registerClassId(const ClassId clsId, const classdata_type& data)
	{
		typename ClassMap::const_iterator it = classes_.find( clsId );
		if (it != classes_.end())
		{
			if (classIds_.find( clsId ) != classIds_.end())
				classIds_.insert( clsId );
			return RC_ALREADY_REGISTERED;
		}
		ClassEntry e;
		e.setData(data);
		classes_.insert( ClassMap::value_type(clsId,e) );
		classIds_.insert( clsId );
		return RC_OK;
	}
	template<typename classentry_type,typename objectid_type>
	const typename objectid_type::ClassIdSet& ClassAndObjectIdManager<classentry_type,objectid_type>::getRegisteredClasses() const
	{
		return classIds_;
	}
	template<typename classentry_type,typename objectid_type>
	objectid_type ClassAndObjectIdManager<classentry_type,objectid_type>::createObjectId(const typename objectid_type::ClassId clsId)
	{
		typename ClassMap::iterator it = classes_.find( clsId );
		if (it == classes_.end())
			return ObjectId::kNull;
		uint16 no = it->second.nextSerialNo();
		if (no == std::numeric_limits<uint16>::max())
			return ObjectId::kNull;
		ObjectId id( clsId, no );
		it->second.addObjectId( id );
		return id;
	}
	template<typename classentry_type,typename objectid_type>
	ResultCode ClassAndObjectIdManager<classentry_type,objectid_type>::addObjectId(const ObjectId& objId)
	{
		typename ClassMap::iterator it = classes_.find( objId.classId() );
		if (it == classes_.end())
			return RC_UNKNOWN_CLASS;
		return it->second.addObjectId(objId);
	}
	template<typename classentry_type,typename objectid_type>
	typename objectid_type::ObjectIdSet ClassAndObjectIdManager<classentry_type,objectid_type>::getObjectIds(const ClassId clsId) const
	{
		typename ClassMap::const_iterator it = classes_.find( clsId );
		if (it == classes_.end())
			return ObjectIdSet();
		return it->second.getObjectIds();
	}
	template<typename classentry_type,typename objectid_type>
	ResultCode ClassAndObjectIdManager<classentry_type,objectid_type>::removeObjectId(const ObjectId& objId)
	{
		typename ClassMap::iterator it = classes_.find( objId.classId() );
		if (it == classes_.end())
			return RC_UNKNOWN_CLASS;
		return it->second.removeObjectId( objId );
	}
	template<typename classentry_type,typename objectid_type>
	const typename classentry_type::data_type& ClassAndObjectIdManager<classentry_type,objectid_type>::getClassData(const ClassId clsId) const
	{
		typename ClassMap::const_iterator it = classes_.find( clsId );
		if (it == classes_.end())
			throw "ClassAndObjectIdManager<>::getClassData(): Class entry not found!";
		return it->second.getData();
	}
