#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "reflection.h"

/*****************************************************************************
    MACROS
 *****************************************************************************/

// removes the braces of (int, float, bool) and returns int, float, bool, null, null ...
// [used by EVENT( ..., (int, float, bool), ... ) etc.]
#define REMOVE_BRACES(arg1, arg2, arg3) typename get_type_or_null<arg1>::type, typename get_type_or_null<arg2>::type/*todo , typename get_type_or_null<arg3>::type*/

// access synonyms used in reflection
#define ACCESS_public        ACCESS_PUBLIC
#define ACCESS_protected     ACCESS_PROTECTED
#define ACCESS_private       ACCESS_PRIVATE


// comma
#define __COMMA_IF0
#define __COMMA_IF1   ,
#define __COMMA_IF(S) __COMMA_IF##S


// repeaters
#define __REPEAT0(M, S)
#define __REPEAT1(M, S)         __COMMA_IF(S) M(1)
#define __REPEAT2(M, S)         __REPEAT1(M, S)  , M(2)
#define __REPEAT3(M, S)         __REPEAT2(M, S)  , M(3)
#define __REPEAT4(M, S)         __REPEAT3(M, S)  , M(4)
#define __REPEAT5(M, S)         __REPEAT4(M, S)  , M(5)
#define __REPEAT6(M, S)         __REPEAT5(M, S)  , M(6)
#define __REPEAT7(M, S)         __REPEAT6(M, S)  , M(7)
#define __REPEAT8(M, S)         __REPEAT7(M, S)  , M(8)
#define __REPEAT9(M, S)         __REPEAT8(M, S)  , M(9)
#define __REPEAT10(M, S)        __REPEAT9(M, S)  , M(10)
#define __REPEAT11(M, S)        __REPEAT10(M, S) , M(11)
#define __REPEAT12(M, S)        __REPEAT11(M, S) , M(12)
#define __REPEAT13(M, S)        __REPEAT12(M, S) , M(13)
#define __REPEAT14(M, S)        __REPEAT13(M, S) , M(14)
#define __REPEAT15(M, S)        __REPEAT14(M, S) , M(15)
#define __REPEAT16(M, S)        __REPEAT15(M, S) , M(16)
#define __REPEAT17(M, S)        __REPEAT16(M, S) , M(17)
#define __REPEAT18(M, S)        __REPEAT17(M, S) , M(18)
#define __REPEAT19(M, S)        __REPEAT18(M, S) , M(19)
#define __REPEAT20(M, S)        __REPEAT19(M, S) , M(20)
#define __REPEAT(N, M, S)       __REPEAT##N(M, S)

// placeholders
#define __PLACEHOLDERS0()      _1
#define __PLACEHOLDERS1()      __PLACEHOLDERS0()  ,_2
#define __PLACEHOLDERS2()      __PLACEHOLDERS1()  ,_3
#define __PLACEHOLDERS3()      __PLACEHOLDERS2()  ,_4
#define __PLACEHOLDERS4()      __PLACEHOLDERS3()  ,_5
#define __PLACEHOLDERS5()      __PLACEHOLDERS4()  ,_6
#define __PLACEHOLDERS6()      __PLACEHOLDERS5()  ,_7
#define __PLACEHOLDERS7()      __PLACEHOLDERS6()  ,_8
#define __PLACEHOLDERS8()      __PLACEHOLDERS7()  ,_9
#define __PLACEHOLDERS9()      __PLACEHOLDERS8()  ,_10
#define __PLACEHOLDERS10()     __PLACEHOLDERS9()  ,_11
#define __PLACEHOLDERS11()     __PLACEHOLDERS10() ,_12
#define __PLACEHOLDERS12()     __PLACEHOLDERS11() ,_13
#define __PLACEHOLDERS13()     __PLACEHOLDERS12() ,_14
#define __PLACEHOLDERS14()     __PLACEHOLDERS13() ,_15
#define __PLACEHOLDERS15()     __PLACEHOLDERS14() ,_16
#define __PLACEHOLDERS16()     __PLACEHOLDERS15() ,_17
#define __PLACEHOLDERS17()     __PLACEHOLDERS16() ,_18
#define __PLACEHOLDERS18()     __PLACEHOLDERS17() ,_19
#define __PLACEHOLDERS19()     __PLACEHOLDERS18() ,_20
#define __PLACEHOLDERS20()     __PLACEHOLDERS19() ,_21
#define __PLACEHOLDERS(N)			 __PLACEHOLDERS##N()

// various defs needed for parameters
#define __MAX_PARAMS__       20
#define __NOTHING__          0
#define __COMMA__            1
#define __TEMPLATE_ARG__(N)  class T##N
#define __TYPE_ARG__(N)      T##N
#define __ARG__(N)           T##N t##N
#define __PARAM__(N)         t##N
#define __NOT_VIRTUAL__
#define __PLACEHOLDER__(N)   _##N


// type traits
#define __IS_CONST__(T)       (sizeof(__const_tester__(T)) == 1)
#define __IS_OBJECT__(T)      (sizeof(__object_tester__((T *)0)) == 1)

namespace reflection 
{

//helper signature for type traits
char __const_tester__(const void *p);
int __const_tester__(void *p);
char __object_tester__(Object *p);
int __object_tester__(void *p);

} // namespace reflection

// calculates the offset of a field
#define __OFFSET__           offsetof
//(C, M)    ((unsigned long)(&((const C *)0)->M))

// calculates the offset of an interface
#define __CLASS_OFFSET__(class, super) ((unsigned long) static_cast<super*>((class*)8) - 8)

// calculates the memory offset of a class member (which can be private as well)
#define __ASM_OFFSET__(ADDRESS, OBJECT_PTR, MEMBER) \
  __asm{ push eax } \
  __asm{ mov eax, OBJECT_PTR } \
  __asm{ add eax, [ MEMBER ] } \
  __asm{ mov ADDRESS, eax } \
  __asm{ pop eax }


/*****************************************************************************
    ENUMERATIONS
 *****************************************************************************/


namespace reflection 
{

/** access type enumeration
 */
enum ACCESS_TYPE 
{
    ///public access
    ACCESS_PUBLIC,

    ///protected access
    ACCESS_PROTECTED,

    ///private access
    ACCESS_PRIVATE
};

} // namespace reflection

#endif // _UTILITY_H_