//#include "yake/base/yakePrerequisites.h"
#include "yake/base/yake.h"
#include "yake/msg/yakeMessage.h"

using namespace yake;

void app_handle_int(const int& i)
{
	std::cout << "app_handle_int(" << i << ")\n";
}

struct MsgResize
{
	int width;
	int height;
	MsgResize(int w = 0, int h = 0) : width(w), height(h) {}
};
void app_handle_MsgResize1(const MsgResize& msg)
{
	std::cout << "app_handle_MsgResize1(width=" << msg.width << ", height=" << msg.height << ")\n";
}
void app_handle_MsgResize2(const MsgResize& msg)
{
	std::cout << "app_handle_MsgResize2(width=" << msg.width << ", height=" << msg.height << ")\n";
}
struct MsgQ2
{
	MsgQ2(msg::router<msg::DoubleQueuedProcessor>& router) : router_(router)
	{}
	msg::router<msg::DoubleQueuedProcessor>& router_;
};
void app_handle_q2(const MsgQ2& msg)
{
	std::cout << "app_handle_q2()\n";
	msg.router_.post(msg::makeMessage(444));
}

/*
struct MsgResizeAllocator
{
	static msg::message<MsgResize>* create(const MsgResize& v)
	{
		msg::message<MsgResize>* msg = pool_.construct();
		msg->setValue(v);
		return msg;
	}
	static void destroy(msg::message<MsgResize>* msg)
	{
		YAKE_ASSERT( pool_.is_from(msg) );
		pool_.destroy( msg );
	}
private:
	typedef boost::object_pool<msg::message<MsgResize> > pool_type;
	static pool_type	pool_;
};
MsgResizeAllocator::pool_type MsgResizeAllocator::pool_;
namespace msg {
	template<>
	message<MsgResize>* makeMessage(const MsgResize& v)
	{ 
		std::cout << "makeMessage<MsgResize>()\n";
		return MsgResizeAllocator::create(v); 
	}
	template<>
	void destroyMessage(message<MsgResize>* msg)
	{ 
		std::cout << "destroyMessage<MsgResize>()\n";
		return MsgResizeAllocator::destroy(msg); 
	}
} // namespace msg
*/

struct ObjectMessage
{
	ObjectMessage() {}

	template<typename A1>
	ObjectMessage(const std::string& name1, const A1& a1)
	{
		params_[ name1 ] = a1;
	}

	template<typename A1, typename A2>
	ObjectMessage(	const std::string& name1, const A1& a1,
					const std::string& name2, const A2& a2)
	{
		params_[ name1 ] = a1;
		params_[ name2 ] = a2;
	}

	template<typename A1, typename A2, typename A3>
	ObjectMessage(	const std::string& name1, const A1& a1,
					const std::string& name2, const A2& a2,
					const std::string& name3, const A3& a3)
	{
		params_[ name1 ] = a1;
		params_[ name2 ] = a2;
		params_[ name3 ] = a3;
	}

	size_t size() const
	{ return params_.size(); }

private:
	typedef std::map<std::string,boost::any> params_map;
	params_map	params_;
};

