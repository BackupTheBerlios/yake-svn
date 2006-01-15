#ifndef NET_EVENT_H
#define NET_EVENT_H

#include "netBitstream.h"
#include "netBitstreamAdapters.h"

namespace net {

	typedef obitstream_vector obitstream;
	typedef ibitstream_vector ibitstream;

	typedef bitstream_stlvector_sink::container_type bitstream_data;
	typedef bitstream_stlvector_sink bitstream_sink;
	typedef bitstream_stlvector_source bitstream_source;

	class NET_API NetEvent
	{
	public:
		virtual ~NetEvent() {}

		typedef uint8 id_type;
		static const id_type EVTID_NONE;
		enum Direction {
			DIR_ANY,
			DIR_CLIENT_TO_SERVER,
			DIR_SERVER_TO_CLIENT
		};
	private:
		NetEvent();
		NetEvent(const NetEvent&);
		NetEvent& operator=(const NetEvent&);
	protected:
		NetEvent(const id_type);
	public:

		id_type id() const;
		void setId(const id_type id); //@really needed?

		virtual bool pack(obitstream&) const = 0;
		virtual bool unpack(ibitstream&) = 0;
	private:
		id_type		id_;
	};

	class NET_API INetEventConnection
	{
	public:
		virtual ~INetEventConnection();

		virtual void setPacketConnection(IPacketConnection*,const NetEvent::Direction) = 0;
		virtual void setMaxEventErrors(const size_t) = 0;

		typedef boost::function<NetEvent*(const NetEvent::id_type)> CreateEventFn;
		typedef boost::function<void(NetEvent*)> DestroyEventFn;

		virtual void registerEvent(const NetEvent::id_type, const NetEvent::Direction,
									const CreateEventFn& fnCreate, const DestroyEventFn& fnDestroy) = 0;
		virtual void sendEvent(const NetEvent&, const SendOptions& opt = SendOptions()) = 0;
		virtual void sendEvent(const PeerId, const NetEvent&, const SendOptions& opt = SendOptions()) = 0;
		virtual void setAllowedIncomingEventIds(const std::vector<NetEvent::id_type>&) = 0;

		typedef boost::function<void(const PeerId, const NetEvent&, const ChannelId)> EvtProcessEventFn;
		typedef boost::function<void(void)> EvtDisconnectFn;

		virtual void setProcessEventCallback(const EvtProcessEventFn&) = 0;
		virtual void setDisconnectCallback(const EvtDisconnectFn&) = 0;
	};
	NET_API INetEventConnection* createEventConnection();

} // namespace net

#endif