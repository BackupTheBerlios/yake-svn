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
