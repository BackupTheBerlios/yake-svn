//****************************************************************************
//**
//**    Sample - EXPERIMENTAL CODE
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yapp/samples/base/events/yakePCH.h>
#include <yapp/base/yapp.h>
#include <yapp/base/event/yakeEvent.h>
#include <yapp/base/event/yakeParamHolder.h>

using namespace yake;
using namespace yake::base::templates;
using namespace yake::base::math;

/// PURE EXPERIMENTAL STUFF AROUND HERE :)
namespace yake {
namespace base {

	class INode
	{
	public:
		virtual ~INode() {}

		virtual INode* getParent() const = 0;
		virtual INode* getChildByName( const String & id ) const = 0;
		virtual INode* removeChildByName( const String & id ) const = 0;
		virtual INode* removeChild( const INode* pNode ) const = 0;
		virtual void setParent( INode* pNode ) = 0;
		virtual void addChild( INode* pNode, const String & id = "" ) = 0;
	};

}
}

namespace yapp {
namespace event {

	class MessageListenerNode : public MessageListener, public INode
	{
	public:
	};

}
}

using namespace yapp;
using namespace yapp::event;

const MessageId MSGID_FRAME = MessageIdManager::createId();

class TheApp
{
public:
	class M1Listener : public MessageListener
	{
	public:
		virtual MsgResultCode onMessage( const MessageId id, yapp::event::ParamHolder* pParams )
		{
			if (kMsgHandled == MessageListener::onMessage( id, pParams ))
				return kMsgHandled;
			if (id == MSGID_FRAME)
			{
				std::cout << "Wow! It's a message 1! Did I really expect one?\n";
				if (pParams)
				{
					// case 1 : dump all parameters
					yapp::event::ParamHolder::StringVector keys;
					keys = pParams->getKeys();
					ConstVectorIterator<yapp::event::ParamHolder::StringVector> it( keys.begin(), keys.end() );
					while (it.hasMoreElements())
					{
						String key = it.getNext();
						std::cout << key;
						yapp::event::ParamHolder::Param value = pParams->get(key);
						std::cout << " [type=" << value.which() << "] ";
						std::cout << " = ";
						if (value.which() == yapp::event::ParamHolder::kString)
							std::cout << varGet<String>(value) << std::endl;
						else
							std::cout << std::endl;
					}

					// case 2 : we expect a certain parameter of type String
					yapp::event::ParamHolder::Param value = pParams->get("aString");
					//YAKE_ASSERT( value.which() != yapp::event::ParamHolder::kNone ); // superfluous, check for error condition...
					YAKE_ASSERT( value.which() == yapp::event::ParamHolder::kString ); // to be sure...
					std::cout << "Retrieving expected param 'aString' = " << varGet<String>(value) << std::endl;

					// case 3 : we expect a certain parameter of type Pointer
					//value = pParams->get("aPointer");
					//YAKE_ASSERT( value.which() == yapp::event::ParamHolder::kPointer ); // to be sure...
					//std::cout << "Retrieving expected param 'aPointer' = " << varGet<yapp::event::ParamHolder::Pointer>(value).get() << std::endl;
				}
				return kMsgHandled;
			}
			return kMsgUnhandled;
		};
	};
	class M2Listener : public MessageListener
	{
	public:
		M2Listener( const String & name, const MsgResultCode code = kMsgUnhandled ) :
			mRetCode(code),
			mName(name)
		{}
		virtual MsgResultCode onMessage( const MessageId id, yapp::event::ParamHolder* pParams )
		{
			std::cout << "> M2 (" << mName << ") received " << id << std::endl;
			return mRetCode;
		};
	private:
		MsgResultCode	mRetCode;
		String			mName;
	};
	void run()
	{
		// construct message instance
		yapp::event::ParamHolder par1;
		par1.set( "aString", "aloha" );
		par1.set( "aString", "aloha2" );
		par1.set( "anotherString", "aloha" );
//		par1.set( "aPointer", yapp::event::ParamHolder::Pointer(&par1) );

		M1Listener L1;

		// post message

		// 1. using a message instance
		MessageInstance m1( MSGID_FRAME, &par1 );
		L1.onMessage( m1.id(), m1.params() );

		// 2. directly
		L1.onMessage( MSGID_FRAME, &par1 );

		// message manager

		MessageManager mgr;
		M2Listener L2("L2-return default");
		M2Listener L3("L3-return kHandled", kMsgHandledExclusively);
		M2Listener L4("L4-return default");

		// 1. testing without 'origin' subscription
		// 1.1 connect listeners
		mgr.connect( &L2, MSGID_FRAME );
		mgr.connect( &L3, MSGID_FRAME );
		mgr.connect( &L4, MSGID_FRAME );

		// 1.2 post a message & execute it immediately
		mgr.postMessage( MSGID_FRAME, &par1, 0, true );

		// 1.3 post messages and queue them for later execution
		mgr.postMessage( MSGID_FRAME, &par1, 0, false );
		mgr.executeQ();

		// 2. testing with 'origin'
		mgr.disconnect( MSGID_FRAME );
		mgr.postMessage( MSGID_FRAME, &par1, 0, true ); // just making sure

		// 2.1 (re)connect listeners
		void* origin = this;
		mgr.connect( &L2, MSGID_FRAME, origin );
		mgr.connect( &L3, MSGID_FRAME, origin );
		mgr.connect( &L4, MSGID_FRAME, origin );

		// 2.2 sending message without origin -> shouldn't be handled!
		mgr.postMessage( MSGID_FRAME, &par1, 0, true );

		// 2.3 sending message with origin
		mgr.postMessage( MSGID_FRAME, &par1, origin, true );
	}
};

int main()
{
	try
	{
		std::cout << std::endl << "Demo provided for YAKE by Stephan Kaiser" << std::endl << std::endl;
		TheApp theApp;
		theApp.run();
	}
	catch (const yake::base::Exception & e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}
