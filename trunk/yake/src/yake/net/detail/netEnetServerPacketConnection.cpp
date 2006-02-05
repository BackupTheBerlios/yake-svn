#include <yake/net/pch.h>
#include <enet/enet.h>
#include <yake/net/net.h>
#include <yake/net/detail/netInternal.h>
#include <yake/net/detail/netEnetServerPacketConnection.h>

namespace net {
	IServerPacketConnection* createServerPacketConnection()
	{
		return new impl::EnetServerPacketConnection();
	}
namespace impl {

	EnetServerPacketConnection::EnetServerPacketConnection() : m_host(0), m_id(0), m_nextPeerId(0)
	{
		m_state.set(S_DEAD);
	}
	EnetServerPacketConnection::~EnetServerPacketConnection()
	{
		if (m_state.get() != S_DEAD)
			this->stop();
	}
	void EnetServerPacketConnection::addStartedCallback(const OnStartedFn& fn)
	{
		startedFnList_.push_back( fn );
	}
	void EnetServerPacketConnection::addClientConnectedCallback(const OnClientConnectedFn& fn)
	{
		clientConnectedFnList_.push_back( fn );
	}
	void EnetServerPacketConnection::addClientDisconnectedCallback(const OnClientDisconnectedFn& fn)
	{
		clientDisconnectedFnList_.push_back( fn );
	}
	CallbackConnection EnetServerPacketConnection::addPacketReceivedCallback( const OnPacketReceivedFn& fn)
	{
		boost::mutex::scoped_lock lck(packetReceivedFnListMtx_);
		packetReceivedFnList_.insert( std::make_pair(++lastPacketReceivedCbHandle_,fn) );
		return CallbackConnection(lastPacketReceivedCbHandle_,boost::bind(&EnetServerPacketConnection::disconnectPacketReceivedCallback,this,_1));
	}
	void EnetServerPacketConnection::setAllowedClientIps(const std::vector<std::string> &ips)
	{
		boost::mutex::scoped_lock ipsLock(m_ipListMtx);
		m_ipWhiteList = ips;
	}
	bool EnetServerPacketConnection::isIpAllowed( const std::string& ip )
	{
		boost::mutex::scoped_lock ipsLock(m_ipListMtx);
		if (m_ipWhiteList.empty() && m_ipBlackList.empty())
			return true;
		if (contains(m_ipBlackList,ip))
			return false;
		if (m_ipWhiteList.empty())
			return true;
		return (contains(m_ipWhiteList,ip));
	}
	void EnetServerPacketConnection::start(const Address& addr, const size_t maxClients)
	{
		NET_ASSERT( m_state.get() == S_DEAD );
		if (m_state.get() != S_DEAD)
		{
			NET_THROW("Object is not dead. Cannot start!");
		}
		m_state.set(S_STARTING);
		if (maxClients == 0)
		{
			m_state.set(S_DEAD);
			NET_THROW("Maximum number of clients must be larger than 0.");
			return;
		}
		if (addr.ip().empty())
		{
			m_address.host = ENET_HOST_ANY;
		}
		else
		{
			const std::string ip = (addr.ip() == "localhost") ? "127.0.0.1" : addr.ip();
			int ret = enet_address_set_host( &m_address, ip.c_str() );
			if (ret != 0)
			{
				m_state.set(S_DEAD);
				NET_THROW("Could not set server IP!");
				return;
			}
		}

		{
			boost::mutex::scoped_lock enetLock(getEnetMtx());
			m_address.port = addr.port();
			m_host = enet_host_create( &m_address,
										maxClients, // up to 'maxClients' clients
										0,  // assume any amount of incoming bandwidth
										0); // assume any amount of outgoing bandwidth
			if (!m_host)
			{
				m_state.set(S_DEAD);
				NET_THROW("Could not create server!");
				return;
			}
		}

		m_state.set(S_RUNNING);
		this->fireCallback_Started();
		m_id = UpdateThread::instance().add( boost::bind(&EnetServerPacketConnection::update,this) );
	}
	void EnetServerPacketConnection::stop()
	{
		if (m_id)
		{
			UpdateThread::instance().remove( m_id );
			m_id = 0;
		}
		if (m_state.get() != S_RUNNING)
		{
			if (m_state.get() == S_DEAD)
				return;
		}
		m_state.set(S_STOPPING);
		{
			NET_ASSERT( m_host );
			if (m_host)
			{
				if (m_host->peerCount > 0)
				{
					{
						boost::mutex::scoped_lock enetLock(getEnetMtx());
						// try to gracefully disconnect clients
						for (size_t i=0; i<m_host->peerCount; ++i)
							enet_peer_disconnect( &m_host->peers[i] );
					}
					Timer timer;
					timer.start();
					while (timer.getTime() < 2) // wait 2 seconds for acknowledgement
					{
						::Sleep(10);
						this->update();
					}
					// forcefully disconnect remaining clients
					if (m_host->peerCount > 0)
					{
						boost::mutex::scoped_lock enetLock(getEnetMtx());
						for (size_t i=0; i<m_host->peerCount; ++i)
							enet_peer_reset( &m_host->peers[i] );
					}
				}
				{
					boost::mutex::scoped_lock enetLock(getEnetMtx());
					enet_host_destroy( m_host );
					m_host = 0;
				}
			}
		}
		m_ip2string.clear();
		m_ipWhiteList.clear();
		{
			boost::mutex::scoped_lock lockClients(m_clientsMtx);
			for (PeerToClientMap::iterator it = m_clients.begin(); it != m_clients.end(); ++it)
				delete it->second;
			m_clients.clear();
			m_id2client.clear();
		}
		//m_events.clear();
		m_state.set(S_DEAD);
	}
	void EnetServerPacketConnection::disconnect( const PeerId client )
	{
		boost::mutex::scoped_lock lockClients(m_clientsMtx);
		IdToClientMap::iterator itFindClient = m_id2client.find( client );
		NET_ASSERT( itFindClient != m_id2client.end() );
		if (itFindClient == m_id2client.end())
			return;
		// set client state
		itFindClient->second->state = CS_DISCONNECTING;
		// disconnect enet client
		{
			boost::mutex::scoped_lock enetLock(getEnetMtx());
			enet_peer_disconnect( itFindClient->second->peer );
		}
	}
	EnetServerPacketConnection::ClientState EnetServerPacketConnection::getClientState(ENetPeer* peer) const
	{
		if (peer == 0)
			return CS_DEAD;

		boost::mutex::scoped_lock lockClients(m_clientsMtx);
		PeerToClientMap::const_iterator it = m_clients.find( peer );
		if (it == m_clients.end())
			return CS_DEAD;
		return it->second->state;
	}
	std::string EnetServerPacketConnection::ipToStringCached( const uint32 host )
	{
		IpToString::const_iterator itFind = m_ip2string.find( host );
		if (itFind != m_ip2string.end())
			return itFind->second;
		const std::string strHost = ipToString(host);
		m_ip2string.insert( std::make_pair( host, strHost ) );
		return strHost;
	}
	void EnetServerPacketConnection::update()
	{
		State state = m_state.get();
		NET_ASSERT( state == S_RUNNING || state == S_STOPPING  );
		if (state != S_RUNNING && state != S_STOPPING)
			return;

		// packet handling
		ENetEvent event;
		int ret = 0;
		{
			boost::mutex::scoped_lock enetLock(getEnetMtx());
			ret = enet_host_service(m_host, &event, 5);
		}
		if (ret == 0) // no events
		{}
		else if (ret > 0) // event received
		{
			switch(event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				NET_LOG("net_packet_server: new client connected.");
				NET_ASSERT( event.peer );
				if (event.peer && state != S_STOPPING)
				{
					// white-list / ban checking - @todo it would be better to do this at a lower level!
					const std::string host = ipToStringCached( event.peer->address.host );

					NET_LOG("net_packet_server: new client attempts to connect: '" << host << ":" << event.peer->address.port << "'.");

					if (!isIpAllowed(host))
					{
						NET_LOG("net_packet_server: new client '" << host << "' is NOT ALLOWED to connect. disconnecting.");
						boost::mutex::scoped_lock enetLock(getEnetMtx());
						//enet_disconnect_peer( event.peer ); // gracefully
						enet_peer_reset( event.peer ); // hard!
					}

					PeerId peerId = 0xffffffff;
					{
						boost::mutex::scoped_lock lockClients(m_clientsMtx);

						Client* c = new Client();
						c->state = CS_CONNECTED;
						c->peer = event.peer;
						c->id = m_nextPeerId++;
						peerId = c->id;
						c->host = event.peer->address.host;
						event.peer->data = c; //!

						m_clients.insert( std::make_pair( event.peer, c ) );
						m_id2client.insert( std::make_pair( c->id, c ) );
					}

					fireCallback_ClientConnected( peerId, Address(host,event.peer->address.port) );
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				NET_ASSERT( event.packet );
				NET_ASSERT( event.peer );
				NET_ASSERT( event.peer->data );
				if (getClientState( event.peer ) == CS_CONNECTED
					&& state != S_STOPPING)
				{
					// create packet object
					NET_ASSERT( event.packet->dataLength > 0 );
					NET_ASSERT( event.packet->data );

					// get "id"
					PeerId peerId = 0xffffffff;
					{
						boost::mutex::scoped_lock lockClients(m_clientsMtx);
						peerId = (reinterpret_cast<Client*>(event.peer->data))->id;
					}

					//Note: The callbacks can take over ownership/destruction of the packet!
					bool bPacketDeleted = false;
					fireCallback_PacketReceived(peerId,event.packet->data,event.packet->dataLength,event.channelID);
				}
				{
					// destroy enet packet
					boost::mutex::scoped_lock enetLock(getEnetMtx());
					enet_packet_destroy( event.packet );
				}
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				{
					NET_ASSERT( event.peer );
					uint32 host = event.peer ? event.peer->address.host : 0;
					PeerId peerId = 0xffffffff;
					if (event.peer)
					{
						boost::mutex::scoped_lock lockClients(m_clientsMtx);
						Client* c = reinterpret_cast<Client*>(event.peer->data);
						if (c)
							peerId = c->id;
						PeerToClientMap::iterator it = m_clients.find( event.peer );
						if (it != m_clients.end())
						{
							m_id2client.erase( m_id2client.find( it->second->id ) );
							host = it->second->host;
							delete it->second;
							m_clients.erase( it );
						}
					}
					fireCallback_ClientDisconnected(peerId);

					const std::string strHost = host ? ipToStringCached( host ) : "<null host>";
					NET_LOG("net_packet_server: client '" << strHost << "' disconnected.");
				}
				break;
			default:
				break;
			};
		}
	}
	void EnetServerPacketConnection::send( const void* dataPtr, const size_t dataSize, const net::SendOptions& opt )
	{
		if (opt.peerId != 0xffffffff)
			this->sendTo( opt.peerId, dataPtr, dataSize, opt.reliability, opt.ordering, opt.channelId );
		else
			this->sendBroadcast( dataPtr, dataSize, opt.reliability, opt.ordering, opt.channelId );
	}
	void EnetServerPacketConnection::send(const PeerId peerId, const void* dataPtr, const size_t dataSize, const SendOptions& opt )
	{
		SendOptions options = opt;
		options.setPeerId( peerId );
		send( dataPtr, dataSize, options );
	}
	void EnetServerPacketConnection::sendTo(const PeerId clientId, const void* dataPtr, const size_t dataSize, const Reliability rel, const Ordering, const ChannelId channel)
	{
		NET_ASSERT( dataSize > 0 );
		NET_ASSERT( dataPtr );
		{
			NET_ASSERT( m_state.get() == S_RUNNING );
			if (m_state.get() != S_RUNNING)
				return;
		}
		{
			boost::mutex::scoped_lock enetLock(getEnetMtx());
			ENetPacket* packet = enet_packet_create( 
														dataPtr, dataSize,
														(rel == R_RELIABLE) ? ENET_PACKET_FLAG_RELIABLE : 0 );
			// send
			boost::mutex::scoped_lock clientsLock(m_clientsMtx);
			IdToClientMap::const_iterator it = m_id2client.find( clientId );
			NET_ASSERT( it != m_id2client.end() );
			enet_peer_send( it->second->peer, channel, packet );
		}
	}
	void EnetServerPacketConnection::sendBroadcast(const void* dataPtr, const size_t dataSize, const Reliability rel, const Ordering, const ChannelId channel)
	{
		NET_ASSERT( dataSize > 0 );
		NET_ASSERT( dataPtr );
		{
			NET_ASSERT( m_state.get() == S_RUNNING );
			if (m_state.get() != S_RUNNING)
				return;
		}
		{
			boost::mutex::scoped_lock enetLock(getEnetMtx());
			ENetPacket* packet = enet_packet_create( 
														dataPtr, dataSize,
														(rel == R_RELIABLE) ? ENET_PACKET_FLAG_RELIABLE : 0 );
			enet_host_broadcast( m_host, channel, packet );
		}
	}

} // namespace impl
} // namespace net
