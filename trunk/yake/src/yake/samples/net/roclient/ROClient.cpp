#include <yake/samples/net/roclient/pch.h>
#include <yake/net/net.h>
#include <yake/samples/net/common/common.h>
#include <yake/samples/net/common/commonEvents.h>
#include <yake/samples/net/roclient/ROClient.h>

namespace ro {

client::client() : timedOut_(false), conn_(false), running_(false), evtConn_(0), stage_(CS_JOINING)
{
}
client::~client()
{
	stop();
}
bool client::running() const
{
	boost::mutex::scoped_lock lck(runningMtx_);
	return running_;
}
bool client::start(const net::Address& serverAddr)
{
	try {
		// create connection & register callbacks
		conn_ = net::createClientPacketConnection();
		assert( conn_ );

		conn_->addTimeOutCallback( boost::bind(&client::onTimeOut,this) );
		conn_->addStartedCallback( boost::bind(&client::onClientStarted,this) );

		// attempt to connect to server
		conn_->connect( serverAddr );

		return true;
	}
	catch (const net::Exception& e)
	{
		COUTLN("Caught net exception: " << e.what());
		if (conn_)
		{
			delete conn_;
			conn_ = 0;
		}
	}
	return false;
}
void client::waitForStart()
{
	while (!timedOut_.getCopy() && !running())
	{
		net::native::sleep(10);
		net::update();
	}
	if (timedOut_.getCopy())
		return;
	// set up event connection
	assert( conn_ );
	assert( !evtConn_ );
	evtConn_ = net::createEventConnection();
	assert( evtConn_ );

	evtConn_->setPolling( true );

	evtConn_->registerEvent( c2sEvtJoinReq::ID, net::NetEvent::DIR_ANY,
		c2sEvtJoinReq::create, c2sEvtJoinReq::destroy );
	evtConn_->registerEvent( s2cEvtJoinReqReply::ID, net::NetEvent::DIR_ANY,
		s2cEvtJoinReqReply::create, s2cEvtJoinReqReply::destroy );
	evtConn_->registerEvent( s2cEvtClassTable::ID, net::NetEvent::DIR_ANY,
			s2cEvtClassTable::create, s2cEvtClassTable::destroy );

	evtConn_->setProcessEventCallback( boost::bind(&client::init_onProcessEvent,this,_1,_2,_3) );
	evtConn_->setPacketConnection( conn_, net::NetEvent::DIR_ANY );

	evtConn_->start();

	// initiate "join" sequence
	this->stage_ = CS_JOINING;

	c2sEvtJoinReq evt;
	evtConn_->sendEvent( evt, net::SendOptions().channel(CHANNELID_CONTROL) );

#if NET_STRESS_TEST_LEVEL >= 1
	// send a second time to test our server logic :)
	for (size_t i=0; i<3; ++i)
		evtConn_->sendEvent( evt, net::SendOptions().channel(CHANNELID_CONTROL) );
#endif

	evt.setId( 23 ); // invalid id!
	evtConn_->sendEvent( evt, net::SendOptions().channel(CHANNELID_CONTROL) );

	while (stage_ == CS_JOINING)
	{
		evtConn_->poll();
		net::update();
		net::native::sleep(1);
	}
}
void client::step()
{
	if (!running())
		return;

	evtConn_->poll();

	if (stage_ == CS_RUNNING)
	{
	}
}
void client::stop()
{
	{
		boost::mutex::scoped_lock lck(runningMtx_);
		running_ = false;
	}
	// clean up
	if (evtConn_)
	{
		evtConn_->stop();
		delete evtConn_;
		evtConn_ = 0;
	}
	if (conn_)
	{
		conn_->disconnect();
		delete conn_;
		conn_ = 0;
	}
}
void client::onClientStarted()
{
	COUTLN("client started.");

	{
		boost::mutex::scoped_lock lck(runningMtx_);
		running_ = true;
	}
}
void client::onTimeOut()
{
	COUTLN("client could not connect to server. destroying connection object.");

	// Do NOT destroy the connection object here! It may still be inside the callback loop!
	// We can call disconnect(), though.
	timedOut_ = true;
	//conn->disconnect();
}
void client::running_onProcessEvent(const net::PeerId, const net::NetEvent& evt, const net::ChannelId)
{
	COUTLN("client (running) received event (" << (int)evt.id() << ").");
	if (evt.id() == s2cEvtClassTable::ID)
	{
		COUTLN("starting sim " << evt.id());
	}
}
void client::init_onProcessEvent(const net::PeerId, const net::NetEvent& evt, const net::ChannelId)
{
	COUTLN("client (init) received event (" << (int)evt.id() << ").");
	if (stage_ == CS_JOINING)
	{
		if (evt.id() == s2cEvtJoinReqReply::ID)
		{
			const s2cEvtJoinReqReply& reply = static_cast<const s2cEvtJoinReqReply&>( evt );
			if (reply.accepted)
			{
				COUTLN("CONNECTED TO SERVER.");
				stage_ = CS_RUNNING;
			}
			else
			{
				COUTLN("SERVER DENIED ACCESS!");
				stage_ = CS_DEAD;
				evtConn_->setProcessEventCallback( boost::bind(&client::init_onProcessEvent,this,_1,_2,_3) );
			}
		}
	}
}


} // namespace ro

