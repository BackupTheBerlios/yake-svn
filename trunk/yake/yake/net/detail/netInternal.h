#ifndef NET_INTERNAL_H
#define NET_INTERNAL_H

#pragma warning(push)
#pragma warning(disable: 4251) // C4251:  'T' needs to have dll-interface to be used by clients of class 'X'
#pragma warning(disable: 4275) // C4275:  'T' used as base for dll-interface class 'X'

#include <cassert>
#include <sstream>
#include <iostream>
#include <map>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

#define NET_ERROR( X ) \
	{ \
		std::stringstream s; \
		s << "net_error: " << X << "\n"; \
		std::cerr << s.str().c_str(); \
	}

#define NET_LOG( X ) \
	{ \
		std::stringstream s; \
		s << "net: " << X << "\n"; \
		std::cout << s.str().c_str(); \
	}

#define NET_THROW(MSG) \
	throw Exception(MSG,__FILE__,__LINE__)

#define NET_EXCEPT NET_THROW

namespace net {
namespace impl {

	boost::mutex& getEnetMtx();

	typedef boost::function<void(void)> UpdateFn;
	class UpdateThread
	{
	private:
		UpdateThread();
		~UpdateThread();
	public:
		uint32 add( const UpdateFn& );
		//void remove( const UpdateFn& );
		void remove( const uint32 );
		bool dead() const;
		void requestQuit();

		static bool create();
		static void destroy();
		static UpdateThread& instance();

		void __update__() // must only be called from net::update()
		{ this->callFns(); }
	private:
		void threadMain();
		void callFns();

		typedef std::map<uint32,UpdateFn> UpdateFnMap;
		UpdateFnMap		fns_;
		uint32			nextId_;
		boost::thread*	thread_;
		mutable boost::mutex	mtx_;
		bool			quitRequested_;

		bool			dead_;
		mutable boost::mutex	deadMtx_;

		static UpdateThread* instance_;
	};

	/** @todo move this into general purpose lib/header
	*/
	template<typename ctr_type>
		bool contains(const ctr_type& ctr, typename const ctr_type::value_type& value)
	{
		return (ctr.end() != std::find(ctr.begin(), ctr.end(), value));
	}
	template<typename kt, typename vt, typename at>
		bool contains(const std::map<kt,vt,at>& ctr, typename const std::map<kt,vt,at>::key_type& key)
	{
		return (ctr.end() != ctr.find(key));
	}
	template<typename map_type>
		bool map_contains(const map_type& ctr, typename const map_type::key_type& key)
	{
		return (ctr.end() != ctr.find(key));
	}

} // namespace impl
} // namespace net

#pragma warning(pop)

#endif
