#ifndef _CEGUIOgre_TestDriver1_h_
#define _CEGUIOgre_TestDriver1_h_

#include <boost/shared_ptr.hpp>

#include "CEGUI.h"
#include "renderers/OgreGUIRenderer/ogrerenderer.h"
#include "ExampleApplication.h"
#include "OgreEventQueue.h"

#include "gui_test.h"

class CEGUIOgre_FrameListener : public ExampleFrameListener, Ogre::MouseMotionListener, Ogre::MouseListener
{
public:
	CEGUIOgre_FrameListener(RenderWindow* win, Camera* cam) : ExampleFrameListener(win, cam)
	{
		mEventProcessor = new EventProcessor();
		mEventProcessor->initialise(win);
		mEventProcessor->startProcessingEvents();
		mEventProcessor->addKeyListener(this);
		mEventProcessor->addMouseMotionListener(this);
		mEventProcessor->addMouseListener(this);
		mInputDevice = mEventProcessor->getInputReader();

		mQuit = false;
		mSkipCount = 0;
		mUpdateFreq = 0;
	}


	virtual void 	mouseMoved (MouseEvent *e)
	{
		CEGUI::Renderer* rend = CEGUI::System::getSingleton().getRenderer();
		CEGUI::System::getSingleton().injectMouseMove(e->getRelX() * rend->getWidth(), e->getRelY() * rend->getHeight());
		e->consume();
	}


	virtual void 	mouseDragged (MouseEvent *e)
	{
		mouseMoved(e);
	}


	virtual void 	keyPressed (KeyEvent *e)
	{
		// give 'quitting' priority
		if (e->getKey() == KC_ESCAPE)
		{
			mQuit = true;
			e->consume();
			return;
		}

		// do event injection
		CEGUI::System& cegui = CEGUI::System::getSingleton();

		// key down
		cegui.injectKeyDown(e->getKey());

		// now character
		cegui.injectChar(e->getKeyChar());

		e->consume();
	}


	virtual void 	keyReleased (KeyEvent *e)
	{
		CEGUI::System::getSingleton().injectKeyUp(e->getKey());
	}



	virtual void 	mousePressed (MouseEvent *e)
	{
		CEGUI::System::getSingleton().injectMouseButtonDown(convertOgreButtonToCegui(e->getButtonID()));
		e->consume();
	}


	virtual void 	mouseReleased (MouseEvent *e)
	{
		CEGUI::System::getSingleton().injectMouseButtonUp(convertOgreButtonToCegui(e->getButtonID()));
		e->consume();
	}

	// do-nothing events
	virtual void 	keyClicked (KeyEvent *e) {}
	virtual void 	mouseClicked (MouseEvent *e) {}
	virtual void 	mouseEntered (MouseEvent *e) {}
	virtual void 	mouseExited (MouseEvent *e) {}


	bool frameStarted(const FrameEvent& evt);

protected:
	CEGUI::MouseButton convertOgreButtonToCegui(int ogre_button_id)
	{
		switch (ogre_button_id)
		{
		case MouseEvent::BUTTON0_MASK:
			return CEGUI::LeftButton;
			break;

		case MouseEvent::BUTTON1_MASK:
			return CEGUI::RightButton;
			break;

		case MouseEvent::BUTTON2_MASK:
			return CEGUI::MiddleButton;
			break;

		case MouseEvent::BUTTON3_MASK:
			return CEGUI::X1Button;
			break;

		default:
			return CEGUI::LeftButton;
			break;
		}

	}

protected:
	float	mSkipCount;
	float	mUpdateFreq;

	bool mQuit;
};

class CEGUIOgre_TestDriver1 : public ExampleApplication
{
public:
	CEGUIOgre_TestDriver1(void) 
		: mGUIRenderer(NULL)
	{}

	virtual bool configure()
	{ 
		if(mRoot->restoreConfig())
		{
			mWindow = mRoot->initialise(true);
			return true;
		}
		else
		{
      return false;
		}
	}

	virtual void cleanup(void);

protected:
	/*************************************************************************
		Methods
	*************************************************************************/
	virtual void createScene(void);
	virtual void createFrameListener(void);

	// new demo methods
	void	createDemoWindows(void);

	/*************************************************************************
		Data
	*************************************************************************/
	CEGUI::OgreRenderer * mGUIRenderer;
	boost::shared_ptr<yake::gui_test> m_gui_test;
};


#endif	// end of guard _CEGUIOgre_TestDriver1_h_
