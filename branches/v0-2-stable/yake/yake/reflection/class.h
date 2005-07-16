#ifndef _CLASS_H_
#define _CLASS_H_

#include "reflection.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

/*****************************************************************************
    MACROS
 *****************************************************************************/

//todo: add yake::reflection:: to __instance_creator etc.

/** the CLASS macro is used to declare reflection support in a class. It must
    be placed on the public part of the class.
    @param CLASS_NAME fully qualified name of this class.
    @param SUPER_CLASS_NAME fully qualified name of the super class;
           if there is no base class, pass NullClass.
 */
#define RX_CLASS(CLASS_NAME, SUPER_CLASS_NAME) \
protected: \
    typedef CLASS_NAME ClassType; \
    static const yake::reflection::Class * getClassStaticPtr() \
		{ \
				static yake::reflection::Class _class( \
            #CLASS_NAME, \
						typeid(CLASS_NAME), \
						SUPER_CLASS_NAME::getClassStaticPtr(), \
						yake::reflection::__instance_creator_factory__<ClassType, __IS_OBJECT__(ClassType)>::create()); \
        return &_class; \
    } \
public: \
    static const yake::reflection::Class & getClassStatic() \
		{ \
        return *CLASS_NAME::getClassStaticPtr(); \
    } \
    virtual const yake::reflection::Class & getClass() const \
		{ \
        return *CLASS_NAME::getClassStaticPtr(); \
    }

/*****************************************************************************
    CLASSES
 *****************************************************************************/

namespace yake
{
namespace reflection 
{

class Object;

//instance creator interface
struct __instance_creator_base__ 
{
    virtual ~__instance_creator_base__() {}
    virtual Object * createInstance() const = 0;
};


//template instance creator
template <class T> struct __instance_creator__ : __instance_creator_base__
{
    virtual Object * createInstance() const 
		{
        return new T();
    }
};


//template instance creator factory
#ifdef _MSC_VER
template <class T> struct __instance_creator_factory1__ 
{
        template <bool IS_OBJECT> struct factory 
				{
            static __instance_creator_base__ *create() 
						{
                return 0;
            }    
        };    
        template <> struct factory<true> 
				{
            static __instance_creator_base__ *create() 
						{
                return new __instance_creator__<T>;
            }    
        };    
};    
template <class T, bool IS_OBJECT> struct __instance_creator_factory__ : __instance_creator_factory1__<T>::factory<IS_OBJECT> 
{};    
#else
template <class T, bool IS_OBJECT> struct __instance_creator_factory__ 
{
    static __instance_creator_base__ *create() 
		{
        return 0;
    }    
};    
template <class T> struct __instance_creator_factory__<T, true> 
{
    static __instance_creator_base__ *create() 
		{
        return new __instance_creator__<T>;
    }    
};
#endif

class Field;
class StaticField;
class Method;
class StaticMethod;
class Property;
class Event;

struct __property_base__;

/** The Class class is used as a placeholder for run-time information of an
    an object's class. A class can be used to query an object's fields,
    methods, properties, events and super class.
 */
class Class 
{
public:
    ///type of list of fields
    typedef std::list<Field> FieldList;

    ///type of list of static fields
    typedef std::list<StaticField> StaticFieldList;

    ///type of list of methods
    typedef std::list<Method> MethodList;

    ///type of list of static methods
    typedef std::list<StaticMethod> StaticMethodList;

    ///type of list of properties
    typedef std::list<Property> PropertyList;

    ///type of list of events
    typedef std::list<Event> EventList;

    //the default constructor
    Class(const char* name, const std::type_info & typeinfo, const Class* super, __instance_creator_base__* creator) 
			: m_name(name),
				m_typeinfo(&typeinfo),
        m_super(super),
        m_creator(creator) 
		{}

    //destructor
    Class() 
		{
        if (m_creator) delete m_creator;
    }

    /** returns the class name
        @return the class name
     */
    const char *getName() const 
		{ return m_name; }

    /** returns the field's type.
        @return the field's type.
     */
    const std::type_info & get_type_info() const 
		{ 
			if (!m_typeinfo) throw exception(); // todo, specialised exception
			return *m_typeinfo; 
		}

    /** checks if the class has a superclass
        @return true if the class has a superclass
     */
    const bool hasSuper() const 
		{ return m_super != 0; }

    /** returns the superclass of the class
        @return the superclass of the class; the superclass must not be null
        @exception SuperClassError thrown if there is no super class
     */
    const Class &getSuper() const 
		{
        if (!m_super) throw SuperClassError(m_name);
        return *m_super;
    }

    /** returns the collection of fields
        @return the collection of fields
     */
    const FieldList &getFields() const 
		{ return m_fields; }

    /** returns the collection of static fields
        @return the collection of static fields
     */
    const StaticFieldList &getStaticFields() const 
		{ return m_staticFields; }

    /** returns the list of methods
        @return the list of methods
     */
    const MethodList &getMethods() const 
		{ return m_methods; }

    /** returns the collection of static methods
        @return the collection of static methods
     */
    const StaticMethodList &getStaticMethods() const 
		{ return m_staticMethods; }

    /** returns the collection of properties
        @return the collection of properties
     */
    const PropertyList &getProperties() const {
        return m_properties;
    }

    /** returns the collection of properties
        @return the collection of properties
     */
    const EventList & getEvents() const 
		{ return m_events; }

    /** checks if this class is a base class of the given class.
        @param cl class to check against
        @return true if this class is a base class of the given class.
     */
    bool isBase(const Class &cl) const 
		{
        for(const Class *c = cl.m_super; c; c = c->m_super) 
				{
            if (c == this) return true;
        }
        return false;
    }

