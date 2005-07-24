#include <yapp/samples/raf/minimal/yakePCH.h>
#include <yapp/raf/yakeRaf.h>

using namespace yake;

/** Configuration of the application */
struct TheConfiguration : public raf::ApplicationConfiguration
{
	virtual StringVector getLibraries()
	{ return MakeStringVector() << "scriptingLua"; }

	virtual StringVector getScriptingSystems()
	{ return MakeStringVector() << "lua"; }
};

/** The mighty application itself! */
class TheApp : public raf::ExampleApplication<TheConfiguration>
{
public:
	TheApp() {}
protected:
	virtual bool onRun()
	{
		scripting::IVM* pVM = getScriptingSystem("lua")->createVM();
		YAKE_ASSERT( pVM ).debug("We need a valid VM!");
		if (!pVM)
			return false;

		pVM->execute("print('Hello from script!')");
		delete pVM;

		return true;
	}
};

int main( int argc, char** argv )
{
	// Use default executor for convenience.
	// It's always possible to manually execute TheApp::initialise() etc.
	return (raf::runApplication( TheApp() )) ? 0 : 1;
}

