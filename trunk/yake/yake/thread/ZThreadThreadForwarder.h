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
		static Thread* create(IRunnablePtr pRunnable)
		{
			return create( SharedIRunnablePtr(pRunnable) );
		}
		static Thread* create(SharedIRunnablePtr pRunnable)
		{
			if (!pRunnable.get())
				return 0;
			return new Thread(pRunnable);
		}
		virtual void yield()
		{ 
			YAKE_ASSERT( m_t );
			if (!m_t)
				return;
			m_t->yield();
		}
		virtual void sleep(const uint32 ms)
		{ 
			YAKE_ASSERT( m_t );
			if (!m_t)
				return;
			m_t->sleep( ms );
		}
		virtual void wait(const uint32 ms = 0)
		{ 
			YAKE_ASSERT( m_t );
			if (!m_t)
				return;
			m_t->wait( ms );
		}
	private:
		ZThread::Thread*	m_t;
	};

} // namespace thread
} // namespace yake

#endif