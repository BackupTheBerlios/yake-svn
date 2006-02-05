#include <yake/net/pch.h>
#include <boost/signals.hpp>
#include <enet/enet.h>
#include <yake/net/net.h>
#include <yake/net/detail/netInternal.h>
#include <yake/net/detail/netEventConnection.h>

namespace net {
	const NetEvent::id_type NetEvent::EVTID_NONE = 0xff;
	NetEvent::NetEvent(const id_type id) : id_(id)
	{
	}
	NetEvent::id_type NetEvent::id() const
	{
		return id_;
	}
	void NetEvent::setId(const id_type id)
	{
		id_ = id;
	}
	INetEventConnection::~INetEventConnection()
	{
	}
	INetEventConnection* createEventConnection()
	{
		return new impl::EventConnection();
	}
namespace impl {

	EventConnection::EventConnection() : conn_(0), dir_(NetEvent::DIR_ANY), maxErrors_(0), manualPolling_(false), started_(false)
	{
	}
	EventConnection::~EventConnection()
	{
		stop();
#ifdef _DEBUG
		std::cout << "\nnet::EventConnection statistics:\n";
		std::cout << "   total errors           : " << stats_.totalErrors() << "\n";
		std::cout << "   total events processed : " << stats_.numEventsProcessed << "\n";
		std::cout << "   blocked event ids      : " << stats_.numBlockedEventIds << "\n";
		std::cout << "   invalid packets        : " << stats_.numInvalidPackets << "\n";
		std::cout << "   invalid events         : " << stats_.numInvalidEvents << "\n";
		std::cout << "   invalid event ids      : " << stats_.numInvalidEventIds << "\n";
		std::cout << "   unregistered event ids : " << stats_.numUnregisteredEventIds << "\n";
		std::cout << "   wrong direction        : " << stats_.numWrongEventDirection << "\n";
		std::cout << "\n";
#endif
	}
	void EventConnection::setMaxEventErrors(const size_t maxErrors)
	{
		maxErrors_ = maxErrors;
	}
	void EventConnection::setProcessEventCallback(const EvtProcessEventFn& fn)
	{
		boost::mutex::scoped_lock lockFn(processEventFnMtx_);
		processEventFn_ = fn;
	}
/*	void EventConnection::addProcessEventCallback(const EvtProcessEventFn& fn)
	{
		NET_ASSERT( !fn.empty() );
		if (fn.empty())
			return;
		processEventFns_.connect( fn );
	}
*/
	void EventConnection::setDisconnectCallback(const EvtDisconnectFn&)
	{
		NET_ASSERT( 0 && "NOT IMPLEMENTED" );
	}
	void EventConnection::setAllowedIncomingEventIds(const std::vector<NetEvent::id_type>& ids)
	{
		boost::mutex::scoped_lock lck(allowedEventIdsMtx_);
		allowedEventIds_ = ids;
	}
	void EventConnection::setPacketConnection(IPacketConnection* conn,const NetEvent::Direction dir)
	{
		boost::mutex::scoped_lock lck(startedMtx_);
		NET_ASSERT( !started_ );
		if (started_)
			return;
		if (conn_)
		{
			//@FIXME unsubscribe handlers!
		}
		conn_ = conn;
		dir_ = dir;
		started_ = false;
	}
	bool EventConnection::start()
	{
		boost::mutex::scoped_lock lck(startedMtx_);
		NET_ASSERT( !started_ );
		if (conn_)
		{
			conn_->addPacketReceivedCallback( boost::bind(&EventConnection::onReceivePacket,this,_1,_2,_3,_4) );
			started_ = true;
		}
		return started_;
	}
	void EventConnection::stop()
	{
		boost::mutex::scoped_lock lck(startedMtx_);
		NET_ASSERT( started_ );
		NET_ASSERT( conn_ );
		if (conn_)
		{
			//@todo fixme unsubscribe from packet connection 'conn_'
			conn_ = 0;
			started_ = false;
		}
	}
	void EventConnection::registerEvent(const NetEvent::id_type id, const NetEvent::Direction dir, const CreateEventFn& fnCreate, const DestroyEventFn& fnDestroy)
	{
		NET_ASSERT( !fnCreate.empty() );
		if (fnCreate.empty())
			return;
		NET_ASSERT( !fnDestroy.empty() );
		if (fnDestroy.empty())
			return;
		NET_ASSERT( !map_contains(eventIds_,id) );
		if (map_contains(eventIds_,id))
			return;
		IdEntry entry;
		entry.dir = dir;
		entry.fnCreate = fnCreate;
		entry.fnDestroy = fnDestroy;
		eventIds_.insert( std::make_pair(id,entry) );
	}
	void EventConnection::sendEvent(const PeerId peerId, const NetEvent& evt, const SendOptions& opt)
	{
		SendOptions options = opt;
		options.setPeerId(peerId);
		this->sendEvent( evt, options );
	}
	void EventConnection::sendEvent(const NetEvent& evt, const SendOptions& opt)
	{
		NET_ASSERT( conn_ );
		if (!conn_)
			return;

		bitstream_data data; // data container
		bitstream_sink dataSink(data); // sink adapter
		obitstream out(&dataSink); // stream interface

		out.write( evt.id(), sizeof(NetEvent::id_type)*8 ) ;
		if (!evt.pack( out ))
			return; //@FIXME: inform user

		out.flush();
		conn_->send( &data.front(), data.size(), opt );
	}
	void EventConnection::onReceivePacket(const PeerId peerId, const void* dataPtr, const size_t dataLen, const ChannelId channel)
	{
		NET_ASSERT( dataPtr && dataLen > 0 );
		NET_ASSERT( dataLen >= sizeof(event_id) );
		if (dataLen < sizeof(event_id))
		{
			++stats_.numInvalidPackets;
			return; //@FIXME increase error count !?
		}

		// copy data
		bitstream_data data;
		data.resize( dataLen );
		for (size_t i=0; i<dataLen; ++i)
			data[i] = ((const uint8*)(dataPtr))[i];

		// attach stream
		bitstream_source dataSource(data);
		ibitstream in(&dataSource);

		// extract event id
		event_id evtId = NetEvent::EVTID_NONE;
		in.read( evtId, sizeof(NetEvent::id_type)*8 );

		// check event id
		EventIdMap::const_iterator it = eventIds_.find( evtId );
		if (it == eventIds_.end())
		{
			++stats_.numUnregisteredEventIds;
			return;
		}
		if (evtId == NetEvent::EVTID_NONE)
		{
			++stats_.numInvalidEventIds;
			return;
		}
		{
			boost::mutex::scoped_lock lck(allowedEventIdsMtx_);
			if (!allowedEventIds_.empty())
			{
				if (!contains(allowedEventIds_,evtId))
				{
					++stats_.numBlockedEventIds;
					return;
				}
			}
		}
		// check event direction
		const IdEntry& idEntry = it->second;
		const event_direction evtDir = idEntry.dir;
		if (evtDir != NetEvent::DIR_ANY)
		{
			if (dir_ == evtDir)
			{
				++stats_.numWrongEventDirection;
				return;
			}
		}
		// create event
		const CreateEventFn& fnCreate = idEntry.fnCreate;
		NET_ASSERT( !fnCreate.empty() );
		const DestroyEventFn& fnDestroy = idEntry.fnDestroy;
		NET_ASSERT( !fnDestroy.empty() );

		NetEvent* evt = fnCreate(evtId);

		// deserialize
		if (!evt->unpack(in))
		{
			fnDestroy(evt);
			++stats_.numInvalidEvents;
			return;
		}

		// notify callbacks
		if (manualPolling_)
		{
			// queue
			boost::mutex::scoped_lock lck(evtQMtx_);
			evtQ_.push_back(EvtQItem(peerId,evt,channel,fnDestroy));
		}
		else
		{
			// process immediately
			++stats_.numEventsProcessed;
			{
				boost::mutex::scoped_lock lockFn(processEventFnMtx_);
				if (!processEventFn_.empty())
				{
					processEventFn_(peerId,*evt,channel);
				}
			}
			fnDestroy(evt);
		}
	}
	void EventConnection::setPolling(const bool manualPolling)
	{
		manualPolling_ = manualPolling;
	}
	void EventConnection::poll()
	{
		NET_ASSERT( manualPolling_ );
		EvtQ evtq;
		{
			boost::mutex::scoped_lock lck(evtQMtx_);
			evtq = evtQ_;
			evtQ_.clear();
		}
		boost::mutex::scoped_lock lockFn(processEventFnMtx_);
		for (EvtQ::const_iterator it = evtq.begin(); it != evtq.end(); ++it)
		{
			if (!processEventFn_.empty())
				processEventFn_( it->peerId_, *it->evt_, it->channel_ );
			it->fnDestroy_( it->evt_ );
		}
	}
} // namespace impl
} // namespace net
