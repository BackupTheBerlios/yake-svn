#ifndef _STATIC_INIT_H_
#define _STATIC_INIT_H_

#define static_init(FUNCTION) \
namespace { \
struct static_init_##FUNCTION \
{ \
	static_init_##FUNCTION##() \
	{ \
		static int counter = 0; \
		if( counter++ > 0 ) return; \
		FUNCTION(); \
	} \
} g_static_init_##FUNCTION; }

#endif _STATIC_INIT_H_