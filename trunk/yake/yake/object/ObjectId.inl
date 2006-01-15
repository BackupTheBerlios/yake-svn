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
template<typename objectid_traits>
const typename ObjectId<objectid_traits>::this_type ObjectId<objectid_traits>::kNull = ObjectId<objectid_traits>(std::numeric_limits<CombinedType>::max());

template<typename objectid_traits>
bool ObjectId<objectid_traits>::isNull() const
{
	return (*this == kNull);
}
template<typename objectid_traits>
ObjectId<objectid_traits>::ObjectId(const ClassId clsId, const SerialNo no) : id_( no | (clsId<<16) )
{
}
template<typename objectid_traits>
ObjectId<objectid_traits>::ObjectId(const ObjectId& other) : id_(other.id_)
{
}
template<typename objectid_traits>
ObjectId<objectid_traits>::ObjectId(const CombinedType id) : id_(id)
{
}
template<typename objectid_traits>
ObjectId<objectid_traits>& ObjectId<objectid_traits>::operator=(const ObjectId& rhs)
{
	id_ = rhs.id_;
	return *this;
}
template<typename objectid_traits>
ObjectId<objectid_traits>& ObjectId<objectid_traits>::operator=(const CombinedType id)
{
	id_ = id;
	return *this;
}
template<typename objectid_traits>
typename ObjectId<objectid_traits>::ClassId ObjectId<objectid_traits>::classId() const
{
	return ((id_>>CombinedType(traits::kNumSerialNoBits)) & traits::kFullClassIdMask);
}
template<typename objectid_traits>
void ObjectId<objectid_traits>::setClassId(const ClassId clsId)
{
	id_ = serialNo() | (clsId<<traits::kNumSerialNoBits);
}
template<typename objectid_traits>
typename ObjectId<objectid_traits>::SerialNo ObjectId<objectid_traits>::serialNo() const
{
	return (id_ & traits::kFullSerialNoMask);
}
template<typename objectid_traits>
void ObjectId<objectid_traits>::setSerialNo(const SerialNo no)
{
	id_ = no | (classId()<<traits::kNumSerialNoBits);
}
template<typename objectid_traits>
bool ObjectId<objectid_traits>::operator==(const ObjectId& rhs)
{
	return id_ == rhs.id_;
}
template<typename objectid_traits>
bool ObjectId<objectid_traits>::operator!=(const ObjectId& rhs)
{
	return !(*this == rhs);
}
template<typename objectid_traits>
bool ObjectId<objectid_traits>::operator<(const ObjectId& rhs)
{
	return (id_ < rhs.id_);
}
template<typename objectid_traits>
bool ObjectId<objectid_traits>::operator<=(const ObjectId& rhs)
{
	return (id_ <= rhs.id_);
}
template<typename objectid_traits>
bool ObjectId<objectid_traits>::operator>(const ObjectId& rhs)
{
	return (id_ > rhs.id_);
}
template<typename objectid_traits>
bool ObjectId<objectid_traits>::operator>=(const ObjectId& rhs)
{
	return (id_ >= rhs.id_);
}
template<typename objectid_traits>
ObjectId<objectid_traits>::operator CombinedType() const
{
	return id_;
}
