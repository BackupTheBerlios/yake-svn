#ifndef YAKE_THREAD_ZTHREADTHREADFORWARDER_H
#define YAKE_THREAD_ZTHREADMUTEXFORWARDER_H

#include <zthread/Thread.h>

namespace yake {
namespace thread {

	class YAKE_THREAD_API Thread : public IThread
	{
	private:
		struct RunnableAdapter : public ZThread::Runnable
		{
			SharedIRunnablePtr	m_r;
			RunnableAdapter(SharedIRunnablePtr pR) : m_r(pR)
			{}
			virtual void run() 
			{
				m_r->run();
			}
		};
		Thread();
		Thread(const Thread&);
		Thread(SharedIRunnablePtr pRunnable)
		{
			YAKE_ASSERT( pRunnable );
			m_t = new ZThread::Thread( new RunnableAdapter(pRunnable) );
			YAKE_ASSERT( m_t );
		}
	public:
		~Thread()
		{
			delete m_t;
		}
		static Thread* create(IRunnablePtr pRunnable) throw( ThreadException )
		{
			return create( SharedIRunnablePtr(pRunnable) );
		}
		static Thread* create(SharedIRunnablePtr pRunnable) throw( ThreadException )
		{
			try {
				if (!pRunnable.get())
					return 0;
				return new Thread(pRunnable);
			} catch(ZThread::Synchronization_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "yake.thread: sync exception!");
			}
		}
		virtual void yield() throw( ThreadException )
		{ 
			YAKE_ASSERT( m_t );
			if (!m_t)
				return;
			try {
				m_t->yield();
			} catch(ZThread::Synchronization_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "yake.thread: sync exception!");
			}
		}
		virtual void sleep(const uint32 ms) throw( ThreadException )
		{ 
			YAKE_ASSERT( m_t );
			if (!m_t)
				return;
			try {
				m_t->sleep( ms );
			} catch(ZThread::Synchronization_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "yake.thread: sync exception!");
			}
		}
		virtual void wait(const uint32 ms = 0) throw( ThreadException )
		{ 
			YAKE_ASSERT( m_t );
			if (!m_t)
				return;
			try {
				m_t->wait( ms );
			} catch(ZThread::Synchronization_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "yake.thread: sync exception!");
			}
		}
	private:
		ZThread::Thread*	m_t;
	};

} // namespace thread
} // namespace yake

#endif