#ifndef RO_CLIENT_H
#define RO_CLIENT_H

namespace ro {

	struct client
	{
	public:
		client();
		~client();
		bool start( const net::Address& );
		void waitForStart();
		void stop();
		bool running() const;
		void step();
	private:
		net::IClientPacketConnection*	conn_;
		safe_var<bool>					timedOut_;
		net::INetEventConnection*	evtConn_;

		bool								running_;
		mutable boost::mutex			runningMtx_;

		enum stage_t {
			CS_DEAD = 1,
			CS_JOINING,
			CS_RUNNING
		};
		stage_t							stage_;
	private:
		void onClientStarted();
		void onTimeOut();
		void running_onProcessEvent(const net::PeerId, const net::NetEvent& , const net::ChannelId);
		void init_onProcessEvent(const net::PeerId, const net::NetEvent& , const net::ChannelId);
	};

} // namespace ro

#endif
#ifndef RO_CLIENT_H
#define RO_CLIENT_H

namespace ro {

	struct client
	{
	public:
		client();
		~client();
		bool start( const net::Address& );
		void waitForStart();
		void stop();
		bool running() const;
		void step();
	private:
		net::IClientPacketConnection*	conn_;
		safe_var<bool>					timedOut_;
		net::INetEventConnection*	evtConn_;

		bool								running_;
		mutable boost::mutex			runningMtx_;

		enum stage_t {
			CS_DEAD = 1,
			CS_JOINING,
			CS_RUNNING
		};
		stage_t							stage_;
	private:
		void onClientStarted();
		void onTimeOut();
		void running_onProcessEvent(const net::PeerId, const net::NetEvent& , const net::ChannelId);
		void init_onProcessEvent(const net::PeerId, const net::NetEvent& , const net::ChannelId);
	};

} // namespace ro

#endif
