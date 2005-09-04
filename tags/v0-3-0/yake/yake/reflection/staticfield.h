#ifndef _STATICFIELD_H_
#define _STATICFIELD_H_

#include "reflection.h"

/*****************************************************************************
    MACROS
 *****************************************************************************/

/** The STATIC_FIELD macro is used to declare a reflected static field.
    @param ACCESS_ATTR access attribute (public, protected or private)
    @param FIELD_TYPE type of the field
    @param FIELD_NAME name of the field
 */
#define STATIC_FIELD(ACCESS_ATTR, FIELD_TYPE, FIELD_NAME)\
private:\
    struct __static_field_##FIELD_NAME##__ {\
        __static_field_##FIELD_NAME##__() {\
            static yake::reflection::__register_static_field__ reg((void *)&FIELD_NAME, typeid(FIELD_TYPE), getClassStaticPtr(), ACCESS_##ACCESS_ATTR, #FIELD_TYPE, #FIELD_NAME);\
        }\
    } __static_field_##FIELD_NAME##__;\
    friend struct __static_field_##FIELD_NAME##__;\
ACCESS_ATTR :\
    static FIELD_TYPE FIELD_NAME

/*****************************************************************************
    CLASSES
 *****************************************************************************/

namespace yake
{
namespace reflection 
{

/** The StaticField class represents an object's static field.
 */
class StaticField 
{
public:
    /** returns the class that the member is declared into.
        @return the class that the member is declared into.
     */
    const Class &getClass() const 
		{ return *m_class; }

    /** returns the field's access.
        @return the field's access.
     */
    const ACCESS_TYPE getAccess() const 
		{ return m_access; }

    /** returns the field's type.
        @return the field's type.
     */
    const char *getType() const 
		{ return m_type; }

    /** returns the field's name.
        @return the field's name.
     */
    const char *getName() const {
        return m_name;
    }

    /** retrieves the static field's value.
        @param value variable to store the value of the field
        @exception TypeMismatchError thrown if the object is of invalid class or
                   the value is of the wrong type.
     */
    template <class Value> void get(Value &value) const 
		{
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
        if (typeid(Value) != m_typeinfo) throw TypeMismatchError("value");
        value = *(const Value *)(m_address);
    }

    /** sets the static field's value.
        @param value new value of the field
        @exception TypeMismatchError thrown if the object is of invalid class or
                   the value is of the wrong type.
        @exception IllegalAccessError thrown if the field's access is not public.
     */
    template <class Value> void set(const Value &value) const 
		{
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
        if (typeid(Value) != m_typeinfo) throw TypeMismatchError("value");
        *(Value *)(m_address) = value;
    }

private:
    const std::type_info &m_typeinfo;
    const Class *m_class;
    enum ACCESS_TYPE m_access;
    const char *m_type;
    const char *m_name;
    void *m_address;

    //default constructor
    StaticField(void *address, const std::type_info &typeinfo, const Class *pclass, ACCESS_TYPE access, const char *type, const char *name) 
			: m_address(address),
        m_typeinfo(typeinfo),
        m_class(pclass),
        m_access(access),
        m_type(type),
        m_name(name) 
		{}

    friend class Class;
    friend struct __register_static_field__;
};

//internal class for registering a static field
struct __register_static_field__ 
{
    __register_static_field__(void *address, const std::type_info &typeinfo, const Class *pclass, enum ACCESS_TYPE access, const char *type, const char *name) 
		{
        StaticField field(address, typeinfo, pclass, access, type, name);
        ((Class *)pclass)->_addStaticField(field);
    }
};

} // namespace reflection
} // namespace yake

#endif // _STATICFIELD_H_