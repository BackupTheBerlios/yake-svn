#include "CEGUI.h"
#include "Ogre.h"
#include "renderers/OgreGUIRenderer/ogrerenderer.h"

#include "CEGUIOgre_TestDriver1.h"


#if OGRE_PLATFORM == PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{
	// Create application object
	CEGUIOgre_TestDriver1 app;

	try {
		app.go();

		// cleanup for the CEGUI bits.
		app.cleanup();
	} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " <<
            e.getFullDescription().c_str() << std::endl;
#endif
    }

    return 0;
}


/*************************************************************************
	createScene method - initialises CEGUI & adds some controls etc
*************************************************************************/
void CEGUIOgre_TestDriver1::createScene(void)
{
	// setup GUI system
	mGUIRenderer = new CEGUI::OgreRenderer(mWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000);
	new CEGUI::System(mGUIRenderer);

	// load some GUI stuff for demo.
	try
	{
		using namespace CEGUI;

		Logger::getSingleton().setLoggingLevel(Informative);

		// load scheme and set up defaults
		SchemeManager::getSingleton().loadScheme((utf8*)"../datafiles/schemes/TaharezLook.scheme");
		System::getSingleton().setDefaultMouseCursor((utf8*)"TaharezLook", (utf8*)"MouseArrow");
		System::getSingleton().setDefaultFont((utf8*)"Tahoma-12");

		Window* sheet = WindowManager::getSingleton().createWindow((utf8*)"DefaultWindow", (utf8*)"root_wnd");
		System::getSingleton().setGUISheet(sheet);

		// do demo stuff
		createDemoWindows();
	}
	// catch to prevent exit (errors will be logged).
	catch(CEGUI::Exception)
	{}
}

/*************************************************************************
	create the windows & widgets for this demo
*************************************************************************/

void CEGUIOgre_TestDriver1::createDemoWindows(void)
{
	m_gui_test.reset(new yake::gui_test());
}

/*************************************************************************
	cleanup - free some stuff and close-down CEGUI system
*************************************************************************/
void CEGUIOgre_TestDriver1::cleanup(void)
{
	delete CEGUI::System::getSingletonPtr();

	if (mGUIRenderer)
	{
		delete mGUIRenderer;
		mGUIRenderer = NULL;
	}

}

/*************************************************************************
	createFrameListener - add out frame lister which handles input
*************************************************************************/
void CEGUIOgre_TestDriver1::createFrameListener(void)
{
	mFrameListener= new CEGUIOgre_FrameListener(mWindow, mCamera);
	mFrameListener->showDebugOverlay(false);
	mRoot->addFrameListener(mFrameListener);
}


/*************************************************************************
	frameStarted - method that handles all input for this demo.
*************************************************************************/
bool CEGUIOgre_FrameListener::frameStarted(const FrameEvent& evt)
{
	if (mQuit)
		return false;
	else
	{
		updateStats();
		return true;
	}

}
