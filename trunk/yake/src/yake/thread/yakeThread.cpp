#include <yake/thread/yakePCH.h>
#include <yake/thread/yakeThread.h>

namespace yake {
namespace thread {
	YAKE_IMPLEMENT_REGISTRY(IThread)

#ifdef YAKE_MUTEX_TYPE
	namespace __private_force_link {
		// Force linking to ZThread:
		struct __private_force_link_runnable : public IRunnable
		{
			virtual void run() {}
		};
		void force_link()
		{
			Mutex m;
			ThreadPtr t = Thread::create( new __private_force_link_runnable() );
			t->wait();
			delete t;
		}
	}
#endif

}
}