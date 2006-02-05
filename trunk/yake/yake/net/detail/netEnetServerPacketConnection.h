#ifndef NET_ENETSERVERPACKETCONNECTION_H
#define NET_ENETSERVERPACKETCONNECTION_H

namespace net {
namespace impl {

	//--------------------------------------------------------------------------
	class EnetServerPacketConnection : public ::net::IServerPacketConnection
	{
	public:
		EnetServerPacketConnection();
		virtual ~EnetServerPacketConnection();

		virtual void start( const Address&, const size_t );
		virtual void stop();

		virtual void setAllowedClientIps(const std::vector<std::string>&);

		virtual void disconnect( const PeerId client );

		virtual void send(const void*, const size_t, const SendOptions& opt = SendOptions());
		virtual void send(const PeerId, const void*, const size_t, const SendOptions& opt = SendOptions());

		virtual void addStartedCallback(const OnStartedFn&);
		virtual void addClientConnectedCallback(const OnClientConnectedFn&);
		virtual void addClientDisconnectedCallback(const OnClientDisconnectedFn&);
		virtual CallbackConnection addPacketReceivedCallback( const OnPacketReceivedFn&);

		void disconnectPacketReceivedCallback(const CallbackHandle h)
		{
			boost::mutex::scoped_lock lck(packetReceivedFnListMtx_);
			OnPacketReceivedFnList::iterator it = packetReceivedFnList_.find( h );
			if (it != packetReceivedFnList_.end())
				packetReceivedFnList_.erase( it );
		}
	private:
		void sendTo(const PeerId clientId, const void* dataPtr, const size_t dataSize, const Reliability rel, const Ordering, const ChannelId channel);
		void sendBroadcast(const void* dataPtr, const size_t dataSize, const Reliability rel, const Ordering, const ChannelId channel);
	private:
		void update();
		bool isIpAllowed( const std::string& );
		std::string ipToStringCached( const uint32 );

		void fireCallback_Started()
		{
			for (OnStartedFnList::const_iterator it = startedFnList_.begin(); it != startedFnList_.end(); ++it)
				(*it)();
		}
		void fireCallback_ClientConnected(const PeerId peerId, const Address& addr)
		{
			for (OnClientConnectedFnList::const_iterator it = clientConnectedFnList_.begin(); it != clientConnectedFnList_.end(); ++it)
				(*it)(peerId, addr);
		}
		void fireCallback_ClientDisconnected(const PeerId peerId)
		{
			for (OnClientDisconnectedFnList::const_iterator it = clientDisconnectedFnList_.begin(); it != clientDisconnectedFnList_.end(); ++it)
				(*it)(peerId);
		}
		void fireCallback_PacketReceived(const PeerId peerId, const void* data, const size_t dataLen, const ChannelId channel)
		{
			boost::mutex::scoped_lock lck(packetReceivedFnListMtx_);
			for (OnPacketReceivedFnList::const_iterator it = packetReceivedFnList_.begin(); it != packetReceivedFnList_.end(); ++it)
				(it->second)(peerId,data,dataLen,channel);
		}
	private:
		typedef std::deque<OnStartedFn> OnStartedFnList;
		typedef std::deque<OnClientConnectedFn> OnClientConnectedFnList;
		typedef std::deque<OnClientDisconnectedFn> OnClientDisconnectedFnList;
		typedef std::map<CallbackHandle,OnPacketReceivedFn> OnPacketReceivedFnList;
		OnStartedFnList					startedFnList_;
		OnClientConnectedFnList			clientConnectedFnList_;
		OnClientDisconnectedFnList		clientDisconnectedFnList_;
		OnPacketReceivedFnList			packetReceivedFnList_;
		CallbackHandle						lastPacketReceivedCbHandle_;
		boost::mutex						packetReceivedFnListMtx_;

	private:
		enum State {
			S_DEAD,
			S_STARTING,
			S_RUNNING,
			S_STOPPING
		};
		struct state_t {
			State get() const
			{
				boost::mutex::scoped_lock lock(mtx_);
				return state_;
			}
			void set(const State state)
			{
				boost::mutex::scoped_lock lock(mtx_);
				state_ = state;
			}
		private:
			State					state_;
			mutable boost::mutex mtx_;
		} m_state;

		ENetAddress				m_address;
		ENetHost*				m_host;
		uint32					m_id;

		// Could use std::vector<uint32> if not for IPv6 and the std::string interface of IPacketServer.
		typedef std::vector<std::string> HostList;
		HostList				m_ipWhiteList;
		HostList				m_ipBlackList;
		mutable boost::mutex	m_ipListMtx;

		typedef std::map<uint32,std::string> IpToString;
		IpToString				m_ip2string;

		enum ClientState
		{
			CS_CONNECTED,
			CS_DISCONNECTING,
			CS_DEAD
		};
		struct Client
		{
			ENetPeer*	peer;
			ClientState	state;
			PeerId		id;
			uint32		host;
			//Address		addr;
		};
		typedef std::map<ENetPeer*,Client*> PeerToClientMap;
		PeerToClientMap			m_clients; // access only via update()
		typedef std::map<PeerId,Client*> IdToClientMap;
		IdToClientMap			m_id2client;
		PeerId					m_nextPeerId;
		mutable boost::mutex m_clientsMtx;
	private:
		ClientState getClientState(ENetPeer* peer) const; // access only via update()
	};

} // namespace impl
} // namespace net

#endif
