#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include "reflection.h"

/*****************************************************************************
    MACROS
 *****************************************************************************/

/** The PROPERTY macro is used to declare an object's property. When used,
    the class must have two private methods in the form of TYPE get()/void set(TYPE)
    for setting and getting the property. By using this macro, the object
    gets a property interface for managing the value is if it was a field.
    Arithmetic and other operators work as expected. The defined property
    is reflected: it can be examined and set in run-time. Properties do
    not define an access attribute: there are always public. The property
    does not add any extra bytes to the class, besides those needed for reflection.
    @param TYPE type of the property
    @param NAME name of the property
 */
// todo: currently defined in bind_network with observer addons etc.
/*#define RX_PROPERTY(TYPE, NAME)\
private:\
    template <class T> class __property__##NAME {\
    public:\
        typedef __property__##NAME<T> Type;\
        __property__##NAME<T>() {\
				static reflection::__register_property__ reg(new __property__<ClassType, T>(&get_##NAME, &set_##NAME), getClassStaticPtr(), typeid(TYPE), #TYPE, #NAME);\
        }\
        inline TYPE get() const {\
            return owner()->get_##NAME();\
        }\
        inline void set(TYPE value) {\
            owner()->set_##NAME(value);\
        }\
        inline operator TYPE () const {\
            return get();\
        }\
        inline bool operator == (TYPE value) const {\
            return get() == value;\
        }\
        inline bool operator != (TYPE value) const {\
            return get() != value;\
        }\
        inline bool operator < (TYPE value) const {\
            return get() < value;\
        }\
        inline bool operator > (TYPE value) const {\
            return get() > value;\
        }\
        inline bool operator <= (TYPE value) const {\
            return get() <= value;\
        }\
        inline bool operator >= (TYPE value) const {\
            return get() >= value;\
        }\
        inline Type &operator = (TYPE value) {\
            set(value);\
            return *this;\
        }\
        inline Type &operator = (const Type &prop) {\
            if (&prop != this) set(prop.get());\
            return *this;\
        }\
        inline Type &operator += (TYPE value) {\
            set(get() + value);\
            return *this;\
        }\
        inline Type &operator -= (TYPE value) {\
            set(get() - value);\
            return *this;\
        }\
        inline Type &operator *= (TYPE value) {\
            set(get() * value);\
            return *this;\
        }\
        inline Type &operator /= (TYPE value) {\
            set(get() / value);\
            return *this;\
        }\
        inline Type &operator %= (TYPE value) {\
            set(get() % value);\
            return *this;\
        }\
        inline Type &operator <<= (int bits) {\
            set(get() << bits);\
            return *this;\
        }\
        inline Type &operator >>= (int bits) {\
            set(get() >> bits);\
            return *this;\
        }\
        inline Type &operator &= (TYPE value) {\
            set(get() & value);\
            return *this;\
        }\
        inline Type &operator |= (TYPE value) {\
            set(get() | value);\
            return *this;\
        }\
        inline Type &operator ^= (TYPE value) {\
            set(get() ^ value);\
            return *this;\
        }\
        inline TYPE operator ~() {\
            return ~get();\
        }\
        inline TYPE operator ++() {\
            TYPE val = get();\
            set(val + 1);\
            return val;\
        }\
        inline TYPE operator ++(int i) {\
            TYPE val = get();\
            set(val + 1);\
            return get();\
        }\
        inline TYPE operator --() {\
            TYPE val = get();\
            set(val - 1);\
            return val;\
        }\
        inline TYPE operator --(int i) {\
            TYPE val = get();\
            set(val - 1);\
            return get();\
        }\
    private:\
        ClassType *owner() {\
            return (ClassType *)(((const char *)this) - __OFFSET__(ClassType, NAME));\
        }\
        const ClassType *owner() const {\
            return (const ClassType *)(((const char *)this) - __OFFSET__(ClassType, NAME));\
        }\
    };\
    friend class __property__##NAME< TYPE >;\
public:\
    __property__##NAME< TYPE > NAME*/


/** The DEFAULT_PROPERTY macro defines a property that has a stock implementation
    of a field named m_NAME, a getter and setter.
    @param TYPE type of the property
    @param NAME name of the property
 */
