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
#include "reflection.h"
#include <algorithm>
using namespace std;

namespace yake
{
namespace reflection 
{


/*****************************************************************************
    Class PUBLIC
 *****************************************************************************/


/** returns field from name
    @param name name of the field to search for
    @param searchSuper if true, super classes are searched if this class does not have the requested member
    @return the field of given name
    @exception UnknownFieldError
 */
const Field &Class::getField(const char *name, bool searchSuper/* = true*/) const
{
    _FieldMap::const_iterator it = m_fieldMap.find(name);
    if (it == m_fieldMap.end()) {
        if (searchSuper && m_super) return m_super->getField(name, true);
        throw UnknownFieldError(name);
    }
    return *it->second;
}


/** returns static field from name
    @param name name of the field to search for
    @param searchSuper if true, super classes are searched if this class does not have the requested member
    @return the field of given name
    @exception UnknownFieldError
 */
const StaticField &Class::getStaticField(const char *name, bool searchSuper/* = true*/) const
{
    _StaticFieldMap::const_iterator it = m_staticFieldMap.find(name);
    if (it == m_staticFieldMap.end()) {
        if (searchSuper && m_super) return m_super->getStaticField(name, true);
        throw UnknownFieldError(name);
    }
    return *it->second;
}


/** returns method from name
    @param name name of the method to search for
    @param searchSuper if true, super classes are searched if this class does not have the requested member
    @return the method of given id
    @exception UnknownMethodError
 */
const Method &Class::getMethod(const char *name, bool searchSuper/* = true*/) const
{
    _MethodMap::const_iterator it = m_methodMap.find(name);
    if (it == m_methodMap.end()) {
        if (searchSuper && m_super) return m_super->getMethod(name, true);
        throw UnknownMethodError(name);
    }
    return *it->second;
}


/** returns static method from name
    @param name name of the method to search for
    @param searchSuper if true, super classes are searched if this class does not have the requested member
    @return the method of given name
    @exception UnknownMethodError
 */
const StaticMethod &Class::getStaticMethod(const char *name, bool searchSuper/* = true*/) const
{
    _StaticMethodMap::const_iterator it = m_staticMethodMap.find(name);
    if (it == m_staticMethodMap.end()) {
        if (searchSuper && m_super) return m_super->getStaticMethod(name, true);
        throw UnknownMethodError(name);
    }
    return *it->second;
}


/** returns property from name
    @param name name of the property to search for
    @param searchSuper if true, super classes are searched if this class does not have the requested member
    @return the property of given name
    @exception UnknownPropertyError
 */
const Property &Class::getProperty(const char *name, bool searchSuper/* = true*/) const
{
    _PropertyMap::const_iterator it = m_propertyMap.find(name);
    if (it == m_propertyMap.end()) {
        if (searchSuper && m_super) return m_super->getProperty(name, true);
        throw UnknownPropertyError(name);
    }
    return *it->second;
}

/** returns property from name
    @param name name of the property to search for
    @param searchSuper if true, super classes are searched if this class does not have the requested member
    @return the property of given name
    @exception UnknownPropertyError
 */
const Event & Class::getEvent(const char *name, bool searchSuper/* = true*/) const
{
    _EventMap::const_iterator it = m_eventMap.find(name);
    if (it == m_eventMap.end()) {
        if (searchSuper && m_super) return m_super->getEvent(name, true);
        throw UnknownEventError(name);
    }
    return *it->second;
}


/*****************************************************************************
    Class PRIVATE
 *****************************************************************************/


//adds a field
void Class::_addField(const Field &field)
{
    m_fields.push_back(field);
    m_fieldMap[field.m_name] = &m_fields.back();
}


//adds a static field
void Class::_addStaticField(const StaticField &field)
{
    m_staticFields.push_back(field);
    m_staticFieldMap[field.m_name] = &m_staticFields.back();
}


//adds a method
void Class::_addMethod(const Method & method, __callable__ * cb, boost::function_base * fbase)
{
    m_methods.push_back(method);
    Method &m = m_methods.back();
    m.m_callable = cb;
		m.m_function_base = fbase;
    m_methodMap[method.m_name] = &m;
}


//adds a static method
void Class::_addStaticMethod(const StaticMethod &method, __callable__ *cb)
{
    m_staticMethods.push_back(method);
    StaticMethod &m = m_staticMethods.back();
    m.m_callable = cb;
    m_staticMethodMap[method.m_name] = &m;
}


//adds a property
void Class::_addProperty(const Property &prop, __property_base__ *handler)
{
    m_properties.push_back(prop);
    Property &p = m_properties.back();
    p.m_handler = handler;
    m_propertyMap[prop.m_name] = &p;
}

//adds an event
void Class::_addEvent(const Event & event)
{
    m_events.push_back(event);
    m_eventMap[event.m_name] = &m_events.back();
}

/*****************************************************************************
    ClassRegistry PUBLIC
 *****************************************************************************/


/** returns a registered class from the name of the class
    @param name NULL-terminated string of the class name
    @return the found class.
    @exception UnknownClassError thrown if the class does not exist
 */
const Class &ClassRegistry::getClass(const char *name)
{
    ClassMap::iterator it = m_classMap.find(name);
    if (it == m_classMap.end()) throw UnknownClassError(name);
    return *it->second;
}


/** registers a class in the registry; if the class is already registered,
    it does nothing.
    @param class_ class to register
    @exception
 */
void ClassRegistry::registerClass(const Class &class_)
{
    ClassMap::iterator it = m_classMap.find(class_.getName());
    if (it != m_classMap.end()) return;
    m_classList.push_back(&class_);
    m_classMap[class_.getName()] = &class_;
}


/** unregisters a class from the registry. If the class is already unregistered,
    it does nothing.
    @param class_ class to register
 */
void ClassRegistry::unregisterClass(const Class &class_)
{
    ClassMap::iterator it = m_classMap.find(class_.getName());
    if (it == m_classMap.end()) return;
    m_classMap.erase(it);
    m_classList.erase(find(m_classList.begin(), m_classList.end(), &class_));
}


/** creates an instance of a class. The class must be derived from
    yake::reflection::Object.
    @param name name of the class to create an instance from
    @return pointer to created object; if NULL, the class does not exist
            or does not inherit from yake::reflection::Object
 */
Object *ClassRegistry::createInstance(const char *name)
{
    ClassMap::iterator it = m_classMap.find(name);
    if (it == m_classMap.end()) return 0;
    const Class &cl = *it->second;
    return cl.createInstance();
}


/*****************************************************************************
    ClassRegistry PRIVATE
 *****************************************************************************/


ClassRegistry::ClassList ClassRegistry::m_classList;
ClassRegistry::ClassMap ClassRegistry::m_classMap;

Registered::InstanceList Registered::instances_list;
Registered::InstanceMap Registered::instances_map;

} // namespace reflection
} // namespace yake