#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "reflection.h"

namespace reflection
{

/** Base class for all objects that are to be used through the class registry.
    It is an abstract class.
 */
class Object 
{
public:
    CLASS(reflection::Object, reflection::NullClass);

    ///the default constructor
    Object() 
		{}

    ///destructor
    virtual ~Object() 
		{}
};

} // namespace reflection

#endif // _OBJECT_H_