#ifndef _FIELD_H_
#define _FIELD_H_

#include "reflection.h"

/*****************************************************************************
    MACROS
 *****************************************************************************/

/** The FIELD macro is used to declare a reflected field.
    @param ACCESS_ATTR access attribute (public, protected or private)
    @param FIELD_TYPE type of the field
    @param FIELD_NAME name of the field
 */
#define FIELD(ACCESS_ATTR, FIELD_TYPE, FIELD_NAME) \
private: \
    struct __field_##FIELD_NAME##__ \
		{ \
        __field_##FIELD_NAME##__() \
				{ \
            static reflection::__register_field__ reg(__OFFSET__(ClassType, FIELD_NAME), typeid(FIELD_TYPE), getClassStaticPtr(), ACCESS_##ACCESS_ATTR, #FIELD_TYPE, #FIELD_NAME); \
        } \
    } __field_##FIELD_NAME##__; \
    friend struct __field_##FIELD_NAME##__; \
ACCESS_ATTR: \
    FIELD_TYPE FIELD_NAME

/*****************************************************************************
    CLASSES
 *****************************************************************************/

namespace reflection 
{

/** The Field class represents an object's field.
 */
class Field 
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
    const char *getName() const 
		{ return m_name; }

    /** retrieves the field of the given object.
        @param result variable to store the result of the field
        @param object object to set the field of
        @exception TypeMismatchError thrown if the object is of invalid class or
                   the result is of the wrong type.
     */
    template <class Object, class Value> void get(Value &result, Object *object) const 
		{
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
        if (!m_class->checkUpCast(object->getClass())) throw TypeMismatchError("object");
        if (typeid(Value) != m_typeinfo) throw TypeMismatchError("result");
        result = *(const Value *)(((const char *)object) + m_offset);
    }

    /** sets the field of the given object.
        @param object object to set the field of
        @param value new value of the field
        @exception TypeMismatchError thrown if the object is of invalid class or
                   the value is of the wrong type.
        @exception IllegalAccessError thrown if the field's access is not public.
     */
    template <class Object, class Value> void set(Object *object, const Value &value) const 
		{
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
        if (!m_class->checkUpCast(object->getClass())) throw TypeMismatchError("object");
        if (typeid(Value) != m_typeinfo) throw TypeMismatchError("value");
        *(Value *)(((char *)object) + m_offset) = value;
    }

private:
    const std::type_info &m_typeinfo;
    const Class *m_class;
    enum ACCESS_TYPE m_access;
    const char *m_type;
    const char *m_name;
    unsigned long m_offset;

    //default constructor
    Field(unsigned long offset, const std::type_info &typeinfo, const Class *pclass, ACCESS_TYPE access, const char *type, const char *name) 
			: m_offset(offset),
        m_typeinfo(typeinfo),
        m_class(pclass),
        m_access(access),
        m_type(type),
        m_name(name) 
		{}

    friend class Class;
    friend struct __register_field__;
};

//internal class for registering a field
struct __register_field__ 
{
    __register_field__(unsigned long offset, const std::type_info &typeinfo, const Class *pclass, enum ACCESS_TYPE access, const char *type, const char *name) 
		{
        Field field(offset, typeinfo, pclass, access, type, name);
        ((Class *)pclass)->_addField(field);
    }
};

} // namespace reflection

#endif // _FIELD_H_