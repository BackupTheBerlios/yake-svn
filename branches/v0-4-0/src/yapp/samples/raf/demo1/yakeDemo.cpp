#include <yapp/samples/raf/demo1/yakePCH.h>
#include <yake/audio/yakeAudio.h>
#include <yake/input/yakeInput.h>
#include <yapp/raf/yakeRaf.h>

using namespace yake;

/** Configuration of the application */
struct TheConfiguration : public raf::ApplicationConfiguration
{
	virtual StringVector getLibraries()
	{ return MakeStringVector() << "scriptingLua" << "graphicsOgre" << "inputOgre"; }

	virtual StringVector getInputSystems()
	{ return MakeStringVector() << "ogre"; }

	virtual StringVector getScriptingSystems()
	{ return MakeStringVector() << "lua"; }

	virtual StringVector getGraphicsSystems()
	{ return MakeStringVector() << "ogre3d"; }
};

/** Main application state */
class TheMainState : public raf::RtMainState
{
public:
	TheMainState(raf::Application& owner) : raf::RtMainState(owner)
	{
		enableInstantQuitByKey( input::KC_ESCAPE );
	}
protected:
	virtual void onCreateScene()
	{
		YAKE_LOG_INFORMATION("Creating scene");
		// create movable ninja
		graphics::ISceneNode* pSN = getGraphicalWorld()->createSceneNode("root");
		pSN->attachEntity( getGraphicalWorld()->createEntity("ninja.mesh") );

		// create a light
		graphics::ILight* pLight = getGraphicalWorld()->createLight();
		pLight->setType( graphics::ILight::LT_DIRECTIONAL );
		pLight->setDirection( Vector3(0,-1,1) );
		getGraphicalWorld()->createSceneNode("lightnode0")->attachLight( pLight );

		// position camera and look at the ninja
		getDefaultCamera()->setFixedYawAxis(Vector3::kUnitY);
		getDefaultCamera()->setPosition(Vector3(100,20,-400));
		getDefaultCamera()->lookAt(Vector3(0,100,0));
	}
	virtual void onFrame(const real timeElapsed)
	{
	}
};

/** The mighty application itself! */
class TheApp : public raf::ExampleApplication<TheConfiguration>
{
public:
	TheApp() {}
protected:
	virtual raf::MainState* createMainState()
	{
		return new TheMainState(*this);
	}
};

int main( int argc, char** argv )
{
	// Use default executor for convenience.
	// It's always possible to manually execute TheApp::initialise() etc.
	return (raf::runApplication( TheApp() )) ? 0 : 1;
}

