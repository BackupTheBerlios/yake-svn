#ifndef RO_SERVER_H
#define RO_SERVER_H

namespace ro {
	namespace server_impl {
		class client;
	}
	struct server
	{
	public:
		server();
		~server();
		bool start();
		void stop();
		void step();

		void queueEvent(const net::PeerId, net::NetEvent*, const net::ChannelId);
		void queueEventBroadcast(net::NetEvent*, const net::ChannelId);
	private:
		void onServerStarted();
		void onReceivePacket(const net::PeerId, const void*, const size_t, const net::ChannelId);
		void onProcessEvent(const net::PeerId, const net::NetEvent&, const net::ChannelId);
	private:
		bool									running_;
		mutable boost::mutex				runningMtx_;

		bool running() const;

		//
		net::IServerPacketConnection*	conn_;
		net::INetEventConnection*		evtConn_;

		typedef server_impl::client client;

		typedef std::map<net::PeerId,client*> ClientPtrMap;
		ClientPtrMap	clients_;

		//@todo put into specialized object "NetEventQueue"
		struct EventQEntry
		{
			net::NetEvent*	evt_;
			net::PeerId		peerId_;
			net::ChannelId	channelId_;
			EventQEntry(const net::PeerId, const net::NetEvent*, const net::ChannelId);
		};
		typedef std::deque<EventQEntry> EventQ;
		EventQ								outEvtQ_;
		mutable boost::mutex				outEvtQMtx_;
	};

} // namespace ro

#endif
#ifndef RO_SERVER_H
#define RO_SERVER_H

namespace ro {
	namespace server_impl {
		class client;
	}
	struct server
	{
	public:
		server();
		~server();
		bool start();
		void stop();
		void step();

		void queueEvent(const net::PeerId, net::NetEvent*, const net::ChannelId);
		void queueEventBroadcast(net::NetEvent*, const net::ChannelId);
	private:
		void onServerStarted();
		void onReceivePacket(const net::PeerId, const void*, const size_t, const net::ChannelId);
		void onProcessEvent(const net::PeerId, const net::NetEvent&, const net::ChannelId);
	private:
		bool									running_;
		mutable boost::mutex				runningMtx_;

		bool running() const;

		//
		net::IServerPacketConnection*	conn_;
		net::INetEventConnection*		evtConn_;

		typedef server_impl::client client;

		typedef std::map<net::PeerId,client*> ClientPtrMap;
		ClientPtrMap	clients_;

		//@todo put into specialized object "NetEventQueue"
		struct EventQEntry
		{
			net::NetEvent*	evt_;
			net::PeerId		peerId_;
			net::ChannelId	channelId_;
			EventQEntry(const net::PeerId, const net::NetEvent*, const net::ChannelId);
		};
		typedef std::deque<EventQEntry> EventQ;
		EventQ								outEvtQ_;
		mutable boost::mutex				outEvtQMtx_;
	};

} // namespace ro

#endif
