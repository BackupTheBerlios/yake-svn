#ifndef _STATIC_INITOR_H_
#define _STATIC_INITOR_H_

#define STATIC_INITOR_CL1( class_name, function_name )\
namespace\
{\
	static struct class_name##_initor\
	{\
	class_name##_initor()\
		{\
			static int counter = 0;\
			if( counter++ > 0 ) return;\
			class_name::function_name();\
		}\
	} g_##class_name##_initor;\
} // nameless

#define STATIC_INITOR_FNC1( function_name )\
namespace\
{\
	static struct function_name##_initor\
	{\
	function_name##_initor()\
		{\
			static int counter = 0;\
			if( counter++ > 0 ) return;\
			function_name();\
		}\
	} g_##function_name##_initor;\
} // nameless

#endif // _STATIC_INITOR_H_