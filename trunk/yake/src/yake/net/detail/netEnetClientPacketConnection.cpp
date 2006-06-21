#include <yake/net/pch.h>
#include <enet/enet.h>
#include <yake/net/net.h>
#include <yake/net/detail/netInternal.h>
#include <yake/net/detail/netEnetClientPacketConnection.h>

namespace net {
	IClientPacketConnection* createClientPacketConnection()
	{
		return new impl::EnetClientPacketConnection();
	}
namespace impl {

	EnetClientPacketConnection::EnetClientPacketConnection() : 
		m_host(0), 
		m_ready(false), 
		m_id(0), 
		m_waitingForConnect(false),
		m_peer(0), 
		m_serverPeer(0), 
		lastPacketReceivedCbHandle_(0)
	{
	}
	EnetClientPacketConnection::~EnetClientPacketConnection()
	{
		if (m_ready)
			this->disconnect();
	}
	void EnetClientPacketConnection::addStartedCallback(const OnStartedFn& fn)
	{
		startedFnList_.push_back( fn );
	}
	CallbackConnection EnetClientPacketConnection::addPacketReceivedCallback(const OnPacketReceivedFn& fn)
	{
		boost::mutex::scoped_lock lck(packetReceivedFnListMtx_);
		packetReceivedFnList_.insert( std::make_pair(++lastPacketReceivedCbHandle_, fn) );
		return CallbackConnection(lastPacketReceivedCbHandle_,boost::bind(&EnetClientPacketConnection::disconnectPacketReceivedCallback,this,_1));
	}
	void EnetClientPacketConnection::addTimeOutCallback(const OnTimeOutFn& fn)
	{
		timeOutFnList_.push_back( fn );
	}
	void EnetClientPacketConnection::connect(const Address& addr, const bool doBlock, const uint32 timeOut)
	{
		NET_ASSERT( !m_ready );
		if (m_ready)
			return;

		const std::string ip = (addr.ip() == "localhost") ? "127.0.0.1" : addr.ip();

		NET_ASSERT( !ip.empty() );
		if (ip.empty())
		{
			NET_ERROR("client: Need a valid IP! rIP=" << ip.c_str());
			NET_EXCEPT("Need a valid IP!");
		}

		{
			boost::mutex::scoped_lock enetLock(getEnetMtx());
			m_host = enet_host_create (			0 /* create a client host */,
												1 /* only allow 1 outgoing connection */,
										57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
										28800 / 8 /* 56K modem with 28 Kbps upstream bandwidth */);
			if (!m_host)
			{
				NET_ERROR("Could not create client!");
				NET_EXCEPT("Could not create client!");
			}

			int ret = enet_address_set_host( &m_address, ip.c_str() );
			NET_ASSERT( ret == 0 );
			if (ret != 0)
			{
				NET_ERROR("Could not set address!");
				NET_EXCEPT("Could not set address!");
			}
			m_address.port = addr.port();

			m_peer = enet_host_connect( m_host, &m_address, 2 /*channel count*/ );
			if (!m_peer)
			{
				NET_ERROR("Could not connect to server!");
				enet_host_destroy( m_host );
				m_host = 0;
				NET_EXCEPT("Could not connect to server!");
			}
		}
		m_waitingForConnect = true;

		m_connTimer.reset();
		m_id = UpdateThread::instance().add( boost::bind(&EnetClientPacketConnection::update,this) );

		if (doBlock)
		{
			//@todo timeout
			while (m_waitingForConnect && !m_ready)
				net::native::sleep(10);
		}
	}
	void EnetClientPacketConnection::disconnect()
	{
		if (m_id)
		{
			UpdateThread::instance().remove( m_id );
			m_id = 0;
		}
		if (!m_ready)
			return;
		NET_ASSERT( m_host );
		if (m_host)
		{
			if (m_serverPeer)
			{
				boost::mutex::scoped_lock enetLock(getEnetMtx());
				enet_peer_disconnect( m_serverPeer, 0 /*@todo data*/ );
			}
			Timer timer;
			timer.start();
			while (timer.getTime() < 1)
			{
				this->update();
				::Sleep(10);
			}
			{
				boost::mutex::scoped_lock enetLock(getEnetMtx());
				enet_peer_reset( m_peer );
				enet_host_destroy( m_host );
				m_host = 0;
			}
		}
		{
			m_ready = false;
			m_serverIp = "";
		}
		m_serverPeer = 0;
	}
	void EnetClientPacketConnection::update()
	{
		if (!m_ready && !m_waitingForConnect)
			return;
		ENetEvent event;
		int ret = 0;
		{
			boost::mutex::scoped_lock enetLock(getEnetMtx());
			ret = enet_host_service(m_host, &event, 5);
		}
		if (ret == 0) // no events
		{
			if (m_waitingForConnect)
			{
				if (m_connTimer.getTime() > 2. )
				{
					m_connTimer.stop();
					m_connTimer.reset();
					this->fireCallback_TimeOut();
				}
			}
		}
		else if (ret > 0) // event received
		{
			switch(event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				NET_ASSERT( event.peer );
				NET_LOG("net_packet_client: (stage1) connected to " << ipToString(event.peer->address.host) << ":" << event.peer->address.port << ".");
				{
					m_serverPeer = event.peer;
					m_ready = true; // we can now start sending packets

					const uint32 host = event.peer->address.host;
					m_serverIp = ipToString( host );

					const uint8 tmp = 0x7F;
					this->send(&tmp,1,SendOptions().reliability(R_RELIABLE));
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				NET_ASSERT( event.packet );
				NET_ASSERT( m_serverPeer );
				NET_ASSERT( event.packet->dataLength > 0 );
				NET_ASSERT( event.packet->data );
				if (m_waitingForConnect)
				{
					NET_LOG("net_packet_client: (stage2) connected to " << ipToString(event.peer->address.host) << ":" << event.peer->address.port << ".");
					m_waitingForConnect = false;
					this->fireCallback_Started();
				}
				else if (event.packet)
				{
					this->fireCallback_PacketReceived(0,event.packet->data,event.packet->dataLength,ChannelId(event.channelID));
				}
				{
					boost::mutex::scoped_lock enetLock(getEnetMtx());
					enet_packet_destroy( event.packet );
				}
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				NET_LOG("net_packet_client: disconnected.");
				m_serverPeer = 0;
				m_serverIp = "";
				break;
			default:
				NET_LOG("net_packet_client: unhandled event.");
				break;
			};
		}
	}
	void EnetClientPacketConnection::send( const void* dataPtr, const size_t dataSize, const net::SendOptions& opt )
	{
		this->sendBroadcast( dataPtr, dataSize, opt.getReliability(), opt.getOrdering(), opt.getChannel() );
	}
	void EnetClientPacketConnection::send(const PeerId, const void* dataPtr, const size_t dataSize, const SendOptions& opt )
	{
		this->sendBroadcast( dataPtr, dataSize, opt.getReliability(), opt.getOrdering(), opt.getChannel() );
	}
	void EnetClientPacketConnection::sendTo(const PeerId clientId, const void* dataPtr, const size_t dataSize, const Reliability rel, const Ordering ord, const ChannelId channel)
	{
		sendBroadcast(dataPtr,dataSize,rel,ord,channel);
	}
	void EnetClientPacketConnection::sendBroadcast(const void* dataPtr, const size_t dataSize, const Reliability rel, const Ordering, const ChannelId channel)
	{
		NET_ASSERT( m_ready );
		if (!m_ready)
			return;
		NET_ASSERT( dataSize > 0 );
		NET_ASSERT( dataPtr );
		NET_ASSERT( m_serverPeer );
		{
			boost::mutex::scoped_lock enetLock(getEnetMtx());
			ENetPacket* packet = enet_packet_create( 
									dataPtr, dataSize,
									//rPacket.m_data, 
									//rPacket.m_size, 
									(rel == R_RELIABLE) ? ENET_PACKET_FLAG_RELIABLE : 0 );
			enet_peer_send(m_serverPeer, channel, packet );
		}
	}

} // namespace impl
} // namespace net
