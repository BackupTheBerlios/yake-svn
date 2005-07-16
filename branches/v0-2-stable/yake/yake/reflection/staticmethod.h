#ifndef _STATICMETHOD_H_
#define _STATICMETHOD_H_

#include "reflection.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

/*****************************************************************************
    MACROS
 *****************************************************************************/

//static callable class macro with return type
#define __STATIC_CALLABLE__(N)\
template <class R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> struct __static_callable##N##__ : public __callable__ {\
    typedef R (*MethodType)(__REPEAT(N, __TYPE_ARG__, __NOTHING__));\
    MethodType method;\
    __static_callable##N##__(MethodType m) : method(m) {\
    }\
    R invoke(__REPEAT(N, __ARG__, __NOTHING__)) const {\
        return (*method)(__REPEAT(N, __PARAM__, __NOTHING__));\
    }\
};


//void version
#define __STATIC_CALLABLE_VOID__(N)\
template <__REPEAT(N, __TEMPLATE_ARG__, __NOTHING__)> struct __static_callable_void##N##__ : public __callable__ {\
    typedef void (*MethodType)(__REPEAT(N, __TYPE_ARG__, __NOTHING__));\
    MethodType method;\
    __static_callable_void##N##__(MethodType m) : method(m) {\
    }\
    void invoke(__REPEAT(N, __ARG__, __NOTHING__)) const {\
        (*method)(__REPEAT(N, __PARAM__, __NOTHING__));\
    }\
};

//macro to create a static callable // BUG ? R shadows R, so changed to RR
#define __CREATE_STATIC_CALLABLE__(N)\
    template <class RR __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> static inline __static_callable##N##__<RR __REPEAT(N, __TYPE_ARG__, __COMMA__)> *create(RR (*method)(__REPEAT(N, __TYPE_ARG__, __NOTHING__))) {\
        return new __static_callable##N##__<RR __REPEAT(N, __TYPE_ARG__, __COMMA__)>(method);\
    }


//void version for 0 params
#define __CREATE_STATIC_CALLABLE_VOID__0\
    static inline __static_callable_void0__ *create(void (*method)()) {\
        return new __static_callable_void0__(method);\
    }


//void version
#define __CREATE_STATIC_CALLABLE_VOID__(N)\
    template <__REPEAT(N, __TEMPLATE_ARG__, __NOTHING__)> static inline __static_callable_void##N##__<__REPEAT(N, __TYPE_ARG__, __NOTHING__)> *create(void (*method)(__REPEAT(N, __TYPE_ARG__, __NOTHING__))) {\
        return new __static_callable_void##N##__<__REPEAT(N, __TYPE_ARG__, __NOTHING__)>(method);\
    }


//static callable generator
#define __STATIC_CALLABLE_GENERATOR__(N)\
    template <class R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> inline __callable__ *__create_static_callable__(R (*method)(__REPEAT(N, __TYPE_ARG__, __NOTHING__))) {\
        return __static_callable_factory__<R>::create(method);\
    }

//static invoke void method with 0 params
#define __STATIC_INVOKE__0\
    template <class R> inline void invoke(R &result) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __static_callable0__<R> CallableType;\
        CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
        if (cb) {\
            result = cb->invoke();\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }


