#ifndef NET_PACKET_H
#define NET_PACKET_H

namespace net {

	typedef std::vector<uint8> byte_chunk;

	class NET_API IPacketSender
	{
	public:
		virtual ~IPacketSender() {}
		virtual void send(const void*, const size_t, const SendOptions& opt = SendOptions()) = 0;
		virtual void send(const PeerId, const void*, const size_t, const SendOptions& opt = SendOptions()) = 0;
	};

	class NET_API IPacketReceiver
	{
	public:
		virtual ~IPacketReceiver() {}

		typedef boost::function<void(const PeerId, const void*, const size_t, const ChannelId)> OnPacketReceivedFn;
		virtual void addPacketReceivedCallback( const OnPacketReceivedFn&) = 0;
	};

	class NET_API IPacketConnection : public IPacketSender, public IPacketReceiver
	{
	public:
		typedef boost::function<void(void)> OnStartedFn;
		typedef boost::function<void(void)> OnTimeOutFn;
		virtual void addStartedCallback(const OnStartedFn&) = 0;
	};

	class NET_API IServerPacketConnection : public IPacketConnection
	{
	public:

		/*** May throw exception. */
		virtual void start( const Address&, const size_t ) = 0;
		virtual void stop() = 0;

		virtual void setAllowedClientIps(const std::vector<std::string>&) = 0;
		//virtual void setAllowedClientIps(const std::vector<uint32>&) = 0;

		virtual void disconnect( const PeerId ) = 0;

		typedef boost::function<void(const PeerId, const Address&)> OnClientConnectedFn;
		typedef boost::function<void(const PeerId)> OnClientDisconnectedFn;

		/** Not thread-safe! Call only before calling start()! */
		virtual void addClientConnectedCallback(const OnClientConnectedFn&) = 0;
		virtual void addClientDisconnectedCallback(const OnClientDisconnectedFn&) = 0;
	};
	class NET_API IClientPacketConnection : public IPacketConnection
	{
	public:
		virtual void connect( const Address& ) = 0;
		virtual void disconnect() = 0;

		virtual void addTimeOutCallback(const OnTimeOutFn&) = 0;
	};
	NET_API IServerPacketConnection* createServerPacketConnection();
	NET_API IClientPacketConnection* createClientPacketConnection();

} // namespace net

#endif