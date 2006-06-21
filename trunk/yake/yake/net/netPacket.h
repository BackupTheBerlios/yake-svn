#ifndef NET_PACKET_H
#define NET_PACKET_H

namespace net {

	typedef std::vector<uint8> byte_chunk;

	/** Interface for packet sending objects.
	*/
	class NET_API IPacketSender
	{
	public:
		virtual ~IPacketSender() {}
		virtual void send(const void*, const size_t, const SendOptions& opt = SendOptions()) = 0;
		virtual void send(const PeerId, const void*, const size_t, const SendOptions& opt = SendOptions()) = 0;
	};

#pragma warning(push)
#pragma warning(disable: 4251) // class 'X' needs to have dll-interface to be used by clients of struct 'net::CallbackConnection'
	typedef uint32 CallbackHandle;
	struct NET_API CallbackConnection
	{
	public:
		typedef boost::function<void(const CallbackHandle)> DisconnectFn;
		CallbackConnection(const CallbackHandle h, const DisconnectFn& fn);
		CallbackConnection(const CallbackConnection&);
		CallbackConnection& operator=(const CallbackConnection&);
		~CallbackConnection(); // does nothing!
		void disconnect();
	private:
		DisconnectFn	disconnectFn_;
		CallbackHandle	handle_;
	};
#pragma warning(pop)

	/** Interface for packet receiving objects.
	*/
	class NET_API IPacketReceiver
	{
	public:
		virtual ~IPacketReceiver() {}

		typedef boost::function<void(const PeerId, const void*, const size_t, const ChannelId)> OnPacketReceivedFn;
		virtual CallbackConnection addPacketReceivedCallback( const OnPacketReceivedFn&) = 0;
	};

	/** Interface for packet sending and receiving objects (connections).
	*/
	class NET_API IPacketConnection : public IPacketSender, public IPacketReceiver
	{
	public:
		typedef boost::function<void(void)> OnStartedFn;
		typedef boost::function<void(void)> OnTimeOutFn;
		virtual void addStartedCallback(const OnStartedFn&) = 0;
	};

	/** A packet connection interface specific to packet servers.
		Packet servers manage zero or more connected clients.
		@see IClientPacketConnection
	*/
	class NET_API IServerPacketConnection : public IPacketConnection
	{
	public:

		/*** @Remarks May throw exception. */
		virtual void start( const Address&, const size_t ) = 0;
		virtual void stop() = 0;

		virtual void setAllowedClientIps(const std::vector<std::string>&) = 0;
		//virtual void setAllowedClientIps(const std::vector<uint32>&) = 0;

		virtual size_t getNumConnectedClients() const = 0;

		virtual void disconnect( const PeerId ) = 0;

		typedef boost::function<void(const PeerId, const Address&)> OnClientConnectedFn;
		typedef boost::function<void(const PeerId)> OnClientDisconnectedFn;

		/** Not thread-safe! Call only before calling start()! */
		virtual void addClientConnectedCallback(const OnClientConnectedFn&) = 0;
		virtual void addClientDisconnectedCallback(const OnClientDisconnectedFn&) = 0;
	};
	/** A packet connection interface specific to packet clients.
		Packet clients manage a single connection to a packet server.
		@see IClientPacketConnection
	*/
	class NET_API IClientPacketConnection : public IPacketConnection
	{
	public:
		virtual void connect( const Address&, const bool doBlock = false, const uint32 timeOut = 2000 ) = 0;
		virtual void disconnect() = 0;

		virtual void addTimeOutCallback(const OnTimeOutFn&) = 0;
	};
	/** @Remarks User code is responsible for destroying the connection objects!*/
	NET_API IServerPacketConnection* createServerPacketConnection();
	/** @Remarks User code is responsible for destroying the connection objects!*/
	NET_API IClientPacketConnection* createClientPacketConnection();

} // namespace net

#endif