//static invoke non-void method
#define __STATIC_INVOKE__(N)\
    template <class R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> void invoke(R &result __REPEAT(N, __ARG__, __COMMA__)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __static_callable##N##__<R __REPEAT(N, __TYPE_ARG__, __COMMA__)> CallableType;\
        CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
        if (cb) {\
            result = cb->invoke(__REPEAT(N, __PARAM__, __NOTHING__));\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }


//static invoke void method with 0 params
#define __STATIC_INVOKE_VOID__0\
    inline void invokeVoid() const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __static_callable_void0__ CallableType;\
        CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
        if (cb) {\
            cb->invoke();\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }


//static invoke void method
#define __STATIC_INVOKE_VOID__(N)\
    template <__REPEAT(N, __TEMPLATE_ARG__, __NOTHING__)> void invokeVoid(__REPEAT(N, __ARG__, __NOTHING__)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        typedef const __static_callable_void##N##__<__REPEAT(N, __TYPE_ARG__, __NOTHING__)> CallableType;\
        CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
        if (cb) {\
            cb->invoke(__REPEAT(N, __PARAM__, __NOTHING__));\
            return;\
        }\
        throw TypeMismatchError(m_name);\
    }

/** The STATIC_METHOD macro is used to declare a reflected static method.
    @param ACCESS_ATTR access attribute (public, protected or private)
    @param RETURN_TYPE the return type of the method
    @param METHOD_NAME name of the method
    @param METHOD_ARGS the method's arguments enclosed in parentheses
 */
#define STATIC_METHOD(ACCESS_ATTR, RETURN_TYPE, METHOD_NAME, METHOD_ARGS)\
private:\
    struct __static_method_##METHOD_NAME##__ {\
        __static_method_##METHOD_NAME##__() {\
            static yake::reflection::__register_static_method__ reg(__create_static_callable__(&METHOD_NAME), getClassStaticPtr(), ACCESS_##ACCESS_ATTR, #RETURN_TYPE, #METHOD_NAME, #METHOD_ARGS);\
        }\
    } __static_method_##METHOD_NAME##__;\
    friend struct __static_method_##METHOD_NAME##__;\
ACCESS_ATTR :\
    static RETURN_TYPE METHOD_NAME METHOD_ARGS



/*****************************************************************************
    CLASSES
 *****************************************************************************/

