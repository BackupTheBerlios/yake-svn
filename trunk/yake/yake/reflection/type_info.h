#ifndef LOKITYPEINFO_INC_
#define LOKITYPEINFO_INC_

#include <typeinfo>
#include <cassert>

namespace yake
{
namespace base
{
    class type_info
    {
    public:
        // Constructors
        type_info(); // needed for containers
        type_info(const std::type_info&); // non-explicit

        // Access for the wrapped std::type_info
        const std::type_info& Get() const;
        // Compatibility functions
        bool before(const type_info& rhs) const;
        const char* name() const;

    private:
        const std::type_info* pInfo_;
    };
    
// Implementation
    
    inline type_info::type_info()
    {
        class Nil {};
        pInfo_ = &typeid(Nil);
        assert(pInfo_);
    }
    
    inline type_info::type_info(const std::type_info& ti)
    : pInfo_(&ti)
    { assert(pInfo_); }
    
    inline bool type_info::before(const type_info& rhs) const
    {
        assert(pInfo_);
        // type_info::before return type is int in some VC libraries 
        return pInfo_->before(*rhs.pInfo_) != 0;
    }

    inline const std::type_info& type_info::Get() const
    {
        assert(pInfo_);
        return *pInfo_;
    }
    
    inline const char* type_info::name() const
    {
        assert(pInfo_);
        return pInfo_->name();
    }

// Comparison operators
    
    inline bool operator==(const type_info& lhs, const type_info& rhs)
    // type_info::operator== return type is int in some VC libraries
    { return (lhs.Get() == rhs.Get()) != 0; }

    inline bool operator<(const type_info& lhs, const type_info& rhs)
    { return lhs.before(rhs); }

    inline bool operator!=(const type_info& lhs, const type_info& rhs)
    { return !(lhs == rhs); }    
    
    inline bool operator>(const type_info& lhs, const type_info& rhs)
    { return rhs < lhs; }
    
    inline bool operator<=(const type_info& lhs, const type_info& rhs)
    { return !(lhs > rhs); }
     
    inline bool operator>=(const type_info& lhs, const type_info& rhs)
    { return !(lhs < rhs); }

} // namespace base
} // namespace yake

#endif // LOKITYPEINFO_INC_
