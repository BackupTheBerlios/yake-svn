/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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
#ifndef YAKE_THREAD_H
#define YAKE_THREAD_H

#include <yake/thread/yakeThreadPrerequisites.h>

#define YAKE_THREAD_WITH_ZTHREAD
#define YAKE_MUTEX_WITH_ZTHREAD

#if defined(YAKE_THREAD_WITH_ZTHREAD)
#	define YAKE_THREAD_TYPE ZThread
//#elif defined(YAKE_THREAD_WITH_OPENTHREADS)
#else
#	pragma message("yakeThread.h: No thread forwarder specified")
#endif

namespace yake {
namespace thread {

	class YAKE_THREAD_API ThreadException : public ::yake::Exception
	{
	};

}
}

#if defined(YAKE_MUTEX_WITH_ZTHREAD)
#	define YAKE_MUTEX_TYPE ZThreadMutexForwarder
#	include <yake/thread/ZThreadMutexForwarder.h>
#endif

namespace yake {
namespace thread {
	class YAKE_THREAD_API IRunnable
	{
	public:
		virtual ~IRunnable() {}
		virtual void run() = 0;
	};
	YAKE_THREAD_COMMON_POINTERS( IRunnable );


	using ::yake::SharedPtr;

	class YAKE_THREAD_API Task : SharedIRunnablePtr
	{
	public:
#if !defined(_MSC_VER) || (_MSC_VER > 1200)
			
		Task(IRunnable* raw)
			: SharedIRunnablePtr(raw) { } 

#endif
		template <typename U>
			Task(U* raw)
			: SharedIRunnablePtr(raw) { } 

		Task(const SharedIRunnablePtr& ptr) 
			: SharedIRunnablePtr(ptr) { } 
/*
		template <typename U, typename V>
			Task(const SharedPtr<U, V>& ptr) 
			: SharedIRunnablePtr(ptr) { } 
*/
		virtual ~Task() {}
		void operator()()
		{
			m_runnable->run();
		}
	private:
		SharedIRunnablePtr	m_runnable;
	};

	class YAKE_THREAD_API IThread
	{
	public:
		YAKE_DECLARE_REGISTRY_0(IThread, ::yake::String)
		virtual ~IThread() {}

		virtual void yield() throw( ThreadException ) = 0;
		virtual void sleep(const uint32 ms) throw( ThreadException ) = 0;
		virtual void wait(const uint32 ms = 0) throw( ThreadException ) = 0;
	};
	YAKE_THREAD_COMMON_POINTERS( IThread );


	class YAKE_THREAD_API ILockable
	{
	public:
		virtual ~ILockable() {}

		virtual void acquire() throw( ThreadException ) = 0;
		virtual bool tryAcquire(const uint32 timeout) throw( ThreadException ) = 0;
		virtual void release() throw( ThreadException ) = 0;
	};
	YAKE_THREAD_COMMON_POINTERS( ILockable );

	class YAKE_THREAD_API IMutex
	{
	public:
		virtual ~IMutex() {}
	};
	YAKE_THREAD_COMMON_POINTERS( IMutex );

	template<class Impl>
	class ForwardingMutexT : public IMutex
	{
	public:
		ForwardingMutexT() {}
		virtual ~ForwardingMutexT() {}

		virtual void acquire() throw( ThreadException )
		{ m_impl.acquire(); }
		virtual bool tryAcquire(const uint32 timeout) throw( ThreadException )
		{ return m_impl.tryAcquire(timeout); }
		virtual void release() throw( ThreadException )
		{ return m_impl.release(); }
	private:
		Impl	m_impl;
	};

#ifdef YAKE_MUTEX_TYPE
	typedef ForwardingMutexT<YAKE_MUTEX_TYPE> Mutex;
	YAKE_THREAD_COMMON_POINTERS( Mutex );
#else
#	pragma message("yakeThread.h: No standard mutex implementation available.")
#endif

	template<class Impl>
	class ForwardingThreadT : public IThread
	{
	public:
		ForwardingThreadT() : m_impl(0) {}
		ForwardingThreadT( Task& rTask ) { m_impl = new Impl( rTask ); }
		virtual ~ForwardingThreadT() { delete m_impl; }

		virtual void yield() throw( ThreadException )
		{ 
			YAKE_ASSERT( m_impl );
			m_impl->yield(); 
		}
		virtual void sleep(const uint32 ms) throw( ThreadException )
		{ 
			YAKE_ASSERT( m_impl );
			m_impl->sleep(ms);
		}
		virtual void wait(const uint32 ms = 0) throw( ThreadException )
		{ 
			YAKE_ASSERT( m_impl );
			m_impl->sleep(ms);
		}
	private:
		Impl*	m_impl;
	};

#if YAKE_THREAD_TYPE == ZThread
} // namespace thread
} // namespace yake
#	include <yake/thread/ZThreadThreadForwarder.h>
namespace yake {
namespace thread {
	YAKE_THREAD_COMMON_POINTERS( Thread );
#else
#	pragma message("yakeThread.h: No standard thread implementation available.")
#endif

} // namespace thread
} // namespace yake

#endif