    /** checks if this class is a derived class of the given class.
        @param cl class to check against
        @return true if this class is a derived class of the given class.
     */
    bool isDerived(const Class &cl) const 
		{ return cl.isBase(*this); }

    /** checks if this class is the super class of the given class.
        @param cl class to check against
        @return true if this class is the super class of the given class.
     */
    bool isSuper(const Class &cl) const 
		{ return cl.m_super == this; }

    /** checks if this class is the direct subclass of the given class.
        @param cl class to check against
        @return true if this class is the sub class of the given class.
     */
    bool isSub(const Class &cl) const 
		{ return m_super == &cl; }

    /** checks if this class is the same as given class
        @param cl class to check against
     */
    bool isSame(const Class &cl) const 
		{ return &cl == this; }

    /** checks if this class is a base class of the given class or the same class;
        i.e. it checks if the given class can be casted to this class
        @return true if this class is a base class of the given class or the
                same class.
     */
    bool checkUpCast(const Class &cl) const 
		{  return isSame(cl) || isBase(cl); }

    /** checks if this class is a derived class of the given class or the same class;
        i.e. it checks if the given class can be casted to this class
        @return true if this class is a derived class of the given class or the
                same class.
     */
    bool checkDownCast(const Class &cl) const 
		{  return isSame(cl) || isDerived(cl); }

    /** checks if the given class is relative to this class. In other words,
        it checks if this class is a base class of the given class, or if they
        are the same class or the given class is a base class of this class.
        @param cl class to check against
        @return true if this class is a base class of the given class or the
                same class or if the given class is a base class of this class.
     */
    bool isRelative(const Class &cl) const 
		{ return isSame(cl) || isBase(cl) || cl.isBase(*this); }

    /** returns field from name
        @param name name of the field to search for
        @param searchSuper if true, super classes are searched if this class does not have the requested member
        @return the field of given name
        @exception UnknownFieldError
     */
    const Field &getField(const char *name, bool searchSuper = true) const;

    /** returns static field from name
        @param name name of the field to search for
        @param searchSuper if true, super classes are searched if this class does not have the requested member
        @return the field of given name
        @exception UnknownFieldError
     */
    const StaticField &getStaticField(const char *name, bool searchSuper = true) const;

    /** returns method from name
        @param name name of the method to search for
        @param searchSuper if true, super classes are searched if this class does not have the requested member
        @return the method of given id
        @exception UnknownMethodError
     */
    const Method &getMethod(const char *name, bool searchSuper = true) const;

    /** returns static method from name
        @param name name of the method to search for
        @param searchSuper if true, super classes are searched if this class does not have the requested member
        @return the method of given name
        @exception UnknownMethodError
     */
    const StaticMethod &getStaticMethod(const char *name, bool searchSuper = true) const;

    /** returns property from name
        @param name name of the property to search for
        @param searchSuper if true, super classes are searched if this class does not have the requested member
        @return the property of given name
        @exception UnknownPropertyError
     */
    const Property &getProperty(const char *name, bool searchSuper = true) const;

    /** returns property from name
        @param name name of the property to search for
        @param searchSuper if true, super classes are searched if this class does not have the requested member
        @return the property of given name
        @exception UnknownPropertyError
     */
    const Event & getEvent(const char *name, bool searchSuper = true) const;

    /** creates an instance of the class
        @return pointer to created object or NULL if the class does not inherit
                from Object
     */
    Object *createInstance() const 
		{ return m_creator ? m_creator->createInstance() : 0; }

private:
    typedef std::map<std::string, Field *> _FieldMap;
    typedef std::map<std::string, StaticField *> _StaticFieldMap;
    typedef std::map<std::string, Method *> _MethodMap;
    typedef std::map<std::string, StaticMethod *> _StaticMethodMap;
    typedef std::map<std::string, Property *> _PropertyMap;
    typedef std::map<std::string, Event *> _EventMap;

    const char *m_name;
		const std::type_info * m_typeinfo;
    const Class *m_super;
    FieldList m_fields;
    _FieldMap m_fieldMap;

    StaticFieldList m_staticFields;
    _StaticFieldMap m_staticFieldMap;

    MethodList m_methods;
    _MethodMap m_methodMap;

    StaticMethodList m_staticMethods;
    _StaticMethodMap m_staticMethodMap;

    PropertyList m_properties;
    _PropertyMap m_propertyMap;

    EventList m_events;
		_EventMap m_eventMap;

    __instance_creator_base__* m_creator;

    //adds a field
    void _addField(const Field &field);

    //adds a static field
    void _addStaticField(const StaticField &field);

    //adds a method
    void _addMethod(const Method & method, __callable__ * cb, boost::function_base * fbase);

    //adds a static method
    void _addStaticMethod(const StaticMethod &method, __callable__ *cb);

    //adds a property
    void _addProperty(const Property &prop, __property_base__ *handler);

    //adds a property
    void _addEvent(const Event & event);

    friend struct __register_field__;
    friend struct __register_static_field__;
    friend struct __register_method__;
    friend struct __register_static_method__;
    friend struct __register_property__;
    friend struct __register_event__;
};


/** Pseudo-class used when a class does not inherit from another class. It
    can not be instantiated.
 */
class NullClass 
{
public:
    static const Class * getClassStaticPtr() { return 0; }
private:
    NullClass() {}
    ~NullClass() {}
};

} // namespace reflection
} // namespace yake

#endif // _CLASS_H_