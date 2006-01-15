#include <yake/samples/net/roserver/pch.h>
#include <yake/net/net.h>
#include <yake/samples/net/common/common.h>
#include <yake/samples/net/roserver/ROServer.h>

#include <yake/object/yakeObjects.h>
#include <yapp/ent/yakeEnt.h>

int main(int argc, char* argv[])
{
	try {

		COUTLN("starting RO server...");
		if (!net::initialize())
			return 0;

		ro::server serverApp;
		serverApp.start();

		COUTLN("waiting for key press...");
		while (!_kbhit())
			net::native::sleep(10);
		_getch();

		COUTLN("stopping...");
		serverApp.stop();

		net::shutdown();
	}
	catch (const net::Exception& e)
	{
		COUTLN("Caught net exception: " << e.what());
		net::shutdown();
	}
	return 0;
}

