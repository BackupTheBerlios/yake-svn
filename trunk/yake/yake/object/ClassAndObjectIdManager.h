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
#ifndef CLASSANDOBJECTIDMANAGER_H
#define CLASSANDOBJECTIDMANAGER_H

#include "common.h"
#include "ObjectId.h"

namespace yake {
namespace OBJECTS_NAMESPACE {

	template<typename classdata_type,typename objectid_type = ::yake::OBJECTS_NAMESPACE::ObjectId<default_objectid_traits> >
	struct ClassEntryT
	{
		// official interface:
		typedef classdata_type data_type;
		ClassEntryT();
		virtual ~ClassEntryT(); // <= virtual so as to allow derived class entries.
		void setData(const classdata_type&);
		const classdata_type& getData() const;
		ResultCode addObjectId(const objectid_type& objId);
		ResultCode removeObjectId(const objectid_type& objId);
		typename objectid_type::SerialNo nextSerialNo();
		typename objectid_type::ObjectIdSet getObjectIds() const;
		// other things:
	private:
		typename objectid_type::ObjectIdSet		ids_;
		typename objectid_type::SerialNo		lastSerialNo_;
		classdata_type	data_;
	};

	template<typename classentry_type,typename objectid_type = ::yake::OBJECTS_NAMESPACE::ObjectId<default_objectid_traits> >
	struct ClassAndObjectIdManager
	{
	public:
		typedef objectid_type ObjectId;
		// pull in types
		typedef typename objectid_type::ClassId ClassId;
		typedef typename objectid_type::ObjectIdSet ObjectIdSet;
		typedef typename objectid_type::ClassIdSet ClassIdSet;

		ClassAndObjectIdManager();
		~ClassAndObjectIdManager();

		typedef typename classentry_type::data_type classdata_type;
		ResultCode registerClassId(const ClassId, const classdata_type& = classdata_type());
		const ClassIdSet& getRegisteredClasses() const;
		const classdata_type& getClassData(const ClassId) const; // may throw
		bool hasClass(const ClassId) const;
		void clear();

		ObjectId createObjectId(const ClassId);
		ResultCode addObjectId(const ObjectId&);
		ObjectIdSet getObjectIds(const ClassId) const;
		ResultCode removeObjectId(const ObjectId&);
	private:
		typedef classentry_type ClassEntry;
		typedef std::map<ClassId,ClassEntry> ClassMap;
		ClassMap		classes_;
		ClassIdSet		classIds_;
	};

	#include "ClassAndObjectIdManager.inl"

} // namespace object
} // namespace yake

#endif