/*#define RX_DEFAULT_PROPERTY(TYPE, NAME)\
private:\
    FIELD(private, TYPE, m_##NAME);\
    TYPE get_##NAME() const {\
        return m_##NAME;\
    }\
    void set_##NAME(TYPE value) {\
        m_##NAME = value;\
    }\
public:\
    RX_PROPERTY(TYPE, NAME)*/

/*****************************************************************************
    CLASSES
 *****************************************************************************/


namespace reflection 
{

//property handler base
struct __property_base__ 
{
    virtual ~__property_base__() 
		{}
};


//property handler
template <class T> struct __property_handler__ : public __property_base__ 
{
    //get
    virtual T get(const void *object) const = 0;

    //set
    virtual void set(void *object, T value) const = 0;
};


//property handler class
template <class C, class T> struct __property__ : public __property_handler__<T> 
{
    //type of getters/setters
    typedef T (C::*Getter)() const;
    typedef void (C::*Setter)(T);

    //pointer to member getters/setters
    Getter getter;
    Setter setter;

    //default constructor
    __property__(Getter g, Setter s) : getter(g), setter(s) 
		{}

    //get
    virtual T get(const void *object) const 
		{
        const C *o = (const C *)(object);
        return (o->*getter)();
    }

    //set
    virtual void set(void *object, T value) const 
		{
        C *o = (C *)(object);
        (o->*setter)(value);
    }
};

/** The Property class holds information about an object's property.
 */
class Property 
{
public:
    ///destructor
    ~Property() 
		{
        if (m_handler) delete m_handler;
    }

    /** returns the class that the member is declared into.
        @return the class that the member is declared into.
     */
    const Class & getClass() const 
		{
        return *m_class;
    }

    /** returns the property's type.
        @return the property's type.
     */
    const char * getType() const 
		{
        return m_type;
    }

    /** returns the field's type.
        @return the field's type.
     */
    const std::type_info & getTypeInfo() const 
		{
        return m_typeinfo;
    }

    /** returns the property's name.
        @return the property's name.
     */
    const char * getName() const 
		{
        return m_name;
    }

    /** returns the value of the property
        @param result result to store the value to
        @param object object to get the property of
        @exception TypeMismatchError thrown if there is a type mismatch
     */
    template <class Object, class Value> 
		void get(Value &result, const Object *object) const 
		{
        if (!m_class->checkUpCast(object->getClass())) throw TypeMismatchError("object");
        typedef const __property_handler__<Value> PropertyType;
        PropertyType *prop = dynamic_cast<PropertyType *>(m_handler);
        if (!prop) throw TypeMismatchError(m_name);
        result = prop->get((const void *)object);
    }

    /** sets the property's value
        @param object object to set the property of
        @param value value of the object
        @exception TypeMismatchError thrown if there is a type mismatch
     */
    template <class Object, class Value> 
		void set(Object *object, const Value &value) const 
		{
        if (!m_class->checkUpCast(object->getClass())) throw TypeMismatchError("object");
        typedef const __property_handler__<Value> PropertyType;
        PropertyType *prop = dynamic_cast<PropertyType *>(m_handler);
        if (!prop) throw TypeMismatchError(m_name);
        prop->set((void *)object, value);
    }

private:
		const std::type_info & m_typeinfo;
    const char * m_type;
    const char * m_name;
    __property_base__ * m_handler;
    const Class * m_class;

    //default constructor
    Property(const Class *pclass, const std::type_info & typeinfo, const char *type, const char *name) 
			: m_class(pclass),
				m_typeinfo(typeinfo),
        m_type(type),
        m_name(name),
        m_handler(0) 
		{}

    friend class Class;
    friend struct __register_property__;
};

//internal class for registering a property
struct __register_property__ 
{
		__register_property__(__property_base__ *handler, const Class *pclass, const std::type_info & typeinfo, const char *type, const char *name) 
		{
        Property property(pclass, typeinfo, type, name);
        ((Class *)pclass)->_addProperty(property, handler);
    }
};

} // namespace reflection

#endif // _PROPERTY_H_