namespace yake
{
namespace reflection
{

//void version for 0 params
struct __static_callable_void0__ : public __callable__ 
{
    typedef void (*MethodType)();
    MethodType method;
    __static_callable_void0__(MethodType m) : method(m) 
		{}
    void invoke() const 
		{ (*method)(); }
};

//static callable classes
__STATIC_CALLABLE__(0)
__STATIC_CALLABLE__(1)
__STATIC_CALLABLE__(2)
__STATIC_CALLABLE__(3)
__STATIC_CALLABLE__(4)
__STATIC_CALLABLE__(5)
__STATIC_CALLABLE__(6)
__STATIC_CALLABLE__(7)
__STATIC_CALLABLE__(8)
__STATIC_CALLABLE__(9)
__STATIC_CALLABLE__(10)
__STATIC_CALLABLE__(11)
__STATIC_CALLABLE__(12)
__STATIC_CALLABLE__(13)
__STATIC_CALLABLE__(14)
__STATIC_CALLABLE__(15)
__STATIC_CALLABLE__(16)
__STATIC_CALLABLE__(17)
__STATIC_CALLABLE__(18)
__STATIC_CALLABLE__(19)
__STATIC_CALLABLE__(20)
__STATIC_CALLABLE_VOID__(1)
__STATIC_CALLABLE_VOID__(2)
__STATIC_CALLABLE_VOID__(3)
__STATIC_CALLABLE_VOID__(4)
__STATIC_CALLABLE_VOID__(5)
__STATIC_CALLABLE_VOID__(6)
__STATIC_CALLABLE_VOID__(7)
__STATIC_CALLABLE_VOID__(8)
__STATIC_CALLABLE_VOID__(9)
__STATIC_CALLABLE_VOID__(10)
__STATIC_CALLABLE_VOID__(11)
__STATIC_CALLABLE_VOID__(12)
__STATIC_CALLABLE_VOID__(13)
__STATIC_CALLABLE_VOID__(14)
__STATIC_CALLABLE_VOID__(15)
__STATIC_CALLABLE_VOID__(16)
__STATIC_CALLABLE_VOID__(17)
__STATIC_CALLABLE_VOID__(18)
__STATIC_CALLABLE_VOID__(19)
__STATIC_CALLABLE_VOID__(20)

//factory method for static non-voids
template <class R> struct __static_callable_factory__ 
{
    __CREATE_STATIC_CALLABLE__(0)
    __CREATE_STATIC_CALLABLE__(1)
    __CREATE_STATIC_CALLABLE__(2)
    __CREATE_STATIC_CALLABLE__(3)
    __CREATE_STATIC_CALLABLE__(4)
    __CREATE_STATIC_CALLABLE__(5)
    __CREATE_STATIC_CALLABLE__(6)
    __CREATE_STATIC_CALLABLE__(7)
    __CREATE_STATIC_CALLABLE__(8)
    __CREATE_STATIC_CALLABLE__(9)
    __CREATE_STATIC_CALLABLE__(10)
    __CREATE_STATIC_CALLABLE__(11)
    __CREATE_STATIC_CALLABLE__(12)
    __CREATE_STATIC_CALLABLE__(13)
    __CREATE_STATIC_CALLABLE__(14)
    __CREATE_STATIC_CALLABLE__(15)
    __CREATE_STATIC_CALLABLE__(16)
    __CREATE_STATIC_CALLABLE__(17)
    __CREATE_STATIC_CALLABLE__(18)
    __CREATE_STATIC_CALLABLE__(19)
    __CREATE_STATIC_CALLABLE__(20)
};


//factory method for static voids
template <> struct __static_callable_factory__<void> 
{
    __CREATE_STATIC_CALLABLE_VOID__0
    __CREATE_STATIC_CALLABLE_VOID__(1)
    __CREATE_STATIC_CALLABLE_VOID__(2)
    __CREATE_STATIC_CALLABLE_VOID__(3)
    __CREATE_STATIC_CALLABLE_VOID__(4)
    __CREATE_STATIC_CALLABLE_VOID__(5)
    __CREATE_STATIC_CALLABLE_VOID__(6)
    __CREATE_STATIC_CALLABLE_VOID__(7)
    __CREATE_STATIC_CALLABLE_VOID__(8)
    __CREATE_STATIC_CALLABLE_VOID__(9)
    __CREATE_STATIC_CALLABLE_VOID__(10)
    __CREATE_STATIC_CALLABLE_VOID__(11)
    __CREATE_STATIC_CALLABLE_VOID__(12)
    __CREATE_STATIC_CALLABLE_VOID__(13)
    __CREATE_STATIC_CALLABLE_VOID__(14)
    __CREATE_STATIC_CALLABLE_VOID__(15)
    __CREATE_STATIC_CALLABLE_VOID__(16)
    __CREATE_STATIC_CALLABLE_VOID__(17)
    __CREATE_STATIC_CALLABLE_VOID__(18)
    __CREATE_STATIC_CALLABLE_VOID__(19)
    __CREATE_STATIC_CALLABLE_VOID__(20)
};

//generators
__STATIC_CALLABLE_GENERATOR__(0)
__STATIC_CALLABLE_GENERATOR__(1)
__STATIC_CALLABLE_GENERATOR__(2)
__STATIC_CALLABLE_GENERATOR__(3)
__STATIC_CALLABLE_GENERATOR__(4)
__STATIC_CALLABLE_GENERATOR__(5)
__STATIC_CALLABLE_GENERATOR__(6)
__STATIC_CALLABLE_GENERATOR__(7)
__STATIC_CALLABLE_GENERATOR__(8)
__STATIC_CALLABLE_GENERATOR__(9)
__STATIC_CALLABLE_GENERATOR__(10)
__STATIC_CALLABLE_GENERATOR__(11)
__STATIC_CALLABLE_GENERATOR__(12)
__STATIC_CALLABLE_GENERATOR__(13)
__STATIC_CALLABLE_GENERATOR__(14)
__STATIC_CALLABLE_GENERATOR__(15)
__STATIC_CALLABLE_GENERATOR__(16)
__STATIC_CALLABLE_GENERATOR__(17)
__STATIC_CALLABLE_GENERATOR__(18)
__STATIC_CALLABLE_GENERATOR__(19)
__STATIC_CALLABLE_GENERATOR__(20)

/** The StaticMethod class represents an object's static method.
 */
class StaticMethod 
{
public:
    ///destructor
    ~StaticMethod() 
		{ if (m_callable) delete m_callable; }

    /** returns the class that the member is declared into.
        @return the class that the member is declared into.
     */
    const Class &getClass() const 
		{ return *m_class; }

