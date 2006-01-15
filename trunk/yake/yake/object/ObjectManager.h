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
#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

namespace yake {
namespace OBJECTS_NAMESPACE {
	/**
		@Remarks Requirements for object_type:
					- method: ObjectId id() const
	*/
	template<typename obj_type,typename objectid_type = ObjectId<default_objectid_traits> >
	struct ObjectManager
	{
	public:
		typedef obj_type object_type;
		typedef objectid_type ObjectId;
		typedef typename objectid_type::ClassId ClassId;
		typedef typename objectid_type::ClassIdSet ClassIdSet;

		typedef boost::function<object_type*(const ObjectId&)> CreateObjectFn;
		typedef boost::function<void(object_type*)> DestroyObjectFn;
		typedef ObjectContainer<object_type> object_ctr_type;

		typedef boost::function<void(object_type*)> AboutToDestroyObjectFn; //@todo use!!
		typedef boost::function<void(object_type*)> ObjectCreatedFn; //@todo use!!

		typedef std::map<ClassId,ClassId> ClassIdMap;

		ObjectManager();
		~ObjectManager();

		// class registration
		void setClassIdGenerationRange(const ClassId min, const ClassId max);

		/** Register class auto-generating an id in the process. */
		std::pair<ResultCode,ClassId> registerClass(const std::string& name, const CreateObjectFn&, const DestroyObjectFn&);
		/** Register class with specific id. */
		ResultCode registerClass(const ClassId clsId, const std::string& name, const CreateObjectFn&, const DestroyObjectFn&);
		/** Return class id for string id. */
		std::pair<ResultCode,ClassId> getClassId(const std::string& name) const;
		/** Return name for class id. */
		std::pair<ResultCode,std::string> getClassName(const ClassId) const;
		/** Register class alias to registered class using ClassIds.
			@Remarks One *cannot* create an alias to an alias.
				Furthermore the aliasClsId shall not be identical to targetClsId!
		*/
		ResultCode registerClassAlias(const ClassId aliasClsId, const ClassId targetClsId);
		/** Register class alias to registered class using string class id.
			@Remarks One *cannot* create an alias to an alias.
				Furthermore "aliasId" shall not be the string id of a registered class!
		*/
		ResultCode registerClassAlias(const std::string& aliasName, const ClassId targetClsId);

		const ClassIdSet& getRegisteredClasses() const;
		const ClassIdMap& getClassAliases() const;

		void unregisterClasses();
		void unregisterAliases();

		// object management
		object_type* createObject(const ClassId, const ObjectId& userObjId = ObjectId::kNull);
		object_type* createObject(const std::string&, const ObjectId& userObjId = ObjectId::kNull);
		void destroyObject(object_type*);
		void destroyAllObjects();

	private:
		uint8 determineClassIdBitsForRange(const ClassId min, const ClassId max) const;

		object_ctr_type					objCtr_;

		struct ClassEntryData
		{
			CreateObjectFn	create_;
			DestroyObjectFn	destroy_;
			std::string		name_;
		};
		typedef ClassAndObjectIdManager<ClassEntryT<ClassEntryData> > TheClassAndObjectIdManager;
		TheClassAndObjectIdManager		idMgr_;

		ClassIdMap						idAliases_;

		typedef std::map<std::string,ClassId> StringToClassIdMap;
		StringToClassIdMap				strIdAliases_;

		ClassId							lastClsId_;
		ClassId							minGenClsId_;
		ClassId							maxGenClsId_;

		typedef std::map<ObjectId,object_type*> IdObjMap;
		IdObjMap						objs_;
	};

#include "ObjectManager.inl"

	template<class objmgr_type>
	inline std::ostream& dump(std::ostream& out, const objmgr_type& objMgr)
	{
		out << "ObjectManager=\n";
		out << "  classes = \n";
		const typename objmgr_type::ClassIdSet& ids = objMgr.getRegisteredClasses();
		for (typename objmgr_type::ClassIdSet::const_iterator itId = ids.begin(); itId != ids.end(); ++itId)
		{
			const std::string name = objMgr.getClassName(*itId).second;
			out << "    '" << (name.empty() ? "(not found)" : name) << "' 0x" << std::hex << *itId << "\n";
		}

		out << "  aliases = \n";
		const typename objmgr_type::ClassIdMap& idMap = objMgr.getClassAliases();
		for (typename objmgr_type::ClassIdMap::const_iterator it = idMap.begin(); it != idMap.end(); ++it)
		{
			const std::string name = objMgr.getClassName(it->second).second;
			out << "    0x" << std::hex << it->first << " -> 0x" << it->second << " '" << (name.empty() ? "(not found)" : name) << "'\n";
		}
		out << std::dec << "\n";
		return out;
	}

	template<typename objmgr_type>
	inline size_t createOptimizedClassIdMap(const objmgr_type& objMgr, size_t& reqBits, typename objmgr_type::ObjectId::ClassIdMap& idMap, const typename objmgr_type::ClassId minId = 0/*, const bool includedAliases = false*/)
	{
		typedef typename objmgr_type::ClassId ClassId;
		typedef typename objmgr_type::ClassIdSet ClassIdSet;
		typedef typename objmgr_type::ObjectId::ClassIdMap ClassIdMap;

		ClassIdSet ids = objMgr.getRegisteredClasses();
		idMap.clear();
		ClassId lastId = minId;
		for (ClassIdSet::const_iterator itId = ids.begin(); itId != ids.end(); ++itId)
		{
			idMap.insert( std::make_pair(lastId++,*itId) );
		}
		/*
		if (includedAliases)
		{
			//@todo const ClassIdMap& aliases = objMgr.getClassAliases();
			for (ClassIdMap::const_iterator it = aliases.begin(); it != aliases.end(); ++it)
			...
		}
		*/
		reqBits = 0;
		while (lastId > 0)
		{
			++reqBits;
			lastId = lastId >> 1;
		}
		return idMap.size();
	}


} // namespace object
} // namespace yake

#endif
