#include <yake/samples/net/roserver/pch.h>
#include <yake/net/net.h>
#include <yake/samples/net/common/common.h>
#include <yake/samples/net/common/commonEvents.h>
#include <yake/samples/net/roserver/ROServer.h>

namespace ro {
	//-----------------------------------------------------------------------------
	namespace server_impl {
		class client
		{
		public:
			enum stage_t
			{
				CS_WAIT_FOR_REQ = 1, // waiting for "req join"
				CS_JOINED, // server acknowledged "join" request & sent ...
				CS_RUNNING,
				CS_DEAD
			};
			enum access_mode_t {
				AM_ADMIN = 0,
				AM_MODERATOR,
				AM_REGULAR,
				AM_SPECTATOR
			};
			net::PeerId						id;
			stage_t							stage;
			access_mode_t					accessMode;
			net::INetEventConnection*	evtConn;

			client() : id(net::NetEvent::EVTID_NONE), stage(CS_DEAD), accessMode(AM_REGULAR), evtConn(0)
			{
				processEventFn_ = boost::bind(&client::init_onProcessEvent,this,_1,_2);
			}
			void onProcessEvent(const net::NetEvent& evt, const net::ChannelId channel)
			{
				YAKE_ASSERT( !processEventFn_.empty() );
				if (!processEventFn_.empty())
					processEventFn_(evt,channel);
			}
		private:
			void init_onProcessEvent(const net::NetEvent& evt, const net::ChannelId);
			void running_onProcessEvent(const net::NetEvent& evt, const net::ChannelId);
		private:
			typedef boost::function<void(const net::NetEvent&,const net::ChannelId)> ProcessNetEventFn;
			ProcessNetEventFn	processEventFn_;
		};
	} // namespace server_impl

	//-----------------------------------------------------------------------------

	server::server() : conn_(0), running_(false), evtConn_(0)
	{
	}
	server::~server()
	{
		stop();
	}
	bool server::start()
	{
		if (conn_)
			return true;

		const net::uint16 port = 40000;
		const size_t maxClients = 32;

		conn_ = net::createServerPacketConnection();
		YAKE_ASSERT( conn_ );

		conn_->addStartedCallback( boost::bind(&server::onServerStarted,this) );
		conn_->addPacketReceivedCallback( boost::bind(&server::onReceivePacket,this,_1,_2,_3,_4) );

		COUTLN("starting server at port " << port << " with max. " << maxClients << " clients...");
		conn_->start( net::Address(port), maxClients );

		while (!running())
		{
			net::update();
			net::native::sleep(10);
		}

		return true;
	}
	void server::stop()
	{
		{
			boost::mutex::scoped_lock lck(runningMtx_);
			running_ = false;
		}
		if (conn_)
		{
			conn_->stop();
			delete conn_;
			conn_ = 0;
		}
		if (evtConn_)
		{
			evtConn_->stop();
			delete evtConn_;
			evtConn_ = 0;
		}
	}
	void server::step()
	{
		if (!running())
			return;
		if (evtConn_)
			evtConn_->poll();
	}
	bool server::running() const
	{
		boost::mutex::scoped_lock lck(runningMtx_);
		return running_;
	}
	void server::onServerStarted()
	{
		COUTLN("server started.");
		{
			boost::mutex::scoped_lock lck(runningMtx_);
			running_ = true;
		}
		YAKE_ASSERT( !evtConn_ );
		evtConn_ = net::createEventConnection();
		YAKE_ASSERT( evtConn_ );

		evtConn_->setPolling(true);

		evtConn_->registerEvent( c2sEvtJoinReq::ID, net::NetEvent::DIR_ANY, c2sEvtJoinReq::create, c2sEvtJoinReq::destroy );
		evtConn_->registerEvent( s2cEvtJoinReqReply::ID, net::NetEvent::DIR_ANY, s2cEvtJoinReqReply::create, s2cEvtJoinReqReply::destroy );
		evtConn_->registerEvent( s2cEvtClassTable::ID, net::NetEvent::DIR_ANY, s2cEvtClassTable::create, s2cEvtClassTable::destroy );
		evtConn_->registerEvent( c2sEvtSimOk::ID, net::NetEvent::DIR_ANY, c2sEvtSimOk::create, c2sEvtSimOk::destroy );

		evtConn_->setProcessEventCallback( boost::bind(&server::onProcessEvent,this,_1,_2,_3) );
		evtConn_->setPacketConnection( conn_, net::NetEvent::DIR_ANY );

		evtConn_->start();
	}
	void server::onReceivePacket(const net::PeerId peerId, const void* data, const size_t len, const net::ChannelId channel)
	{
		COUTLN("server received packet.");
		//echoing...
		//conn_->send( peerId, data, len, net::SendOptions().channel(channel) );
	}
	void server::onProcessEvent(const net::PeerId peerId, const net::NetEvent& evt, const net::ChannelId channel)
	{
		try {
			COUTLN("server received event (" << (int)evt.id() << ").");

			// get client object
			client* c = 0;
			ClientPtrMap::iterator it = clients_.find( peerId );
			if (it == clients_.end())
			{
				c = new client();
				c->id = peerId;
				c->stage = client::CS_WAIT_FOR_REQ;
				c->evtConn = evtConn_;
				clients_.insert(std::make_pair(peerId,c));
			}
			else
				c = it->second;

			// handle event
			c->onProcessEvent( evt, channel );
		}
		catch (...)
		{
		}
	}
	//-----------------------------------------------------------------------------
	namespace server_impl {

		void client::init_onProcessEvent(const net::NetEvent& evt, const net::ChannelId channel)
		{
			YAKE_ASSERT( stage == CS_WAIT_FOR_REQ );
			if (stage != CS_WAIT_FOR_REQ)
				return;
			if (evt.id() == c2sEvtJoinReq::ID)
			{
				const c2sEvtJoinReq& evtJoinReq = static_cast<const c2sEvtJoinReq&>(evt);
				COUTLN("   evt: Join Request");

				// send reply: OK
				COUTLN("   => accepted");
				s2cEvtJoinReqReply joinReply;
				joinReply.accepted = true;
				evtConn->sendEvent( this->id, joinReply, net::SendOptions().channel(CHANNELID_CONTROL) );
#if NET_STRESS_TEST_LEVEL >= 1
				for (size_t i=0; i<3; ++i)
					evtConn->sendEvent( this->id, joinReply, net::SendOptions().channel(CHANNELID_CONTROL) );
#endif
				this->stage = CS_JOINED;

				// use different event callback for the CS_RUNNING stage:
				COUTLN("   => RUNNING");
				processEventFn_ = boost::bind(&client::running_onProcessEvent,this,_1,_2);
				this->stage = CS_RUNNING;
			}
		}
		void client::running_onProcessEvent(const net::NetEvent& evt, const net::ChannelId channel)
		{
			COUTLN("server::client::running_onProcessEvent()");
			YAKE_ASSERT( stage == CS_RUNNING );
			if (stage != CS_RUNNING)
				return;
			//@todo put events in a queue - it is then processed in the main simulation thread!
			//// send class table & start sim
			//s2cEvtClassTable evtClassTbl;
			//evtConn->sendEvent( this->id, evtClassTbl, net::SendOptions().channel(CHANNELID_SIMCTRL) );
		}
	} // namespace server_impl
} // namespace ro
