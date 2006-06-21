#include <yake/samples/net/packet/pch.h>
#include <yake/net/net.h>
#include <yake/base/yake.h> // for YAKE_ASSERT etc
#include <yake/samples/net/common/common.h>

/** A simple server class for managing packet connections.
*/
struct server
{
private:
	server& operator=(const server&);
	server(const server&);
public:
	server(const net::Address&, const net::uint32 maxClients);
	~server();

	bool start();
	void stop();

	void broadcastHello();
	size_t numConnectedClients() const
	{
		return conn_ ? conn_->getNumConnectedClients() : 0;
	}
private:
	void onStarted();
private:
	safe_var<bool>					started_;
	net::Address					addr_;
	net::uint32						maxClients_;
	net::IServerPacketConnection*	conn_;
};
server::server(const net::Address& addr, const net::uint32 maxClients) : 
	addr_(addr), 
	maxClients_(maxClients),
	conn_(0), 
	started_(false)
{
	YAKE_ASSERT( maxClients > 0 );
}
server::~server()
{
	stop();
}
void server::onStarted()
{
	started_ = true;
}
bool server::start()
{
	YAKE_ASSERT( !started_.getCopy() );
	YAKE_ASSERT( !conn_ );

	YAKE_LOG("creating server...\n");
	conn_ = net::createServerPacketConnection();
	YAKE_ASSERT( conn_ );

	YAKE_LOG("starting server...\n");
	started_ = false;
	conn_->addStartedCallback(boost::bind(&server::onStarted,this));
	try {
		conn_->start(addr_, maxClients_);
	} 
	catch (net::Exception& e)
	{
		YAKE_LOG_ERROR( yake::String("NET Exception\n") + e.what() );
		YAKE_SAFE_DELETE( conn_ );
		return false;
	}
	while (!started_.getCopy())
	{
		net::native::sleep(10);
		net::update();
	}
	COUTLN("server is running.");

	return true;
}
void server::stop()
{
	if (!conn_)
		return;
	conn_->stop();
	YAKE_SAFE_DELETE( conn_ );
}
void server::broadcastHello()
{
	YAKE_ASSERT( conn_ );
	YAKE_ASSERT( started_.getCopy() );
	std::string hello("hello!");
	conn_->send( hello.c_str(), hello.size(), net::SendOptions().reliability(net::R_RELIABLE) );
	COUTLN("server:broadcastHello() " << conn_->getNumConnectedClients() << " client(s)");
}

void client_onPacketReceived(const net::PeerId peerId, const void* data, const size_t len, const net::ChannelId channel)
{
	COUTLN("  client " << peerId << " packet(size=" << len << ") on channel " << channel << "\n");
}

int main(int argc, char* argv[])
{
	net::initialize();

	// server
	server theServer(net::Address(4000), 16);
	if (!theServer.start())
		return 0;

	// client(s)
	YAKE_LOG("creating clients...\n");
	typedef std::deque<yake::SharedPtr<net::IClientPacketConnection> > ClientConnections;
	ClientConnections clients;
	for (size_t i=0; i<3; ++i)
	{
		net::IClientPacketConnection* client = net::createClientPacketConnection();
		YAKE_ASSERT( client );
		client->connect(net::Address(4000), false /*true=block until connection established or timeout.*/);
		client->addPacketReceivedCallback( client_onPacketReceived );
		clients.push_back( yake::SharedPtr<net::IClientPacketConnection>(client) );
	}

	//net::native::sleep(500); // unfortunately, this is still necessary...

	// main loop
	YAKE_LOG("waiting for key press...");
	bool sentHello = false;
	while (!_kbhit())
	{
		net::native::sleep(10);
		net::update();
		if (!sentHello && theServer.numConnectedClients() > 0)
		{
			theServer.broadcastHello();
			sentHello = true;
		}
	}
	if (_kbhit())
		_getch();

	// cleanup
	YAKE_LOG("stopping clients...\n");
	yake::ConstVectorIterator<ClientConnections> itClient(clients);
	while (itClient.hasMoreElements())
		itClient.getNext()->disconnect();
	clients.clear();

	theServer.stop();

	net::shutdown();
	return 0;
}

