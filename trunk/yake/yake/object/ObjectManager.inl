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
	template<typename obj_type, typename objectid_type>
	ObjectManager<obj_type,objectid_type>::ObjectManager() : minGenClsId_(0), maxGenClsId_(std::numeric_limits<ClassId>::max())
	{
		lastClsId_ = minGenClsId_;
	}
	template<typename obj_type, typename objectid_type>
	ObjectManager<obj_type,objectid_type>::~ObjectManager()
	{
	}
	template<typename obj_type, typename objectid_type>
	void ObjectManager<obj_type,objectid_type>::unregisterClasses()
	{
		assert( objs_.empty() );
		if (!objs_.empty()) //@TODO is this desirable behaviour?
			destroyAllObjects();

		unregisterAliases();
		idMgr_.clear();
		lastClsId_ = minGenClsId_;
	}
	template<typename obj_type, typename objectid_type>
	void ObjectManager<obj_type,objectid_type>::unregisterAliases()
	{
		idAliases_.clear();
		strIdAliases_.clear();
	}
	template<typename obj_type, typename objectid_type>
	std::pair<ResultCode,typename objectid_type::ClassId> ObjectManager<obj_type,objectid_type>::getClassId(const std::string& id) const
	{
		assert( !id.empty() );
		if (id.empty())
			return std::make_pair(RC_INVALID_INPUT,0);
		StringToClassIdMap::const_iterator it = strIdAliases_.find( id );
		if (it == strIdAliases_.end())
			return std::make_pair(RC_NOT_FOUND,0);
		return std::make_pair(RC_OK,it->second);
	}
	template<typename obj_type, typename objectid_type>
	std::pair<ResultCode,std::string> ObjectManager<obj_type,objectid_type>::getClassName(const ClassId clsId) const
	{
		assert( idMgr_.hasClass( clsId ) );
		if (!idMgr_.hasClass( clsId ))
			return std::make_pair(RC_NOT_FOUND,"");
		return std::make_pair(RC_OK,idMgr_.getClassData( clsId ).name_);
	}
	template<typename obj_type, typename objectid_type>
	ResultCode ObjectManager<obj_type,objectid_type>::registerClass(const ClassId clsId, const std::string& name, const CreateObjectFn& createFn, const DestroyObjectFn& destroyFn)
	{
		// check params
		assert( !name.empty() );
		if (name.empty())
			return RC_INVALID_INPUT;
		assert( !createFn.empty() );
		if (createFn.empty())
			return RC_INVALID_INPUT;
		assert( !destroyFn.empty() );
		if (destroyFn.empty())
			return RC_INVALID_INPUT;

		// try to register class
		ClassEntryData data;
		data.name_ = name;
		data.create_ = createFn;
		data.destroy_ = destroyFn;

		ResultCode rc = idMgr_.registerClassId( clsId, data );
		assert( rc == RC_OK );
		if (rc != RC_OK)
			return rc;

		strIdAliases_.insert( StringToClassIdMap::value_type(name,clsId) );

		return RC_OK;
	}
	template<typename obj_type, typename objectid_type>
	std::pair<ResultCode,typename objectid_type::ClassId> ObjectManager<obj_type,objectid_type>::registerClass(const std::string& name, const CreateObjectFn& createFn, const DestroyObjectFn& destroyFn)
	{
		// check params
		assert( !name.empty() );
		if (name.empty())
			return std::make_pair(RC_INVALID_INPUT,0);
		assert( !createFn.empty() );
		if (createFn.empty())
			return std::make_pair(RC_INVALID_INPUT,0);
		assert( !destroyFn.empty() );
		if (destroyFn.empty())
			return std::make_pair(RC_INVALID_INPUT,0);

		// generate class id
		ClassId clsId = lastClsId_++;

		// try to register class
		ResultCode rc = this->registerClass(clsId, name, createFn, destroyFn);
		if (rc != RC_OK)
		{
			--lastClsId_;
		}
		return std::make_pair(rc,clsId);
	}
	template<typename obj_type, typename objectid_type>
	ResultCode ObjectManager<obj_type,objectid_type>::registerClassAlias(const ClassId aliasClsId, const ClassId targetClsId)
	{
		// check params
		assert( aliasClsId != targetClsId );
		if (aliasClsId == targetClsId)
			return RC_NOT_FOUND;
		assert( idMgr_.hasClass( targetClsId ) );
		if (!idMgr_.hasClass( targetClsId ))
			return RC_NOT_FOUND;

		// check for existing alias
		assert( !util::map_contains(idAliases_,aliasClsId) );
		if (util::map_contains(idAliases_,aliasClsId))
			return RC_ALREADY_EXISTS;

		// register alias
		idAliases_.insert( std::make_pair( aliasClsId, targetClsId ) );
		return RC_OK;
	}
	template<typename obj_type, typename objectid_type>
	ResultCode ObjectManager<obj_type,objectid_type>::registerClassAlias(const std::string& aliasName, const ClassId targetClsId)
	{
		// check params
		assert( !aliasName.empty() );
		if (aliasName.empty())
			return RC_INVALID_INPUT;
		assert( idMgr_.hasClass( targetClsId ) );
		if (!idMgr_.hasClass( targetClsId ))
			return RC_NOT_FOUND;

		// check for existing alias
		assert( !util::map_contains(strIdAliases_,aliasName) );
		if (util::map_contains(strIdAliases_,aliasName))
			return RC_ALREADY_EXISTS;

		// register alias
		strIdAliases_.insert( std::make_pair( aliasName, targetClsId ) );
		return RC_OK;
	}
	template<typename obj_type, typename objectid_type>
	void ObjectManager<obj_type,objectid_type>::setClassIdGenerationRange(const ClassId min, const ClassId max)
	{
		//@FIXME make this whole thingy safer...
		minGenClsId_ = min;
		maxGenClsId_ = max;
		lastClsId_ = minGenClsId_;
	}
	template<typename obj_type, typename objectid_type>
	uint8 ObjectManager<obj_type,objectid_type>::determineClassIdBitsForRange(const ClassId min, const ClassId max) const
	{
		uint8 count = 0;
		ClassId curr = max - min;
		if (curr == 0)
			return 1;
		while (curr > 0)
		{
			++count;
			curr = curr >> 1;
		}
		return count;
	}
	template<typename obj_type, typename objectid_type>
	const typename ObjectManager<obj_type,objectid_type>::ClassIdSet& ObjectManager<obj_type,objectid_type>::getRegisteredClasses() const
	{
		return idMgr_.getRegisteredClasses();
	}
	template<typename obj_type, typename objectid_type>
	const typename ObjectManager<obj_type,objectid_type>::ClassIdMap& ObjectManager<obj_type,objectid_type>::getClassAliases() const
	{
		return this->idAliases_;
	}
	template<typename obj_type, typename objectid_type>
	void ObjectManager<obj_type,objectid_type>::destroyAllObjects()
	{
		IdObjMap objs = objs_; // copy!
		for (IdObjMap::const_iterator it = objs.begin(); it != objs.end(); ++it)
		{
			destroyObject( it->second );
		}
		assert( objs_.empty() );
	}
	template<typename obj_type, typename objectid_type>
	void ObjectManager<obj_type,objectid_type>::destroyObject(object_type* obj)
	{
		if (!obj)
			return;

		// erase reference to object
		assert( !obj->id().isNull() );
		if (!obj->id().isNull())
		{
			IdObjMap::iterator it = objs_.find( obj->id() );
			if (it != objs_.end())
				objs_.erase( obj->id() );
		}

		// look up class
		ClassId clsId = obj->id().classId();
		if (!idMgr_.hasClass( clsId ))
		{
			// try alias
			ClassIdMap::const_iterator it = idAliases_.find( clsId );
			assert( it != idAliases_.end() );
			if (it == idAliases_.end())
				return; //@FIXME or throw ?
			clsId = it->second;
			assert( idMgr_.hasClass( clsId ) );
			if (!idMgr_.hasClass( clsId ))
				return; //@FIXME or throw ?
		}
		// try to destroy instance
		const ClassEntryData& data = idMgr_.getClassData( clsId );
		assert( !data.destroy_.empty() );
		if (data.destroy_.empty())
		{
			return; //@FIXME or throw ?
		}

		data.destroy_( obj );
	}
	template<typename obj_type, typename objectid_type>
	typename ObjectManager<obj_type,objectid_type>::object_type* ObjectManager<obj_type,objectid_type>::createObject(const std::string& id, const objectid_type& userObjId = objectid_type::kNull)
	{
		// check params
		assert( !id.empty() );
		if (id.empty())
			return 0;

		// look up class id and try to create object if the class id could be found.
		std::pair<ResultCode,ClassId> ret = this->getClassId( id );
		if (ret.first != RC_OK)
			return 0;
		return this->createObject( ret.second, userObjId );
	}
	template<typename obj_type, typename objectid_type>
	typename ObjectManager<obj_type,objectid_type>::object_type* ObjectManager<obj_type,objectid_type>::createObject(const ClassId clsId, const objectid_type& userObjId = objectid_type::kNull)
	{
		if (!userObjId.isNull())
		{
			assert( clsId == userObjId.classId() );
		}

		ClassId theClsId = clsId;

		// look up class
		if (!idMgr_.hasClass( theClsId ))
		{
			// try alias
			ClassIdMap::const_iterator it = idAliases_.find( theClsId );
			assert( it != idAliases_.end() );
			if (it == idAliases_.end())
				return 0;
			theClsId = it->second;
			assert( idMgr_.hasClass( theClsId ) );
			if (!idMgr_.hasClass( theClsId ))
				return 0;
		}

		// try to create instance
		const ClassEntryData& data = idMgr_.getClassData( theClsId );
		assert( !data.create_.empty() );
		if (data.create_.empty())
			return 0;

		ObjectId objId;
		if (!userObjId.isNull())
		{
			objId = userObjId;
		}
		else
		{
			objId = idMgr_.createObjectId( theClsId );
			assert( !objId.isNull() );
			if (objId.isNull())
				return 0;
		}
		assert( !objId.isNull() );
		if (objId.isNull())
			return 0;

		object_type* obj = data.create_( objId );
		assert( obj );
		if (!obj)
		{
			idMgr_.removeObjectId( objId );
			return 0;
		}

		assert( obj->id() == objId );

		objs_.insert( std::make_pair( obj->id(), obj ) );
		return obj;
	}
