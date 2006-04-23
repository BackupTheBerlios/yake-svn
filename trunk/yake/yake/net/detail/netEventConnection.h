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

		virtual bool start();
		virtual void stop();

		virtual void registerEvent(const NetEvent::id_type, const NetEvent::Direction,
									const CreateEventFn& fnCreate, const DestroyEventFn& fnDestroy);
		virtual void sendEvent(const NetEvent&, const SendOptions& opt = SendOptions());
		virtual void sendEvent(const PeerId, const NetEvent&, const SendOptions& opt = SendOptions());
		virtual void setAllowedIncomingEventIds(const std::vector<NetEvent::id_type>&);

		virtual void setProcessEventCallback(const EvtProcessEventFn&);
		virtual void setDisconnectCallback(const EvtDisconnectFn&);

		virtual void setPolling(const bool manualPolling = false);
		virtual void poll();

		void onReceivePacket(const PeerId, const void*, const size_t, const ChannelId);
	private:
		mutable EvtProcessEventFn	processEventFn_;
		boost::mutex					processEventFnMtx_;

		typedef NetEvent::id_type event_id;
		typedef NetEvent::Direction event_direction;

		struct IdEntry
		{
			event_direction		dir;
			CreateEventFn			fnCreate;
			DestroyEventFn			fnDestroy;
		};
		typedef std::map<event_id,IdEntry> EventIdMap;
		EventIdMap					eventIds_;

		typedef std::vector<NetEvent::id_type> EventIdVector;
		EventIdVector				allowedEventIds_;
		mutable boost::mutex		allowedEventIdsMtx_;

		IPacketConnection*		conn_;
		event_direction			dir_;

		struct EvtQItem
		{
			PeerId					peerId_;
			NetEvent*				evt_;
			ChannelId				channel_;
			DestroyEventFn			fnDestroy_;
			EvtQItem(const PeerId peerId, NetEvent* evt, const ChannelId channel, const DestroyEventFn fnDestroy) :
				peerId_(peerId), evt_(evt), channel_(channel), fnDestroy_(fnDestroy)
			{}
			EvtQItem& operator = (const EvtQItem& rhs)
			{
				peerId_ = rhs.peerId_;
				channel_= rhs.channel_;
				evt_    = rhs.evt_;
				fnDestroy_ = rhs.fnDestroy_;
				return *this;
			}
		};
		typedef std::deque<EvtQItem> EvtQ;
		mutable EvtQ				evtQ_;
		boost::mutex				evtQMtx_;
		bool							manualPolling_;

		mutable bool				started_;
		boost::mutex				startedMtx_;

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
