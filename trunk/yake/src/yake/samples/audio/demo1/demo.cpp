#include <yake/samples/audio/demo1/pch.h>

int main(int argc,char* argv[])
{
	std::cout << "starting...\n";

	// loading & starting plugin
	using namespace yake;
	SharedPtr<base::Library> pLib( new base::Library( "audioOpenAL" ) );
	SharedPtr<audio::IAudioSystem> pAudio = create_default<audio::IAudioSystem>();

	// create world
	SharedPtr<audio::IWorld> pWorld;
	pWorld.reset( pAudio->createWorld() );

	// init listener
	SharedPtr<audio::IListener> pListener;
	pListener.reset( pWorld->createListener() );
	pWorld->setActiveListener( pListener.get() );

	// load audio data
	SharedPtr<audio::ISource> pSource1;
	pSource1.reset( pWorld->createSource() );

	// create sound souce & attach audio data
	SharedPtr<audio::ISoundData> pData1;
	pData1.reset( pWorld->createSoundDataFromFile("../../media/audio/Forest1.L.wav") );

	pSource1->setSoundData( pData1.get() );

	// start playing
	pSource1->play();

	// wait for stop
	std::cout << "running...\npress a key to stop...\n";
	while (!_kbhit())
		::Sleep(10);
	_getch();

	pSource1.reset();
	pData1.reset();
	pListener.reset();
	pWorld.reset();
	pAudio.reset();
	pLib.reset();

	return 0;
}