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

		virtual size_t getNumConnectedClients() const
		{
			PeerToClientMap::const_iterator itEnd = m_clients.end();
			size_t count = 0;
			for (PeerToClientMap::const_iterator it = m_clients.begin(); it != itEnd; ++it)
			{
				if (it->second->state == CS_CONNECTED)
					++count;
			}
			return count;
		}

		virtual void disconnect( const PeerId client );

		virtual void send(const void*, const size_t, const SendOptions& opt = SendOptions());
		virtual void send(const PeerId, const void*, const size_t, const SendOptions& opt = SendOptions());

		virtual void addStartedCallback(const OnStartedFn&);
		virtual void addClientConnectedCallback(const OnClientConnectedFn&);
		virtual void addClientDisconnectedCallback(const OnClientDisconnectedFn&);
		virtual CallbackConnection addPacketReceivedCallback( const OnPacketReceivedFn&);

		void disconnectPacketReceivedCallback(const CallbackHandle h)
		{
			OnPacketReceivedFnList::iterator it = packetReceivedFnList_.find( h );
			if (it != packetReceivedFnList_.end())
				packetReceivedFnList_.erase( it );
		}

		struct statistics_t
		{
			size_t		numConnectionAttempts_;
			size_t		numSuccessfulConnectionAttempts_;
			size_t		numRefusedConnectionAttempts_;
			size_t		numConnectionAttemptTimeouts_;
			size_t		numConnectionTimeouts_;
			size_t		numMaxLiveClients_;
		};
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
		CallbackHandle					lastPacketReceivedCbHandle_;

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
				return state_;
			}
			void set(const State state)
			{
				state_ = state;
			}
		private:
			State					state_;
		} m_state;

		ENetAddress				m_address;
		ENetHost*				m_host;
		uint32					m_id;

		// Could use std::vector<uint32> if not for IPv6 and the std::string interface of IPacketServer.
		typedef std::vector<std::string> HostList;
		HostList				m_ipWhiteList;
		HostList				m_ipBlackList;

		typedef std::map<uint32,std::string> IpToString;
		IpToString				m_ip2string;

		enum ClientState
		{
			CS_CONNECTING,
			CS_CONNECTED,
			CS_DISCONNECTING,
			CS_DEAD
		};
		struct Client
		{
			double		timeLeft_;
			ENetPeer*	peer;
			ClientState	state;
			PeerId		id;
			uint32		host;
			//Address		addr;

			/*
			void onConnectStage1(...);
			void onConnectStage2(...);
			void onConnected(...);
			void onDisconnected();
			*/
		};
		typedef std::map<ENetPeer*,Client*> PeerToClientMap;
		PeerToClientMap			m_clients; // access only via update()
		typedef std::map<PeerId,Client*> IdToClientMap;
		IdToClientMap			m_id2client;
		PeerId					m_nextPeerId;
		net::Timer				m_timer;
		double					m_timeOfLastUpdate;
	private:
		ClientState getClientState(ENetPeer* peer) const; // access only via update()
	};

} // namespace impl
} // namespace net

#endif