    /** returns the method's access.
        @return the method's access.
     */
    const ACCESS_TYPE getAccess() const 
		{ return m_access; }

    /** returns the method's type (return type).
        @return the method's type.
     */
    const char *getType() const 
		{ return m_type; }

    /** returns the method's name.
        @return the method's name.
     */
    const char *getName() const 
		{ return m_name; }

    /** returns the method's id (name + arguments).
        @return the method's name.
     */
    const char *getId() const 
		{ return m_id.c_str(); }

    /** returns the method's arguments.
        @return the method's arguments.
     */
    const char *getArgs() const 
		{ return m_args; }

    /** invokes the non-void method
        @param result optional variable to store the result (if the method is non-void)
        @param t1...tn parameters of the invocation
        @exception IllegalAccessError if the method is not public
        @exception TypeMismatchError if the method has different arguments than
                   the ones passed to it
     */
    __STATIC_INVOKE__0
    __STATIC_INVOKE__(1)
    __STATIC_INVOKE__(2)
    __STATIC_INVOKE__(3)
    __STATIC_INVOKE__(4)
    __STATIC_INVOKE__(5)
    __STATIC_INVOKE__(6)
    __STATIC_INVOKE__(7)
    __STATIC_INVOKE__(8)
    __STATIC_INVOKE__(9)
    __STATIC_INVOKE__(10)
    __STATIC_INVOKE__(11)
    __STATIC_INVOKE__(12)
    __STATIC_INVOKE__(13)
    __STATIC_INVOKE__(14)
    __STATIC_INVOKE__(15)
    __STATIC_INVOKE__(16)
    __STATIC_INVOKE__(17)
    __STATIC_INVOKE__(18)
    __STATIC_INVOKE__(19)
    __STATIC_INVOKE__(20)

    /** invokes the void method
        @param t1...tn parameters of the invocation
        @exception IllegalAccessError if the method is not public
        @exception TypeMismatchError if the method has different arguments than
                   the ones passed to it
     */
    __STATIC_INVOKE_VOID__0
    __STATIC_INVOKE_VOID__(1)
    __STATIC_INVOKE_VOID__(2)
    __STATIC_INVOKE_VOID__(3)
    __STATIC_INVOKE_VOID__(4)
    __STATIC_INVOKE_VOID__(5)
    __STATIC_INVOKE_VOID__(6)
    __STATIC_INVOKE_VOID__(7)
    __STATIC_INVOKE_VOID__(8)
    __STATIC_INVOKE_VOID__(9)
    __STATIC_INVOKE_VOID__(10)
    __STATIC_INVOKE_VOID__(11)
    __STATIC_INVOKE_VOID__(12)
    __STATIC_INVOKE_VOID__(13)
    __STATIC_INVOKE_VOID__(14)
    __STATIC_INVOKE_VOID__(15)
    __STATIC_INVOKE_VOID__(16)
    __STATIC_INVOKE_VOID__(17)
    __STATIC_INVOKE_VOID__(18)
    __STATIC_INVOKE_VOID__(19)
    __STATIC_INVOKE_VOID__(20)

private:
    const Class *m_class;
    enum ACCESS_TYPE m_access;
    std::string m_id;
    const char *m_type;
    const char *m_name;
    const char *m_args;
    __callable__ *m_callable;

    //default constructor
    StaticMethod(const Class *pclass, ACCESS_TYPE access, const char *type, const char *name, const char *args) 
			: m_class(pclass),
        m_access(access),
        m_type(type),
        m_name(name),
        m_args(args),
        m_callable(0),
        m_id(name) 
		{
        m_id += args;
    }

    friend class Class;
    friend struct __register_static_method__;
};

//internal class for registering a static method
struct __register_static_method__ 
{
    __register_static_method__(__callable__ *cb, const Class *pclass, enum ACCESS_TYPE access, const char *type, const char *name, const char *args) 
		{
        StaticMethod method(pclass, access, type, name, args);
        ((Class *)pclass)->_addStaticMethod(method, cb);
    }
};

} // namespace reflection
} // namespace yake

#endif // _STATICMETHOD_H_