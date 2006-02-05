#ifndef NET_ENETCLIENTPACKETCONNECTION_H
#define NET_ENETCLIENTPACKETCONNECTION_H

namespace net {
namespace impl {

	//--------------------------------------------------------------------------
	class EnetClientPacketConnection : public ::net::IClientPacketConnection
	{
	public:
		EnetClientPacketConnection();
		virtual ~EnetClientPacketConnection();

		virtual void connect( const Address& );
		virtual void disconnect();

		virtual void send(const void*, const size_t, const SendOptions& opt = SendOptions());
		virtual void send(const PeerId, const void*, const size_t, const SendOptions& opt = SendOptions());

		virtual void addStartedCallback(const OnStartedFn&);
		virtual CallbackConnection addPacketReceivedCallback( const OnPacketReceivedFn&);
		virtual void addTimeOutCallback(const OnTimeOutFn&);

		void disconnectPacketReceivedCallback(const CallbackHandle h)
		{
			boost::mutex::scoped_lock lck(packetReceivedFnListMtx_);
			OnPacketReceivedFnList::iterator it = packetReceivedFnList_.find(h);
			if (it == packetReceivedFnList_.end())
				return;
			packetReceivedFnList_.erase( it );
		}
	private:
		void sendTo(const PeerId clientId, const void* dataPtr, const size_t dataSize, const Reliability rel, const Ordering, const ChannelId channel);
		void sendBroadcast(const void* dataPtr, const size_t dataSize, const Reliability rel, const Ordering, const ChannelId channel);
	private:
		void update();

		void fireCallback_Started()
		{
			for (OnStartedFnList::const_iterator it = startedFnList_.begin(); it != startedFnList_.end(); ++it)
				(*it)();
		}
		void fireCallback_PacketReceived(const PeerId peerId, const void* data, const size_t dataLen, const ChannelId channel)
		{
			boost::mutex::scoped_lock lck(packetReceivedFnListMtx_);
			for (OnPacketReceivedFnList::const_iterator it = packetReceivedFnList_.begin(); it != packetReceivedFnList_.end(); ++it)
				(it->second)(peerId,data,dataLen,channel);
		}
		void fireCallback_TimeOut()
		{
			for (OnTimeOutFnList::const_iterator it = timeOutFnList_.begin(); it != timeOutFnList_.end(); ++it)
				(*it)();
		}
	private:
		typedef std::deque<OnStartedFn> OnStartedFnList;
		typedef std::map<CallbackHandle,OnPacketReceivedFn> OnPacketReceivedFnList;
		typedef std::deque<OnTimeOutFn> OnTimeOutFnList;
		OnStartedFnList					startedFnList_;
		OnPacketReceivedFnList			packetReceivedFnList_;
		boost::mutex						packetReceivedFnListMtx_;
		OnTimeOutFnList					timeOutFnList_;
		CallbackHandle						lastPacketReceivedCbHandle_;

	private:
		ENetAddress		m_address;
		ENetHost*		m_host;
		ENetPeer*		m_peer;
		ENetPeer*		m_serverPeer;
		bool			m_ready;
		uint32			m_id;
		bool			m_waitingForConnect;
		std::string		m_serverIp;
		Timer			m_connTimer;
	};

} // namespace impl
} // namespace net

#endif
