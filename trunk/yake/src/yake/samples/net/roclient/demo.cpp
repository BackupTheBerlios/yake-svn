#include <yake/samples/net/roclient/pch.h>
#include <yake/net/net.h>
#include <yake/samples/net/common/common.h>
#include <yake/samples/net/roclient/ROClient.h>

int main(int argc, char* argv[])
{
	net::initialize();

	ro::client clientApp;

	if (!clientApp.start( net::Address("127.0.0.1",40000) ))
		return 0;

	clientApp.waitForStart();
	if (clientApp.running())
	{
		COUTLN("waiting for key press...");
		while (!_kbhit())
		{
			net::native::sleep(10);
			net::update();
			clientApp.step();
		}
		if (_kbhit())
			_getch();
	}
	clientApp.stop();

	net::shutdown();
	return 0;
}

