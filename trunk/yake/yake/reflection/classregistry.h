#ifndef _CLASSREGISTRY_H_
#define _CLASSREGISTRY_H_

#include "reflection.h"

namespace reflection
{

/** Static class that represents a repository of class objects that can
    create instances at run-time.
 */
class ClassRegistry  
{
public:
    ///list of classes type
    typedef std::list<const Class *> ClassList;

    /** returns the list of registered classes
        @return the list of registered classes
     */
    static const ClassList &getClasses() 
		{ return m_classList; }

    /** returns a registered class from the name of the class
        @param name NULL-terminated string of the class name
        @return the found class.
        @exception UnknownClassError thrown if the class does not exist
     */
    static const Class &getClass(const char *name);

    /** registers a class in the registry; if the class is already registered,
        it does nothing.
        @param class_ class to register
        @exception
     */
    static void registerClass(const Class &class_);

    /** unregisters a class from the registry. If the class is already unregistered,
        it does nothing.
        @param class_ class to register
     */
    static void unregisterClass(const Class &class_);

    /** creates an instance of a class. The class must be derived from
        reflection::Object.
        @param name name of the class to create an instance from
        @return pointer to created object; if NULL, the class does not exist
                or does not inherit from reflection::Object
     */
    static reflection::Object *createInstance(const char *name);

private:
    typedef std::map<std::string, const Class *> ClassMap;
    static ClassList m_classList;
    static ClassMap m_classMap;
};

} // namespace reflection

#endif _CLASSREGISTRY_H_