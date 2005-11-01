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
#ifndef _YAKE_BASE_TYPE_INFO_
#define _YAKE_BASE_TYPE_INFO_

#include <typeinfo>
#include <cassert>

namespace yake
{
namespace base
{
    
class type_info
{
public: // constructors
    type_info(); // needed for containers
    type_info(const std::type_info &); // non-explicit

    // access for the wrapped std::type_info
    const std::type_info & Get() const;
    // compatibility functions
    bool before(const type_info & rhs) const;
    const char * name() const;

private: // data
	const std::type_info * m_info;
};

/* Implementation */
  
inline type_info::type_info()
{
	class nil {};
    m_info = &typeid(nil);
    assert(m_info);
}
    
inline type_info::type_info(const std::type_info & ti)
	: m_info(&ti)
{ assert(m_info); }
    
inline bool type_info::before(const type_info & rhs) const
{
	assert(m_info);
	// type_info::before return type is int in some VC libraries 
	return m_info->before(*rhs.m_info) != 0;
}

inline const std::type_info& type_info::Get() const
{
	assert(m_info);
	return *m_info;
}
    
inline const char * type_info::name() const
{
	assert(m_info);
	return m_info->name();
}

/* Comparison operators */
    
inline bool operator==(const type_info & lhs, const type_info & rhs)
// type_info::operator== return type is int in some VC libraries
{ return (lhs.Get() == rhs.Get()) != 0; }

inline bool operator<(const type_info & lhs, const type_info & rhs)
{ return lhs.before(rhs); }

inline bool operator!=(const type_info & lhs, const type_info & rhs)
{ return !(lhs == rhs); }    
    
inline bool operator>(const type_info & lhs, const type_info & rhs)
{ return rhs < lhs; }
    
inline bool operator<=(const type_info & lhs, const type_info & rhs)
{ return !(lhs > rhs); }
     
inline bool operator>=(const type_info & lhs, const type_info & rhs)
{ return !(lhs < rhs); }

} // namespace base
} // namespace yake

#endif // _YAKE_BASE_TYPE_INFO_
