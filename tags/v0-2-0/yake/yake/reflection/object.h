#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "reflection.h"

namespace yake
{
namespace reflection 
{

class NullClass;

/** Base class for all objects that are to be used through the class registry.
    It is an abstract class.
 */
class Object 
{
public:
		CLASS(yake::reflection::Object, yake::reflection::NullClass);

    ///the default constructor
    Object() 
		{}

    ///destructor
    virtual ~Object() 
		{}
};

} // namespace reflection
} // namespace yake

#endif // _OBJECT_H_