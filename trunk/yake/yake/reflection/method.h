#ifndef _METHOD_H_
#define _METHOD_H_

#include "reflection.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

/*****************************************************************************
    MACROS
 *****************************************************************************/

//callable base class macro with return type
#define __CALLABLE_BASE__(N) \
template<class R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> \
struct __callable_base##N##__ : public __callable__ \
{ \
    virtual R invoke(void *object __REPEAT(N, __ARG__, __COMMA__)) const = 0; \
};


//callable class macro with return type
#define __CALLABLE__(N)\
template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> struct __callable##N##__ : public __callable_base##N##__<R __REPEAT(N, __TYPE_ARG__, __COMMA__)> {\
    typedef R (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __NOTHING__));\
    MethodType method;\
    __callable##N##__(MethodType m) : method(m) {\
    }\
    virtual R invoke(void *object __REPEAT(N, __ARG__, __COMMA__)) const {\
        return (((C *)object)->*method)(__REPEAT(N, __PARAM__, __NOTHING__));\
    }\
};


//callable const base class macro with return type
#define __CALLABLE_CONST_BASE__(N)\
template <class R __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> struct __callable_const_base##N##__ : public __callable_base##N##__<R __REPEAT(N, __TYPE_ARG__, __COMMA__)> {\
    virtual R invoke(void *object __REPEAT(N, __ARG__, __COMMA__)) const = 0;\
};


//callable const class macro with return type
#define __CALLABLE_CONST__(N)\
template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> struct __callable_const##N##__ : public __callable_const_base##N##__<R __REPEAT(N, __TYPE_ARG__, __COMMA__)> {\
    typedef R (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __NOTHING__)) const;\
    MethodType method;\
    __callable_const##N##__(MethodType m) : method(m) {\
    }\
    virtual R invoke(void *object __REPEAT(N, __ARG__, __COMMA__)) const {\
        return (((const C *)object)->*method)(__REPEAT(N, __PARAM__, __NOTHING__));\
    }\
};


//callable base class macro with void return type
#define __CALLABLE_VOID_BASE__(N)\
template <__REPEAT(N, __TEMPLATE_ARG__, __NOTHING__)> struct __callable_void_base##N##__ : public __callable__ {\
    virtual void invoke(void *object __REPEAT(N, __ARG__, __COMMA__)) const = 0;\
};


//callable class macro with void return type
#define __CALLABLE_VOID__(N)\
template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> struct __callable_void##N##__ : public __callable_void_base##N##__<__REPEAT(N, __TYPE_ARG__, __NOTHING__)> {\
    typedef void (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __NOTHING__));\
    MethodType method;\
    __callable_void##N##__(MethodType m) : method(m) {\
    }\
    virtual void invoke(void *object __REPEAT(N, __ARG__, __COMMA__)) const {\
        (((C *)object)->*method)(__REPEAT(N, __PARAM__, __NOTHING__));\
    }\
};


//callable base class macro with void return type
#define __CALLABLE_CONST_VOID_BASE__(N)\
template <__REPEAT(N, __TEMPLATE_ARG__, __NOTHING__)> struct __callable_const_void_base##N##__ : public __callable_void_base##N##__<__REPEAT(N, __TYPE_ARG__, __NOTHING__)> {\
    virtual void invoke(void *object __REPEAT(N, __ARG__, __COMMA__)) const = 0;\
};


//callable class macro with void return type
#define __CALLABLE_CONST_VOID__(N)\
template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> struct __callable_const_void##N##__ : public __callable_const_void_base##N##__<__REPEAT(N, __TYPE_ARG__, __NOTHING__)> {\
    typedef void (C::*MethodType)(__REPEAT(N, __TYPE_ARG__, __NOTHING__)) const;\
    MethodType method;\
    __callable_const_void##N##__(MethodType m) : method(m) {\
    }\
    virtual void invoke(void *object __REPEAT(N, __ARG__, __COMMA__)) const {\
        (((const C *)object)->*method)(__REPEAT(N, __PARAM__, __NOTHING__));\
    }\
};