int main(int argc, char* argv[])
{
	// Immediate processing:
	{
		YAKE_LOG_INFORMATION("demo: immediate processing");
		msg::router<msg::ImmediateProcessor> router;

		// The following message will be lost as there's no handler registered yet!
		router.post(msg::makeMessage(1234));

		// register a few handlers
		router << msg::makeListener<int>(&app_handle_int);
		router << msg::makeListener<MsgResize>(&app_handle_MsgResize1);
		router << msg::makeListener<MsgResize>(&app_handle_MsgResize2);

		// post a few message to different handlers (depending on type)

		// The following 2 calls result in the same. makeMessage() can be useful for creating and queuing
		// message prior to sending them.
		router.postMessage(1234);
		router.post(msg::makeMessage(1234));

		router.post(msg::makeMessage(123.12));
		router.post(msg::makeMessage(MsgResize(20,30)));
	}
	//Q'd :
	{
		YAKE_LOG_INFORMATION("demo: 1-queued processing");
		msg::router<msg::SingleQueuedProcessor> router;

		// The following message will *not* be lost even though there's no handler registered yet!
		router.post(msg::makeMessage(1234));

		router << msg::makeListener<int>(&app_handle_int);

		// The following call will trigger processing of the messages.
		// NB It is not possible to post message while messages are processed! Use a DoubleQueuedProcessor router for that!
		router.processMessages();
	}

	//Double-Q'd :
	{
		YAKE_LOG_INFORMATION("demo: 2-queued processing");
		msg::router<msg::DoubleQueuedProcessor> router;

		// add a few handlers/listeners
		router << msg::makeListener<int>(&app_handle_int);
		router << msg::makeListener<MsgQ2>(&app_handle_q2);

		// Post a message - the message handler will post a message to the same router!
		// router<DoubleQueuedProcessor> supports this behaviour!
		router.post(msg::makeMessage(MsgQ2(router)));
		router.processMessages(); // handles MsgQ2 message
		router.processMessages(); // handles int message that the MsgQ2 handler posted

		// @todo
		router.postMessage(ObjectMessage("entity",0x00a0a0a0,"name",std::string("freak")));
	}
	// Immediate processing using source identifiers
	{
		YAKE_LOG_INFORMATION("demo: immediate processing with subscription to source identifiers");
		msg::router<msg::ImmediateProcessor> router;

		// register a few handlers
		router << std::make_pair(msg::makeListener<MsgResize>(&app_handle_MsgResize1),msg::Source(12));

		// post a few message to different handlers (depending on type)
		router.post(msg::makeMessage(MsgResize(44,44)));
		router.post(msg::makeMessage(MsgResize(44,44),msg::kNoSource));
		router.post(msg::makeMessage(MsgResize(55,55),msg::Source(11)));
		router.post(msg::makeMessage(MsgResize(66,66),msg::Source(12)));
	}
	// EXPERIMENTAL:
	// Immediate processing using source and target identifiers
	{
		YAKE_LOG_INFORMATION("demo: immediate processing with subscription to source and target identifiers");
		msg::router<msg::ImmediateProcessor> router;

		// register handler(s)
		msg::ListenerConnection conn1 = router.add( msg::makeListener<MsgResize>(&app_handle_MsgResize1), msg::Source(12), msg::Target(1) );
		//router.add( msg::makeListener<MsgResize>(&app_handle_MsgResize2), msg::Source(13), msg::kAnyTarget );
		msg::ListenerConnection conn2 = router.add( msg::makeListener<MsgResize>(&app_handle_MsgResize2), msg::kAnySource, msg::kAnyTarget );

		// post a few message to different handlers (depending on type)
		router.post(msg::makeMessage(MsgResize(22,22)));
		router.post(msg::makeMessage(MsgResize(33,33),msg::kNoSource));
		router.post(msg::makeMessage(MsgResize(55,55),msg::Source(12),msg::Target(2)));
		router.post(msg::makeMessage(MsgResize(44,44),msg::Source(12),msg::kBroadcastTarget)); // should be handled
		router.post(msg::makeMessage(MsgResize(66,66),msg::Source(12),msg::Target(1))); // should be handled

		// the following messages are all process by the catch-call handler registered above with kAnyTarget.
		YAKE_LOG_INFORMATION("  removing all listeners except catch-all listener");
		conn1.disconnect();
		router.post(msg::makeMessage(MsgResize(11,11),msg::Source(13),msg::Target(2)));
		router.post(msg::makeMessage(MsgResize(12,12),msg::Source(13),msg::kBroadcastTarget));
		router.post(msg::makeMessage(MsgResize(13,13),msg::Source(13),msg::Target(1)));

		// testing scoped listener
		YAKE_LOG_INFORMATION("  removing all listeners and demo'ing a scoped listener");
		conn2.disconnect();
		{
			msg::ScopedListenerConnection conn3 = router.add( msg::makeListener<MsgResize>(&app_handle_MsgResize2), msg::kAnySource, msg::kAnyTarget );
			router.post(msg::makeMessage(MsgResize(1,1),msg::Source(13),msg::Target(2))); // will be handled
		}
		router.post(msg::makeMessage(MsgResize(2,2),msg::Source(13),msg::Target(2))); // will *not* be handled
	}
	// Processing polymorphic message system
	{
		YAKE_LOG_INFORMATION("demo: polymorphism");
	}
}

