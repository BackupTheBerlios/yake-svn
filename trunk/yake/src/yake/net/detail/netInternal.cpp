#include <yake/net/pch.h>
#include <yake/net/net.h>
#include <yake/net/detail/netInternal.h>

namespace net {
namespace impl {

	//--------------------------------------------------------------------------
	UpdateThread* UpdateThread::instance_ = 0;
	bool UpdateThread::create()
	{
		if (!instance_)
			instance_ = new UpdateThread();
		return (instance_ != 0);
	}
	void UpdateThread::destroy()
	{
		if (instance_)
		{
			instance_->requestQuit();
			while (!instance_->dead())
				native::sleep(10);
			delete instance_;
			instance_ = 0;
		}
	}
	UpdateThread& UpdateThread::instance()
	{
		NET_ASSERT( instance_ );
		return *instance_;
	}
	UpdateThread::UpdateThread() : nextId_(1), thread_(0), quitRequested_(false), dead_(false)
	{
		thread_ = new boost::thread( boost::bind(&UpdateThread::callFns,this) );
	}
	UpdateThread::~UpdateThread()
	{
		if (thread_)
		{
			thread_->join();
			delete thread_;
			thread_ = 0;
		}
		fns_.clear();
	}
	void UpdateThread::requestQuit()
	{
		boost::mutex::scoped_lock lock(mtx_);
		quitRequested_ = true;
	}
	bool UpdateThread::dead() const
	{
		boost::mutex::scoped_lock lock(deadMtx_);
		return dead_;
	}
	void UpdateThread::callFns()
	{
		while (true)
		{
			{
				boost::mutex::scoped_lock lock(mtx_);

				if (quitRequested_)
				{
					boost::mutex::scoped_lock lock(deadMtx_);
					fns_.clear();
					dead_ = true;
					return;
				}

				for (UpdateFnMap::iterator it = fns_.begin(); it != fns_.end(); ++it)
				{
					it->second();
				}
			}
			native::sleep(10);
		}
	}
	uint32 UpdateThread::add( const UpdateFn& fn )
	{
		boost::mutex::scoped_lock lock(mtx_);
		std::cout << "UpdateThread::add()\n";
		fns_[ nextId_++ ] = fn;
		return nextId_-1;
	}
	void UpdateThread::remove( const uint32 id )
	{
		boost::mutex::scoped_lock lock(mtx_);
		std::cout << "UpdateThread::remove()\n";
		UpdateFnMap::iterator it = fns_.find( id );
		if (it != fns_.end())
			fns_.erase( it );
	}
	//void UpdateThread::remove( const UpdateFn& fn )
	//{
	//	boost::mutex::scoped_lock lock(mtx_);
	//	for (UpdateFnMap::iterator it = fns_.begin(); it != fns_.end(); ++it)
	//	{
	//		if (it->second == fn)
	//		{
	//			fns_.erase( it );
	//			return;
	//		}
	//	}
	//}

	boost::mutex& getEnetMtx()
	{
		static boost::mutex mtx;
		return mtx;
	}

} // namespace impl
} // namespace net
