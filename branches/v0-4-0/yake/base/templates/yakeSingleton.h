/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#ifndef YAKE_BASE_TEMPLATES_SINGLETON_H
#define YAKE_BASE_TEMPLATES_SINGLETON_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif
// Yake
#include <yake/base/templates/yakeThreads.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace templates
{

namespace detail
{

        class LifetimeTracker
        {
        public:
            LifetimeTracker(unsigned int x) : longevity_(x) 
            {}
            
            virtual ~LifetimeTracker() = 0;
            
            static bool Compare(const LifetimeTracker* lhs,
                const LifetimeTracker* rhs)
            {
                return lhs->longevity_ > rhs->longevity_;
            }
            
        private:
            unsigned int longevity_;
        };
        
        // Definition required
        inline LifetimeTracker::~LifetimeTracker() {} 
        
        // Helper data
        typedef LifetimeTracker** TrackerArray;
        extern TrackerArray pTrackerArray;
        extern unsigned int elements;

        // Helper destroyer function
        template <typename T>
        struct Deleter
        {
            static void Delete(T* pObj)
            { delete pObj; }
        };

        // Concrete lifetime tracker for objects of type T
        template <typename T, typename Destroyer>
        class ConcreteLifetimeTracker : public LifetimeTracker
        {
        public:
            ConcreteLifetimeTracker(T* p,unsigned int longevity, Destroyer d)
                : LifetimeTracker(longevity)
                , pTracked_(p)
                , destroyer_(d)
            {}
            
            ~ConcreteLifetimeTracker()
            { destroyer_(pTracked_); }
            
        private:
            T* pTracked_;
            Destroyer destroyer_;
        };

        void AtExitFn(); // declaration needed below
    
} // detail



template <typename T, typename Destroyer>
void SetLongevity(T* pDynObject, unsigned int longevity,Destroyer d = detail::Deleter<T>::Delete)
{
	using namespace detail;
        
	TrackerArray pNewArray = static_cast<TrackerArray>(
		std::realloc(pTrackerArray, 
			sizeof(*pTrackerArray) * (elements + 1)));
	if (!pNewArray) throw std::bad_alloc();
        
	// Delayed assignment for exception safety
	pTrackerArray = pNewArray;
        
	LifetimeTracker* p = new ConcreteLifetimeTracker<T, Destroyer>(
		pDynObject, longevity, d);
        
	// Insert a pointer to the object into the queue
	TrackerArray pos = std::upper_bound(
		pTrackerArray, 
		pTrackerArray + elements, 
		p, 
		LifetimeTracker::Compare);
	std::copy_backward(
		pos, 
		pTrackerArray + elements,
		pTrackerArray + elements + 1);
	*pos = p;
  ++elements;
        
  // Register a call to AtExitFn
  std::atexit(detail::AtExitFn);
}


template <class T> struct CreateUsingNew
{
	static T* Create()
	{ return new T; }
        
	static void Destroy(T* p)
	{ delete p; }
};
    

template <class T> struct CreateUsingMalloc
{
	static T* Create()
	{
		void* p = std::malloc(sizeof(T));
		if (!p) return 0;
		return new(p) T;
	}
        
	static void Destroy(T* p)
	{
		p->~T();
		std::free(p);
	}
};
    

template <class T> struct CreateStatic
{
#if defined(_MSC_VER) && _MSC_VER >= 1300
#pragma warning( push ) 
 // alignment of a member was sensitive to packing
#pragma warning( disable : 4121 )
#endif // _MSC_VER
	union MaxAlign
	{
		char t_[sizeof(T)];
		short int shortInt_;
		int int_;
		long int longInt_;
		float float_;
		double double_;
		long double longDouble_;
		struct Test;
		int Test::* pMember_;
		int (Test::*pMemberFn_)(int);
	};
#if defined(_MSC_VER) && _MSC_VER >= 1300
#pragma warning( pop )
#endif // _MSC_VER
        
static T* Create()
{
	static MaxAlign staticMemory_;
	return new(&staticMemory_) T;
}
        
static void Destroy(T* p)
{
	p->~T();
}
};
    
template <class T>
struct DefaultLifetime
{
	static void ScheduleDestruction(T*, void (*pFun)())
	{ std::atexit(pFun); }
        
	static void OnDeadReference()
	{ throw std::logic_error("Dead Reference Detected"); }
};

template <class T>
class PhoenixSingleton
{
public:
	static void ScheduleDestruction(T*, void (*pFun)())
	{
#ifndef ATEXIT_FIXED
		if (!destroyedOnce_)
#endif
		std::atexit(pFun);
	}
        
	static void OnDeadReference()
	{
#ifndef ATEXIT_FIXED
		destroyedOnce_ = true;
#endif
	}
        
private:
#ifndef ATEXIT_FIXED
	static bool destroyedOnce_;
#endif
};
    
#ifndef ATEXIT_FIXED
    template <class T> bool PhoenixSingleton<T>::destroyedOnce_ = false;
#endif
        

namespace detail
{
        template <class T>
        struct Adapter
        {
            void operator()(T*) { return pFun_(); }
            void (*pFun_)();
        };
} // detail



template <class T>
class SingletonWithLongevity
{
public:
	static void ScheduleDestruction(T* pObj, void (*pFun)())
	{
		detail::Adapter<T> adapter = { pFun };
		SetLongevity(pObj, GetLongevity(pObj), adapter);
	}
        
	static void OnDeadReference()
	{ throw std::logic_error("Dead Reference Detected"); }
};


template <class T>
struct NoDestroy
{
	static void ScheduleDestruction(T*, void (*)())
	{}
        
	static void OnDeadReference()
	{}
};

template
<
	typename T,
	template <class> class CreationPolicy = CreateUsingNew,
	template <class> class LifetimePolicy = DefaultLifetime,
	template <class> class ThreadingModel = SingleThreaded
>
class SingletonHolder
{
YAKE_DECLARE_CLASS(yake::base::templates::SingletonHolder)
public:
	static T& instance();
        
private:
	// Helpers
	static void MakeInstance();
	static void DestroySingleton();
        
	// Protection
	SingletonHolder();
        
	// Data
	typedef typename ThreadingModel<T*>::VolatileType PtrInstanceType;
	static PtrInstanceType pInstance_;
	static bool destroyed_;
};
    

template
<
	class T,
	template <class> class C,
	template <class> class L,
	template <class> class M
>
typename SingletonHolder<T, C, L, M>::PtrInstanceType SingletonHolder<T, C, L, M>::pInstance_;


template
<
	class T,
	template <class> class C,
	template <class> class L,
	template <class> class M
>
bool SingletonHolder<T, C, L, M>::destroyed_;


template
<
	class T,
	template <class> class CreationPolicy,
	template <class> class LifetimePolicy,
	template <class> class ThreadingModel
>
inline T& SingletonHolder<T, CreationPolicy, LifetimePolicy, ThreadingModel>::instance()
{
	if (!pInstance_)
	{
		MakeInstance();
	}
	return *pInstance_;
}


template
<
	class T,
	template <class> class CreationPolicy,
	template <class> class LifetimePolicy,
	template <class> class ThreadingModel
>
void SingletonHolder<T, CreationPolicy, LifetimePolicy, ThreadingModel>::MakeInstance()
{
	typename ThreadingModel<T>::Lock guard;
	(void)guard;
        
	if (!pInstance_)
	{
	if (destroyed_)
	{
		LifetimePolicy<T>::OnDeadReference();
		destroyed_ = false;
	}
	pInstance_ = CreationPolicy<T>::Create();
		LifetimePolicy<T>::ScheduleDestruction(pInstance_, 
			&DestroySingleton);
	}
}


template
<
	class T,
	template <class> class CreationPolicy,
	template <class> class L,
	template <class> class M
>
void SingletonHolder<T, CreationPolicy, L, M>::DestroySingleton()
{
	assert(!destroyed_);
	CreationPolicy<T>::Destroy(pInstance_);
	pInstance_ = 0;
	destroyed_ = true;
}

} // templates
} // base
} // yake

// Adds a singleton to the current class ClassName.
#define YAKE_BUILD_SINGLETON( ClassName ) \
	friend ClassName * yake::base::templates::CreateUsingNew<ClassName>::Create(); \
	public: \
		static ClassName& instance() \
		{ \
		return yake::base::templates::SingletonHolder< ClassName, \
			yake::base::templates::CreateUsingNew, \
				yake::base::templates::DefaultLifetime, \
					yake::SingleThreaded >::instance(); \
		}

#define YAKE_BUILD_PHOENIX_SINGLETON( ClassName ) \
	public: \
		static ClassName& instance() \
		{ \
			return yake::base::templates::SingletonHolder< ClassName, \
				yake::base::templates::CreateUsingNew, \
					yake::base::templates::PhoenixSingleton >::instance(); \
		}

#endif // YAKE_BASE_TEMPLATES_SINGLETON_H
