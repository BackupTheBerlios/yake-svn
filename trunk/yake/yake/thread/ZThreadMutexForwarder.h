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
			mtx_.acquire();
		}
		bool tryAcquire(const uint32 timeout)
		{
			return mtx_.tryAcquire(timeout);
		}
		void release()
		{
			return mtx_.release();
		}
	private:
		ZThread::Mutex	mtx_;
	};

} // namespace thread
} // namespace yake

#endif