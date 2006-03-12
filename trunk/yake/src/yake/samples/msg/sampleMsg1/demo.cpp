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
		assert( pool_.is_from(msg) );
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
	ObjectMessage(const std::string& name1, const A1& a1, const std::string& name2, const A2& a2)
	{
		params_[ name1 ] = a1;
		params_[ name2 ] = a2;
	}

	size_t size() const
	{ return params_.size(); }

private:
	typedef std::map<std::string,boost::any> params_map;
	params_map	params_;
};

int main(int argc, char* argv[])
{
	// Default/Immediate processing:
	{
		msg::router<msg::ImmediateProcessor> router;

		// The following message will be lost as there's no handler registered yet!
		router.post(msg::makeMessage(1234));

		// register a few handlers
		router << msg::makeListener<int>(&app_handle_int);
		router << msg::makeListener<MsgResize>(&app_handle_MsgResize1);
		router << msg::makeListener<MsgResize>(&app_handle_MsgResize2);

		// post a few message to different handlers (depending on type)
		router.post(msg::makeMessage(1234));
		router.post(msg::makeMessage(123.12));
		router.post(msg::makeMessage(MsgResize(20,30)));
	}
	//Q'd :
	{
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
}

