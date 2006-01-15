#ifndef NET_EVENTCONNECTION_H
#define NET_EVENTCONNECTION_H

namespace net {
namespace impl {

	//--------------------------------------------------------------------------
	class EventConnection : public ::net::INetEventConnection
	{
	public:
		EventConnection();
		virtual ~EventConnection();

		// INetEventConnection interface
		virtual void setPacketConnection(IPacketConnection*,const NetEvent::Direction);
		virtual void setMaxEventErrors(const size_t);

		virtual void registerEvent(const NetEvent::id_type, const NetEvent::Direction,
									const CreateEventFn& fnCreate, const DestroyEventFn& fnDestroy);
		virtual void sendEvent(const NetEvent&, const SendOptions& opt = SendOptions());
		virtual void sendEvent(const PeerId, const NetEvent&, const SendOptions& opt = SendOptions());
		virtual void setAllowedIncomingEventIds(const std::vector<NetEvent::id_type>&);

		virtual void setProcessEventCallback(const EvtProcessEventFn&);
		//virtual void removeProcessEventCallback(const EvtProcessEventFn&);
		virtual void setDisconnectCallback(const EvtDisconnectFn&);
	private:
		void onReceivePacket(const PeerId, const void*, const size_t, const ChannelId);
	private:
		//typedef std::deque<EvtProcessEventFn> EvtProcessEventFnList;
		//EvtProcessEventFnList	processEventFns_;
		EvtProcessEventFn processEventFn_;
		void fireCallbacks_ProcessEvent(const PeerId peerId, const NetEvent& evt, const ChannelId channel)
		{
			if (!processEventFn_.empty())
				processEventFn_(peerId,evt,channel);

			//EvtProcessEventFnList fns = processEventFns_; // Use a copy as the original container may be modified during the callbacks!
			//for (EvtProcessEventFnList::const_iterator it = fns.begin(); it != fns.end(); ++it)
			//	(*it)(peerId,evt,channel);
		}

	private:
		typedef NetEvent::id_type event_id;
		typedef NetEvent::Direction event_direction;

		struct IdEntry
		{
			event_direction		dir;
			CreateEventFn		fnCreate;
			DestroyEventFn		fnDestroy;
		};
		typedef std::map<event_id,IdEntry> EventIdMap;
		EventIdMap				eventIds_;

		typedef std::vector<NetEvent::id_type> EventIdVector;
		EventIdVector			allowedEventIds_;
		mutable boost::mutex	allowedEventIdsMtx_;

		event_direction			dir_;
		IPacketConnection*		conn_;

		struct statistics_t
		{
			statistics_t() : 
				numInvalidPackets(0),
				numInvalidEvents(0),
				numInvalidEventIds(0),
				numUnregisteredEventIds(0),
				numWrongEventDirection(0),
				numEventsProcessed(0),
				numBlockedEventIds(0)
			{}
			size_t		numInvalidPackets;
			size_t		numInvalidEvents;
			size_t		numInvalidEventIds;
			size_t		numUnregisteredEventIds;
			size_t		numWrongEventDirection;
			size_t		numEventsProcessed;
			size_t		numBlockedEventIds;
			size_t totalErrors() const
			{
				return numInvalidPackets +
						numInvalidEvents +
						numInvalidEventIds +
						numUnregisteredEventIds +
						numWrongEventDirection;
			}
		} stats_;
		size_t					maxErrors_;
	};

} // namespace impl
} // namespace net

#endif
