#ifndef _EVENT_H_
#define _EVENT_H_

#include "reflection.h"

#include "events.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

#define classoffset(class, super) ((unsigned long) static_cast<super*>((class*)8) - 8)

/*****************************************************************************
    MACROS
*****************************************************************************/

#define REMOVE_BRACES(ARG1) ARG1

// todo: use construct_type_from_arbitry_args to construct an event class for typeid()
#define RX_EVENT(ACCESS_ATTR, EVENT_NAME, ARGS) \
private: \
    struct __event_##EVENT_NAME##__ \
		{ \
        __event_##EVENT_NAME##__() \
				{ \
					static __register_event__ reg(__OFFSET__(ClassType, EVENT_NAME), typeid(reflection::event< REMOVE_BRACES##ARGS >), getClassStaticPtr(), ACCESS_##ACCESS_ATTR, #EVENT_NAME); \
        } \
    } __event_##EVENT_NAME##__; \
    friend struct __event_##EVENT_NAME##__; \
ACCESS_ATTR: \
    reflection::event< REMOVE_BRACES##ARGS > EVENT_NAME

// todo typeid is wrong, should point to rx::event ...
#define RX_CUSTOM_EVENT(ACCESS_ATTR, EVENT_CLASS, EVENT_NAME) \
private: \
    struct __event_##EVENT_NAME##__ \
		{ \
        __event_##EVENT_NAME##__() \
				{ \
					static __register_event__ reg(__OFFSET__(ClassType, EVENT_NAME), typeid(EVENT_CLASS), getClassStaticPtr(), ACCESS_##ACCESS_ATTR, #EVENT_NAME); \
        } \
    } __event_##EVENT_NAME##__; \
    friend struct __event_##EVENT_NAME##__; \
ACCESS_ATTR: \
    EVENT_CLASS EVENT_NAME

/*****************************************************************************
    CLASSES
 *****************************************************************************/

namespace reflection
{

namespace { struct Null {}; }

/** The Event class represents an object's event. */
class Event 
{
public:
    /** returns the class that the member is declared into.
        @return the class that the member is declared into. */
    const Class & getClass() const 
		{ return * m_class; }

    /** returns the method's access.
        @return the method's access. */
    const ACCESS_TYPE getAccess() const 
		{ return m_access; }

    /** returns the method's name.
        @return the method's name. */
    const char * getName() const 
		{ return m_name; }

    template<class Object>
		void attach_handler(Object * object, const boost::function_base & this_handler) const
		{
			// check
      if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
      if (!m_class->checkUpCast(object->getClass())) throw TypeMismatchError("object");
			
			// this pointer + member offset + interface offset => ptr to the base interface
			event_base * this_event = reinterpret_cast<event_base*>(((char*)object) + m_offset + classoffset(event<Null>, event_base));
			this_event->attach_handler(this_handler);
		}

    template<class Object, class ObjectHandler>
		void attach_handler(Object * object, const Method & this_handler, ObjectHandler * object_handler) const
		{
			// check
      if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
      if (!m_class->checkUpCast(object->getClass())) throw TypeMismatchError("object");

			// this pointer + member offset + interface offset => ptr to the base interface
			rx_event_base * this_event = reinterpret_cast<rx_event_base*>(((char*)object) + m_offset + classoffset(event<Null>, rx_event_base));
			this_event->attach_handler(object_handler, this_handler);
		}

    template<class Object, class Arg1> 
		void fire(Object * object, const Arg1 & arg1) const 
		{
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
        if (!m_class->checkUpCast(object->getClass())) throw TypeMismatchError("object");
				if (typeid(reflection::event<Arg1>) != m_typeinfo) throw TypeMismatchError("event");
				reflection::event<Arg1> * this_event = reinterpret_cast<reflection::event<Arg1>*>(((char*)object) + m_offset);
				this_event->operator()(arg1);
    }

    template<class Object, class Arg1, class Arg2> 
		void fire(Object * object, const Arg1 & arg1, const Arg2 & arg2) const 
		{
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);
        if (!m_class->checkUpCast(object->getClass())) throw TypeMismatchError("object");
				if (typeid(reflection::event<Arg1>) != m_typeinfo) throw TypeMismatchError("event");
				reflection::event<Arg1, Arg2> * this_event = reinterpret_cast<reflection::event<Arg1, Arg2>*>(((char *)object) + m_offset);
				this_event->operator()(arg1, arg2);
    }

private:
    const Class * m_class;
    enum ACCESS_TYPE m_access;
    const char * m_name;
    const std::type_info & m_typeinfo;
    unsigned long m_offset;

    //default constructor
    Event(unsigned long offset, const std::type_info & typeinfo, const Class * pclass, enum ACCESS_TYPE access, const char *name) 
			: m_offset(offset),
        m_typeinfo(typeinfo),
				m_class(pclass),
        m_access(access),
        m_name(name)
		{}

    friend class Class;
    friend struct __register_event__;
};

// internal class for registering an event
struct __register_event__ 
{
    __register_event__(unsigned long offset, const std::type_info & typeinfo, const Class * pclass, enum ACCESS_TYPE access, const char * name) 
		{
        Event event(offset, typeinfo, pclass, access, name);
        ((Class *)pclass)->_addEvent(event);
    }
};

} // namespace reflection

#endif // _EVENT_H_