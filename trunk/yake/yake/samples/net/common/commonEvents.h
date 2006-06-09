#ifndef TEST_COMMONEVENTS_H
#define TEST_COMMONEVENTS_H

#include <map>

const net::ChannelId CHANNELID_CONTROL = 0;
const net::ChannelId CHANNELID_SIMCTRL = 1;
const net::ChannelId CHANNELID_SIMUPD  = 2;

//---
/*
// pull in types
typedef net::INetEventConnection::CreateEventFn CreateEventFn;
typedef net::INetEventConnection::DestroyEventFn DestroyEventFn;

// event pool
struct EventPool
{
	EventPool();
	~EventPool();
	void register_class(const net::NetEvent::id_type id, const CreateEventFn&, const DestroyEventFn&);
	{
		classes_.insert( std::make_pair( id, ClassEntry(createFn,destroyFn) ) );
	}
	net::NetEvent* create(const net::NetEvent::id_type);
	void destroy(net::NetEvent*);
	void lookup(const net::NetEvent::id_type id, CreateEventFn&, DestroyEventFn&);
private:
	typedef std::deque<net::NetEvent*> EventList;
	struct ClassEntry
	{
		CreateEventFn	createFn_;
		DestroyEventFn	destroyFn_;
		EventList		avail_;
		EventList		taken_;
	};
	typedef std::map<net::NetEvent::id_type,ClassEntry> ClassMap; //<= perfect candidate for AssocVector
	ClassMap		classes_;
};

// event registrator for use with pooled events and event connections
template<typename evt_type>
bool s_registerEvent(const EventPool& evtpool,
							const net::IEventConnection& conn,
							const net::NetEvent::id_type id,
							const net::NetEvent::Direction dir)
{
	CreateEventFn createFn = boost::bind(&EventPool::create,&evtpool,_1);
	DestroyEventFn destroyFn = boost::bind(&EventPool::destroy,&evtpool,_1);;
	conn.registerEvent(id,dir,ce.createFn,ce.destroyFn);
}
*/
//---

#define DECLARE_EVENT(CLASS,IDVALUE) \
public: \
static net::NetEvent* create(net::NetEvent::id_type id) \
{ CLASS* p = new CLASS(); p->setId(id); return p; } \
static void destroy(net::NetEvent* p) { delete p; } \
static const net::NetEvent::id_type ID = IDVALUE; \
CLASS() : net::NetEvent(ID) \
{}

struct version_t
{
	net::uint8 ver[3];
	version_t(const net::uint8 maj = 0, const net::uint8 min = 0, const net::uint8 b = 0)
	{
		ver[0] = maj;
		ver[1] = min;
		ver[2] = b;
	}
};
inline net::obitstream& operator << (net::obitstream& out, const version_t& rhs)
{
	out.write( rhs.ver[0], 8 );
	out.write( rhs.ver[1], 8  );
	out.write( rhs.ver[2], 8  );
	return out;
}
inline net::ibitstream& operator >> (net::ibitstream& in, version_t& rhs)
{
	in.read( rhs.ver[0], 8 );
	in.read( rhs.ver[1], 8 );
	in.read( rhs.ver[2], 8 );
	return in;
}

struct c2sEvtJoinReq : public net::NetEvent
{
	DECLARE_EVENT( c2sEvtJoinReq, 1 );
	virtual bool pack(net::obitstream& out) const
	{
		out << version;;
		return true;
	}
	virtual bool unpack(net::ibitstream& in)
	{
		in >> version;
		return true;
	}
	version_t	version;
};
struct s2cEvtJoinReqReply : public net::NetEvent
{
	DECLARE_EVENT( s2cEvtJoinReqReply, 2 );
	virtual bool pack(net::obitstream& out) const
	{
		out.write( accepted );
		return true;
	}
	virtual bool unpack(net::ibitstream& in)
	{
		in.read( accepted );
		return true;
	}
	bool accepted;
};
struct s2cEvtClassTable : public net::NetEvent
{
	DECLARE_EVENT( s2cEvtClassTable, 10 );
	virtual bool pack(net::obitstream& out) const
	{
		//out << globalClassIds_;
		return true;
	}
	virtual bool unpack(net::ibitstream& in)
	{
		//in >> globalClassIds_;
		return true;
	}
	//uint8 numClassIdBits_;
	std::map<std::string,net::uint16> globalClassIds_;
};
struct c2sEvtSimOk : public net::NetEvent
{
	DECLARE_EVENT( c2sEvtSimOk, 11 );
	virtual bool pack(net::obitstream& out) const
	{
		return true;
	}
	virtual bool unpack(net::ibitstream& in)
	{
		return true;
	}
};


#endif