//macro of a inline method that accepts a method pointer and creates a callable for it
#define __CREATE_CALLABLE__(N)\
    template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> static inline __callable##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__)> *create(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __NOTHING__))) {\
        return new __callable##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__)>(method);\
    }


//macro of a inline method that accepts a method pointer and creates a const callable for it
#define __CREATE_CALLABLE_CONST__(N)\
    template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> static inline __callable_const##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__)> *create(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __NOTHING__)) const) {\
        return new __callable_const##N##__<R, C __REPEAT(N, __TYPE_ARG__, __COMMA__)>(method);\
    }


//void version
#define __CREATE_CALLABLE_VOID__(N)\
    template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> static inline __callable_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__)> *create(void (C::*method)(__REPEAT(N, __TYPE_ARG__, __NOTHING__))) {\
        return new __callable_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__)>(method);\
    }


//const void version
#define __CREATE_CALLABLE_CONST_VOID__(N)\
    template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> static inline __callable_const_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__)> *create(void (C::*method)(__REPEAT(N, __TYPE_ARG__, __NOTHING__)) const) {\
        return new __callable_const_void##N##__<C __REPEAT(N, __TYPE_ARG__, __COMMA__)>(method);\
    }


//callable generator
#define __CALLABLE_GENERATOR__(N)\
    template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> inline __callable__ *__create_callable__(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __NOTHING__))) {\
        return __callable_factory__<R>::create(method);\
    }


//const callable generator
#define __CALLABLE_GENERATOR_CONST__(N)\
    template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> inline __callable__ *__create_callable__(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __NOTHING__)) const) {\
        return __callable_factory__<R>::create(method);\
    }

//callable generator
#define __FUNCTION_BASE_GENERATOR__0 \
    template <class R, class C> \
		inline boost::function_base * __create_function_base__(R (C::*method)()) \
		{ \
			return new boost::function1<R, C*>(boost::bind(method, _1)); \
    }

//callable generator
#define __FUNCTION_BASE_GENERATOR__(N) \
    template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> \
		inline boost::function_base * __create_function_base__(R (C::*method)(__REPEAT(N, __TYPE_ARG__, __NOTHING__))) \
		{ \
			return new boost::function<R (C*, __REPEAT(N, __TYPE_ARG__, __NOTHING__))>(boost::bind(method, __PLACEHOLDERS(N))); \
    }

