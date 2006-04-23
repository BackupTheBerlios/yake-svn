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

		//NetEvent* clone();
		//{
		//	NetEvent* clone = doClone();
		//	NET_ASSERT( clone );
		//	if (clone)
		//	{
		//		clone->setId( id_ );
		//		return clone;
		//	}
		//	return 0;
		//}
	protected:
		//virtual NetEvent* doClone() = 0;
	private:
		id_type		id_;
	};

	/*
	template<typename T>
	class ResourceManager
	{
	public:
		struct handle
		{
			friend class ResourceManager;
			T* operator ->()
			{ return px_; }
			T* operator ->() const
			{ return px_; }
			bool empty() const
			{ return (px_==0); }
			void destroy()
			{
				owner_->destroy(this);
				owner_ = 0;
				px_ = 0;
			}
			handle& operator=(const handle& rhs)
			{
				owner_ = rhs.owner_;
				px_ = rhs.px_;
			}
		protected:
			~handle()
			{}
			handle(const handle& other) : creator_(other.creator_), px_(other.px_)
			{}
			handle(ResourceManager<T>* owner, T* px) : owner_(owner), px_(px)
			{}
		private:
			T*							px_;
			ResourceManager<T>*	owner_;
		};
		handle create()
		{
			return handle(this,onCreate());
		}
		void destroy(const handle& h)
		{
			onDestroy( h.px_ );

			ptr_list::iterator it = std::find( objs_.begin(), objs_.end() );
			if (it != objs_.end())
				objs_.erase( it );
		}
		void destroyAll()
		{
			ptr_list::iterator itEnd = objs_.end();
			for (ptr_list::iterator it = objs_.begin(); it != itEnd; ++it)
				onDestroy( *it );
			objs_.clear();
		}
	protected:
		virtual void onDestroy(T*) = 0;
		virtual T* onCreate() = 0;
	private:
		typedef std::deque<T*> ptr_list;
		ptr_list		objs_;
	};
	typedef ResourceManager<NetEvent> NetEventManager;
	typedef NetEventManager::handle NetEventHandle;
	*/
	class INetEventConnection;
	namespace impl { class EventConnection; }
	struct NET_API NetEventHandle
	{
		friend class INetEventConnection;
		friend class impl::EventConnection;
		NetEvent* operator ->() const;
		NetEvent& operator *() const;
		bool empty() const;
		void destroy();
		NetEventHandle& operator=(const NetEventHandle& rhs);
	protected:
		~NetEventHandle();
		NetEventHandle(const NetEventHandle& other);
		NetEventHandle(impl::EventConnection* owner, NetEvent* px);
	private:
		NetEvent*					px_;
		impl::EventConnection*	owner_;
	};

	class NET_API INetEventConnection
	{
	public:
		virtual ~INetEventConnection();

		virtual void setPacketConnection(IPacketConnection*,const NetEvent::Direction) = 0;
		virtual void setMaxEventErrors(const size_t) = 0;

		virtual bool start() = 0;
		virtual void stop() = 0;

		typedef boost::function<NetEvent*(const NetEvent::id_type)> CreateEventFn;
		typedef boost::function<void(NetEvent*)> DestroyEventFn;

		/** NB fnCreate and fnDestroy have to be threadsafe when manual polling is used! */
		virtual void registerEvent(const NetEvent::id_type, const NetEvent::Direction,
									const CreateEventFn& fnCreate, const DestroyEventFn& fnDestroy) = 0;
		virtual void sendEvent(const NetEvent&, const SendOptions& opt = SendOptions()) = 0;
		virtual void sendEvent(const PeerId, const NetEvent&, const SendOptions& opt = SendOptions()) = 0;
		virtual void setAllowedIncomingEventIds(const std::vector<NetEvent::id_type>&) = 0;

		virtual void setPolling(const bool manualPolling = false) = 0;
		virtual void poll() = 0;

		typedef boost::function<void(const PeerId, const NetEvent&, const ChannelId)> EvtProcessEventFn;
		typedef boost::function<void(void)> EvtDisconnectFn;

		/** Pass 0 to disable. */
		virtual void setProcessEventCallback(const EvtProcessEventFn&) = 0;
		virtual void setDisconnectCallback(const EvtDisconnectFn&) = 0;
	};
	NET_API INetEventConnection* createEventConnection();

} // namespace net

#endif
