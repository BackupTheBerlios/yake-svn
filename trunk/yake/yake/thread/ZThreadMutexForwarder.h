#ifndef YAKE_THREAD_ZTHREADMUTEXFORWARDER_H
#define YAKE_THREAD_ZTHREADMUTEXFORWARDER_H

#include <zthread/Mutex.h>

namespace yake {
namespace thread {

	/** @todo exception handling
	*/
	class ZThreadMutexForwarder
	{
	public:
		void acquire()
		{
			try {
				mtx_.acquire();
			} 
			catch(ZThread::Deadlock_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "zthread mutex forwarder: deadlock in acquire()");
			} 
			catch(ZThread::Interrupted_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "zthread mutex forwarder: interrupt in acquire()");
			}
		}
		bool tryAcquire(const uint32 timeout)
		{
			try {
				return mtx_.tryAcquire(timeout);
			}
			catch(ZThread::Deadlock_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "zthread mutex forwarder: deadlock in tryAcquire()");
			} 
			catch(ZThread::Interrupted_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "zthread mutex forwarder: interrupt in tryAcquire()");
			}
		}
		void release()
		{
			try {
				mtx_.release();
			} catch(ZThread::InvalidOp_Exception& e)
			{
				YAKE_THREAD_EXCEPT(e.what(), "zthread mutex forwarder: invalid op in release()");
			}
		}
	private:
		ZThread::Mutex	mtx_;
	};

} // namespace thread
} // namespace yake

#endif