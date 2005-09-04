#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "reflection.h"

namespace yake
{
namespace reflection 
{

/** Exception type that is the base class for all this library's exceptions.
 */
class Exception : public std::runtime_error 
{
public:
    /** the default constructor
        @param what the item that caused the error
     */
    Exception(const std::string &what) : std::runtime_error(what) 
		{}
};


/** Exception thrown when there is a type mismatch.
 */
class TypeMismatchError : public Exception 
{
public:
    /** the default constructor
        @param what the item that caused the error
     */
    TypeMismatchError(const std::string &what) : Exception(what) 
		{}
};


/** Exception thrown when there is an illegal access error.
 */
class IllegalAccessError : public Exception 
{
public:
    /** the default constructor
        @param what the item that caused the error
     */
    IllegalAccessError(const std::string &what) : Exception(what) 
		{}
};


/** Exception thrown when an unknown field has been requested.
 */
class UnknownFieldError : public Exception 
{
public:
    /** the default constructor
        @param what the item that caused the error
     */
    UnknownFieldError(const std::string &what) : Exception(what) 
		{}
};


/** Exception thrown when an unknown method has been requested.
 */
class UnknownMethodError : public Exception 
{
public:
    /** the default constructor
        @param what the item that caused the error
     */
    UnknownMethodError(const std::string &what) : Exception(what) 
		{}
};


/** Exception thrown when an unknown property has been requested.
 */
class UnknownPropertyError : public Exception 
{
public:
    /** the default constructor
        @param what the item that caused the error
     */
    UnknownPropertyError(const std::string &what) : Exception(what) 
		{}
};

/** Exception thrown when an unknown event has been requested.
 */
class UnknownEventError : public Exception 
{
public:
    /** the default constructor
        @param what the item that caused the error */
    UnknownEventError(const std::string & what) : Exception(what) {}
};


/** Exception thrown when there the superclass of a class was asked to be
    returned but there was no superclass.
 */
class SuperClassError : public Exception 
{
public:
    /** the default constructor
        @param what the item that caused the error
     */
    SuperClassError(const std::string &what) : Exception(what) 
		{}
};


/** Exception thrown when an unknown class has been requested.
 */
class UnknownClassError : public Exception 
{
public:
    /** the default constructor
        @param what the item that caused the error
     */
    UnknownClassError(const std::string &what) : Exception(what) 
		{}
};

} // namespace reflection
} // namespace yake

#endif // _PROPERTY_H_