//declares a method
#define __METHOD__(ACCESS_ATTR, VIRTUAL, RETURN_TYPE, METHOD_NAME, METHOD_ARGS) \
private: \
    struct __method_##METHOD_NAME##__ \
		{ \
        __method_##METHOD_NAME##__() \
				{ \
            static reflection::__register_method__ reg(__create_callable__(&METHOD_NAME), __create_function_base__(&METHOD_NAME), getClassStaticPtr(), reflection::ACCESS_##ACCESS_ATTR, #RETURN_TYPE, #METHOD_NAME, #METHOD_ARGS, #VIRTUAL); \
        } \
    } __method_##METHOD_NAME##__; \
    friend struct __method_##METHOD_NAME##__; \
ACCESS_ATTR: \
    VIRTUAL RETURN_TYPE METHOD_NAME METHOD_ARGS

//macro that defines a non-const 'invoke' method with a return type
#define __INVOKE__(N)\
    template <class R, class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> void invoke(R &result, C *object __REPEAT(N, __ARG__, __COMMA__)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        if (!m_class->checkUpCast(object->getClass())) throw TypeMismatchError("object");\
        if (__IS_CONST__(object)) {\
            typedef const __callable_const_base##N##__<R __REPEAT(N, __TYPE_ARG__, __COMMA__)> CallableType;\
            CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
            if (!cb) throw TypeMismatchError(m_name);\
            result = cb->invoke((void *)object __REPEAT(N, __PARAM__, __COMMA__));\
        }\
        else {\
            typedef const __callable_base##N##__<R __REPEAT(N, __TYPE_ARG__, __COMMA__)> CallableType;\
            CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
            if (!cb) throw TypeMismatchError(m_name);\
            result = cb->invoke((void *)object __REPEAT(N, __PARAM__, __COMMA__));\
        }\
    }


//macro that defines an 'invoke' method without a return type
#define __INVOKE_VOID__(N)\
    template <class C __REPEAT(N, __TEMPLATE_ARG__, __COMMA__)> void invoke(C *object __REPEAT(N, __ARG__, __COMMA__)) const {\
        if (m_access != ACCESS_PUBLIC) throw IllegalAccessError(m_name);\
        if (!m_class->checkUpCast(object->getClass())) throw TypeMismatchError("object");\
        if (__IS_CONST__(object)) {\
            typedef const __callable_const_void_base##N##__<__REPEAT(N, __TYPE_ARG__, __NOTHING__)> CallableType;\
            CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
            if (!cb) throw TypeMismatchError(m_name);\
            cb->invoke((void *)object __REPEAT(N, __PARAM__, __COMMA__));\
        }\
        else {\
            typedef const __callable_void_base##N##__<__REPEAT(N, __TYPE_ARG__, __NOTHING__)> CallableType;\
            CallableType *cb = dynamic_cast<CallableType *>(m_callable);\
            if (!cb) throw TypeMismatchError(m_name);\
            cb->invoke((void *)object __REPEAT(N, __PARAM__, __COMMA__));\
        }\
    }


/** The METHOD macro is used to declare a reflected method.
    @param ACCESS_ATTR access attribute (public, protected or private)
    @param RETURN_TYPE the return type of the method
    @param METHOD_NAME name of the method
    @param METHOD_ARGS the method's arguments enclosed in parentheses
 */
#define RX_METHOD(ACCESS_ATTR, RETURN_TYPE, METHOD_NAME, METHOD_ARGS)\
    __METHOD__(ACCESS_ATTR, __NOT_VIRTUAL__, RETURN_TYPE, METHOD_NAME, METHOD_ARGS)


/** The VIRTUAL_METHOD macro is used to declare a reflected method that is
    virtual, i.e. can be overloaded by subclasses.
    @param ACCESS_ATTR access attribute (public, protected or private)
    @param VIRTUAL pass virtual or no_virtual
    @param RETURN_TYPE the return type of the method
    @param METHOD_NAME name of the method
    @param METHOD_ARGS the method's arguments enclosed in parentheses
 */
#define VIRTUAL_METHOD(ACCESS_ATTR, RETURN_TYPE, METHOD_NAME, METHOD_ARGS)\
    __METHOD__(ACCESS_ATTR, virtual, RETURN_TYPE, METHOD_NAME, METHOD_ARGS)



/*****************************************************************************
    CLASSES
 *****************************************************************************/

namespace reflection 
{

//callable void base 0
template <class T = int> struct __callable_void_base0__ : public __callable__ 
{
    virtual void invoke(void *object) const = 0;
};


//callable class macro with void return type with 0 params
template <class C> struct __callable_void0__ : public __callable_void_base0__<> 
{
    typedef void (C::*MethodType)();
    MethodType method;
    __callable_void0__(MethodType m) : method(m) {
    }
    virtual void invoke(void *object) const {
        (((C *)object)->*method)();
    }
};


//callable const base 0
template <class T = int> struct __callable_const_void_base0__ : public __callable_void_base0__<> 
{
    virtual void invoke(void *object) const = 0;
};


//callable class macro with void return type and 0 params
template <class C> struct __callable_const_void0__ : public __callable_const_void_base0__<> 
{
    typedef void (C::*MethodType)() const;
    MethodType method;
    __callable_const_void0__(MethodType m) : method(m) {
    }
    virtual void invoke(void *object) const {
        (((const C *)object)->*method)();
    }
};


//define callables
__CALLABLE_BASE__(0);
__CALLABLE_BASE__(1);
__CALLABLE_BASE__(2);
__CALLABLE_BASE__(3);
__CALLABLE_BASE__(4);
__CALLABLE_BASE__(5);
__CALLABLE_BASE__(6);
__CALLABLE_BASE__(7);
__CALLABLE_BASE__(8);
__CALLABLE_BASE__(9);
__CALLABLE_BASE__(10);
__CALLABLE_BASE__(11);
__CALLABLE_BASE__(12);
__CALLABLE_BASE__(13);
__CALLABLE_BASE__(14);
__CALLABLE_BASE__(15);
__CALLABLE_BASE__(16);
__CALLABLE_BASE__(17);
__CALLABLE_BASE__(18);
__CALLABLE_BASE__(19);
__CALLABLE_BASE__(20);
__CALLABLE__(0);
__CALLABLE__(1);
__CALLABLE__(2);
__CALLABLE__(3);
__CALLABLE__(4);
__CALLABLE__(5);
__CALLABLE__(6);
__CALLABLE__(7);
__CALLABLE__(8);
__CALLABLE__(9);
__CALLABLE__(10);
__CALLABLE__(11);
__CALLABLE__(12);
__CALLABLE__(13);
__CALLABLE__(14);
__CALLABLE__(15);
__CALLABLE__(16);
__CALLABLE__(17);
__CALLABLE__(18);
__CALLABLE__(19);
__CALLABLE__(20);


//define const callables
__CALLABLE_CONST_BASE__(0);
__CALLABLE_CONST_BASE__(1);
__CALLABLE_CONST_BASE__(2);
__CALLABLE_CONST_BASE__(3);
__CALLABLE_CONST_BASE__(4);
__CALLABLE_CONST_BASE__(5);
__CALLABLE_CONST_BASE__(6);
__CALLABLE_CONST_BASE__(7);
__CALLABLE_CONST_BASE__(8);
__CALLABLE_CONST_BASE__(9);
__CALLABLE_CONST_BASE__(10);
__CALLABLE_CONST_BASE__(11);
__CALLABLE_CONST_BASE__(12);
__CALLABLE_CONST_BASE__(13);
__CALLABLE_CONST_BASE__(14);
__CALLABLE_CONST_BASE__(15);
__CALLABLE_CONST_BASE__(16);
__CALLABLE_CONST_BASE__(17);
__CALLABLE_CONST_BASE__(18);
__CALLABLE_CONST_BASE__(19);
__CALLABLE_CONST_BASE__(20);
__CALLABLE_CONST__(0);
__CALLABLE_CONST__(1);
__CALLABLE_CONST__(2);
__CALLABLE_CONST__(3);
__CALLABLE_CONST__(4);
__CALLABLE_CONST__(5);
__CALLABLE_CONST__(6);
__CALLABLE_CONST__(7);
__CALLABLE_CONST__(8);
__CALLABLE_CONST__(9);
__CALLABLE_CONST__(10);
__CALLABLE_CONST__(11);
__CALLABLE_CONST__(12);
__CALLABLE_CONST__(13);
__CALLABLE_CONST__(14);
__CALLABLE_CONST__(15);
__CALLABLE_CONST__(16);
__CALLABLE_CONST__(17);
__CALLABLE_CONST__(18);
__CALLABLE_CONST__(19);
__CALLABLE_CONST__(20);


//define void callables
__CALLABLE_VOID_BASE__(1);
__CALLABLE_VOID_BASE__(2);
__CALLABLE_VOID_BASE__(3);
__CALLABLE_VOID_BASE__(4);
__CALLABLE_VOID_BASE__(5);
__CALLABLE_VOID_BASE__(6);
__CALLABLE_VOID_BASE__(7);
__CALLABLE_VOID_BASE__(8);
__CALLABLE_VOID_BASE__(9);
__CALLABLE_VOID_BASE__(10);
__CALLABLE_VOID_BASE__(11);
__CALLABLE_VOID_BASE__(12);
__CALLABLE_VOID_BASE__(13);
__CALLABLE_VOID_BASE__(14);
__CALLABLE_VOID_BASE__(15);
__CALLABLE_VOID_BASE__(16);
__CALLABLE_VOID_BASE__(17);
__CALLABLE_VOID_BASE__(18);
__CALLABLE_VOID_BASE__(19);
__CALLABLE_VOID_BASE__(20);
__CALLABLE_VOID__(1);
__CALLABLE_VOID__(2);
__CALLABLE_VOID__(3);
__CALLABLE_VOID__(4);
__CALLABLE_VOID__(5);
__CALLABLE_VOID__(6);
__CALLABLE_VOID__(7);
__CALLABLE_VOID__(8);
__CALLABLE_VOID__(9);
__CALLABLE_VOID__(10);
__CALLABLE_VOID__(11);
__CALLABLE_VOID__(12);
__CALLABLE_VOID__(13);
__CALLABLE_VOID__(14);
__CALLABLE_VOID__(15);
__CALLABLE_VOID__(16);
__CALLABLE_VOID__(17);
__CALLABLE_VOID__(18);
__CALLABLE_VOID__(19);
__CALLABLE_VOID__(20);


//define const void callables
__CALLABLE_CONST_VOID_BASE__(1);
__CALLABLE_CONST_VOID_BASE__(2);
__CALLABLE_CONST_VOID_BASE__(3);
__CALLABLE_CONST_VOID_BASE__(4);
__CALLABLE_CONST_VOID_BASE__(5);
__CALLABLE_CONST_VOID_BASE__(6);
__CALLABLE_CONST_VOID_BASE__(7);
__CALLABLE_CONST_VOID_BASE__(8);
__CALLABLE_CONST_VOID_BASE__(9);
__CALLABLE_CONST_VOID_BASE__(10);
__CALLABLE_CONST_VOID_BASE__(11);
__CALLABLE_CONST_VOID_BASE__(12);
__CALLABLE_CONST_VOID_BASE__(13);
__CALLABLE_CONST_VOID_BASE__(14);
__CALLABLE_CONST_VOID_BASE__(15);
__CALLABLE_CONST_VOID_BASE__(16);
__CALLABLE_CONST_VOID_BASE__(17);
__CALLABLE_CONST_VOID_BASE__(18);
__CALLABLE_CONST_VOID_BASE__(19);
__CALLABLE_CONST_VOID_BASE__(20);
__CALLABLE_CONST_VOID__(1);
__CALLABLE_CONST_VOID__(2);
__CALLABLE_CONST_VOID__(3);
__CALLABLE_CONST_VOID__(4);
__CALLABLE_CONST_VOID__(5);
__CALLABLE_CONST_VOID__(6);
__CALLABLE_CONST_VOID__(7);
__CALLABLE_CONST_VOID__(8);
__CALLABLE_CONST_VOID__(9);
__CALLABLE_CONST_VOID__(10);
__CALLABLE_CONST_VOID__(11);
__CALLABLE_CONST_VOID__(12);
__CALLABLE_CONST_VOID__(13);
__CALLABLE_CONST_VOID__(14);
__CALLABLE_CONST_VOID__(15);
__CALLABLE_CONST_VOID__(16);
__CALLABLE_CONST_VOID__(17);
__CALLABLE_CONST_VOID__(18);
__CALLABLE_CONST_VOID__(19);
__CALLABLE_CONST_VOID__(20);





//factory method for non-voids
template <class R> struct __callable_factory__ {
    __CREATE_CALLABLE__(0)
    __CREATE_CALLABLE__(1)
    __CREATE_CALLABLE__(2)
    __CREATE_CALLABLE__(3)
    __CREATE_CALLABLE__(4)
    __CREATE_CALLABLE__(5)
    __CREATE_CALLABLE__(6)
    __CREATE_CALLABLE__(7)
    __CREATE_CALLABLE__(8)
    __CREATE_CALLABLE__(9)
    __CREATE_CALLABLE__(10)
    __CREATE_CALLABLE__(11)
    __CREATE_CALLABLE__(12)
    __CREATE_CALLABLE__(13)
    __CREATE_CALLABLE__(14)
    __CREATE_CALLABLE__(15)
    __CREATE_CALLABLE__(16)
    __CREATE_CALLABLE__(17)
    __CREATE_CALLABLE__(18)
    __CREATE_CALLABLE__(19)
    __CREATE_CALLABLE__(20)
    __CREATE_CALLABLE_CONST__(0)
    __CREATE_CALLABLE_CONST__(1)
    __CREATE_CALLABLE_CONST__(2)
    __CREATE_CALLABLE_CONST__(3)
    __CREATE_CALLABLE_CONST__(4)
    __CREATE_CALLABLE_CONST__(5)
    __CREATE_CALLABLE_CONST__(6)
    __CREATE_CALLABLE_CONST__(7)
    __CREATE_CALLABLE_CONST__(8)
    __CREATE_CALLABLE_CONST__(9)
    __CREATE_CALLABLE_CONST__(10)
    __CREATE_CALLABLE_CONST__(11)
    __CREATE_CALLABLE_CONST__(12)
    __CREATE_CALLABLE_CONST__(13)
    __CREATE_CALLABLE_CONST__(14)
    __CREATE_CALLABLE_CONST__(15)
    __CREATE_CALLABLE_CONST__(16)
    __CREATE_CALLABLE_CONST__(17)
    __CREATE_CALLABLE_CONST__(18)
    __CREATE_CALLABLE_CONST__(19)
    __CREATE_CALLABLE_CONST__(20)
};


//factory method for non-voids
template <> struct __callable_factory__<void> {
    __CREATE_CALLABLE_VOID__(0)
    __CREATE_CALLABLE_VOID__(1)
    __CREATE_CALLABLE_VOID__(2)
    __CREATE_CALLABLE_VOID__(3)
    __CREATE_CALLABLE_VOID__(4)
    __CREATE_CALLABLE_VOID__(5)
    __CREATE_CALLABLE_VOID__(6)
    __CREATE_CALLABLE_VOID__(7)
    __CREATE_CALLABLE_VOID__(8)
    __CREATE_CALLABLE_VOID__(9)
    __CREATE_CALLABLE_VOID__(10)
    __CREATE_CALLABLE_VOID__(11)
    __CREATE_CALLABLE_VOID__(12)
    __CREATE_CALLABLE_VOID__(13)
    __CREATE_CALLABLE_VOID__(14)
    __CREATE_CALLABLE_VOID__(15)
    __CREATE_CALLABLE_VOID__(16)
    __CREATE_CALLABLE_VOID__(17)
    __CREATE_CALLABLE_VOID__(18)
    __CREATE_CALLABLE_VOID__(19)
    __CREATE_CALLABLE_VOID__(20)
    __CREATE_CALLABLE_CONST_VOID__(0)
    __CREATE_CALLABLE_CONST_VOID__(1)
    __CREATE_CALLABLE_CONST_VOID__(2)
    __CREATE_CALLABLE_CONST_VOID__(3)
    __CREATE_CALLABLE_CONST_VOID__(4)
    __CREATE_CALLABLE_CONST_VOID__(5)
    __CREATE_CALLABLE_CONST_VOID__(6)
    __CREATE_CALLABLE_CONST_VOID__(7)
    __CREATE_CALLABLE_CONST_VOID__(8)
    __CREATE_CALLABLE_CONST_VOID__(9)
    __CREATE_CALLABLE_CONST_VOID__(10)
    __CREATE_CALLABLE_CONST_VOID__(11)
    __CREATE_CALLABLE_CONST_VOID__(12)
    __CREATE_CALLABLE_CONST_VOID__(13)
    __CREATE_CALLABLE_CONST_VOID__(14)
    __CREATE_CALLABLE_CONST_VOID__(15)
    __CREATE_CALLABLE_CONST_VOID__(16)
    __CREATE_CALLABLE_CONST_VOID__(17)
    __CREATE_CALLABLE_CONST_VOID__(18)
    __CREATE_CALLABLE_CONST_VOID__(19)
    __CREATE_CALLABLE_CONST_VOID__(20)
};




//generators
__CALLABLE_GENERATOR__(0)
__CALLABLE_GENERATOR__(1)
__CALLABLE_GENERATOR__(2)
__CALLABLE_GENERATOR__(3)
__CALLABLE_GENERATOR__(4)
__CALLABLE_GENERATOR__(5)
__CALLABLE_GENERATOR__(6)
__CALLABLE_GENERATOR__(7)
__CALLABLE_GENERATOR__(8)
__CALLABLE_GENERATOR__(9)
__CALLABLE_GENERATOR__(10)
__CALLABLE_GENERATOR__(11)
__CALLABLE_GENERATOR__(12)
__CALLABLE_GENERATOR__(13)
__CALLABLE_GENERATOR__(14)
__CALLABLE_GENERATOR__(15)
__CALLABLE_GENERATOR__(16)
__CALLABLE_GENERATOR__(17)
__CALLABLE_GENERATOR__(18)
__CALLABLE_GENERATOR__(19)
__CALLABLE_GENERATOR__(20)
__CALLABLE_GENERATOR_CONST__(0)
__CALLABLE_GENERATOR_CONST__(1)
__CALLABLE_GENERATOR_CONST__(2)
__CALLABLE_GENERATOR_CONST__(3)
__CALLABLE_GENERATOR_CONST__(4)
__CALLABLE_GENERATOR_CONST__(5)
__CALLABLE_GENERATOR_CONST__(6)
__CALLABLE_GENERATOR_CONST__(7)
__CALLABLE_GENERATOR_CONST__(8)
__CALLABLE_GENERATOR_CONST__(9)
__CALLABLE_GENERATOR_CONST__(10)
__CALLABLE_GENERATOR_CONST__(11)
__CALLABLE_GENERATOR_CONST__(12)
__CALLABLE_GENERATOR_CONST__(13)
__CALLABLE_GENERATOR_CONST__(14)
__CALLABLE_GENERATOR_CONST__(15)
__CALLABLE_GENERATOR_CONST__(16)
__CALLABLE_GENERATOR_CONST__(17)
__CALLABLE_GENERATOR_CONST__(18)
__CALLABLE_GENERATOR_CONST__(19)
__CALLABLE_GENERATOR_CONST__(20)
__FUNCTION_BASE_GENERATOR__0
__FUNCTION_BASE_GENERATOR__(1)
__FUNCTION_BASE_GENERATOR__(2)
__FUNCTION_BASE_GENERATOR__(3)
__FUNCTION_BASE_GENERATOR__(4)
__FUNCTION_BASE_GENERATOR__(5)
__FUNCTION_BASE_GENERATOR__(6)
__FUNCTION_BASE_GENERATOR__(7)
__FUNCTION_BASE_GENERATOR__(8)
__FUNCTION_BASE_GENERATOR__(9)
__FUNCTION_BASE_GENERATOR__(10)
__FUNCTION_BASE_GENERATOR__(11)
__FUNCTION_BASE_GENERATOR__(12)
__FUNCTION_BASE_GENERATOR__(13)
__FUNCTION_BASE_GENERATOR__(14)
__FUNCTION_BASE_GENERATOR__(15)
__FUNCTION_BASE_GENERATOR__(16)
__FUNCTION_BASE_GENERATOR__(17)
__FUNCTION_BASE_GENERATOR__(18)
__FUNCTION_BASE_GENERATOR__(19)
__FUNCTION_BASE_GENERATOR__(20)

/** The Method class represents an object's method.
 */
class Method {
public:
    ///destructor
    ~Method() 
		{
        if(m_callable) delete m_callable;
				if(m_function_base) delete m_function_base;
    }

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

    /** returns true if the method is virtual.
        @return true if the method is virtual.
     */
    bool isVirtual() const 
		{ return m_virtual; }

    /** returns the boost function base.
        @return the boost function base.
     */
		boost::function_base & get_function_base() const
		{ return *m_function_base; }

    /* invokes a method that has a result
        @param result optional variable to store the result (if the method is non-void)
        @param object object to execute the method of
        @param t1...tn parameters of the invocation
        @exception IllegalAccessError if the method is not public
        @exception TypeMismatchError if the method has different arguments than
                   the ones passed to it
     */
    __INVOKE__(0)
    __INVOKE__(1)
    __INVOKE__(2)
    __INVOKE__(3)
    __INVOKE__(4)
    __INVOKE__(5)
    __INVOKE__(6)
    __INVOKE__(7)
    __INVOKE__(8)
    __INVOKE__(9)
    __INVOKE__(10)
    __INVOKE__(11)
    __INVOKE__(12)
    __INVOKE__(13)
    __INVOKE__(14)
    __INVOKE__(15)
    __INVOKE__(16)
    __INVOKE__(17)
    __INVOKE__(18)
    __INVOKE__(19)
    __INVOKE__(20)

    /* invokes a method that 'returns' void
        @param object object to execute the method of
        @param t1...tn parameters of the invocation
        @exception IllegalAccessError if the method is not public
        @exception TypeMismatchError if the method has different arguments than
                   the ones passed to it
     */
    __INVOKE_VOID__(0)
    __INVOKE_VOID__(1)
    __INVOKE_VOID__(2)
    __INVOKE_VOID__(3)
    __INVOKE_VOID__(4)
    __INVOKE_VOID__(5)
    __INVOKE_VOID__(6)
    __INVOKE_VOID__(7)
    __INVOKE_VOID__(8)
    __INVOKE_VOID__(9)
    __INVOKE_VOID__(10)
    __INVOKE_VOID__(11)
    __INVOKE_VOID__(12)
    __INVOKE_VOID__(13)
    __INVOKE_VOID__(14)
    __INVOKE_VOID__(15)
    __INVOKE_VOID__(16)
    __INVOKE_VOID__(17)
    __INVOKE_VOID__(18)
    __INVOKE_VOID__(19)
    __INVOKE_VOID__(20)

private:
    const Class *m_class;
    enum ACCESS_TYPE m_access;
    std::string m_id;
    const char *m_type;
    const char *m_name;
    const char *m_args;
    __callable__ *m_callable;
    boost::function_base * m_function_base;
    bool m_virtual:1;

    //default constructor
    Method(const Class *pclass, ACCESS_TYPE access, const char *type, const char *name, const char *args, bool virt) 
			: m_class(pclass),
        m_access(access),
        m_type(type),
        m_name(name),
        m_args(args),
        m_virtual(virt),
        m_callable(0),
				m_function_base(0),
        m_id(name) 
		{
        m_id += args;
    }

    friend class Class;
    friend struct __register_method__;
};


//internal class for registering a method
struct __register_method__ 
{
    __register_method__(__callable__ * cb, boost::function_base * fbase, const Class *pclass, enum ACCESS_TYPE access, const char *type, const char *name, const char *args, const char *virt) 
		{
        Method method(pclass, access, type, name, args, virt[0] == 'v' ? true : false);
        ((Class *)pclass)->_addMethod(method, cb, fbase);
    }
};

} // namespace reflection

#endif // _METHOD_H_