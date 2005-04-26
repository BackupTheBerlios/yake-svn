#ifndef YAKE_THREAD_PREREQUISITES_H
#define YAKE_THREAD_PREREQUISITES_H

#ifdef YAKE_THREAD_EXPORTS
#	define YAKE_THREAD_API DLLEXPORT
#else
#	define YAKE_THREAD_API DLLIMPORT
#endif

#define YAKE_THREAD_COMMON_POINTERS( CLASS ) \
	typedef CLASS* ##CLASS##Ptr; \
	typedef ::yake::WeakPtr<CLASS> Weak##CLASS##Ptr; \
	typedef ::yake::SharedPtr<CLASS> Shared##CLASS##Ptr;

#define YAKE_THREAD_EXCEPT(m,s) throw( yake::Exception(m,s,__FILE__,__LINE__) )

#endif