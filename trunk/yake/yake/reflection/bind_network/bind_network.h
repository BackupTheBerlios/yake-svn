#ifndef _BIND_NETWORK_H_
#define _BIND_NETWORK_H_

#include <algorithm>
#include <vector>

#include "tnl.h"
#include "tnlNetObject.h"
#include "tnlGhostConnection.h"
#include "tnlNetInterface.h"
namespace yake { namespace net = TNL; }

#include "reflection.h"
using namespace reflection;

// todo use network wrapper, add yake rpc
// todo net_parm for extrapolation etc.

// -----------------------------------------
// class macro
#define DEFINE_NET_CLASS(CLASS_NAME, SUPER_CLASS_NAME)\
public:\
	virtual const Class & getRxClass() const { return getClass(); }\
	TNL_DECLARE_CLASS(CLASS_NAME);

#define IMPLEMENT_NET_CLASS(NAME) TNL_IMPLEMENT_NETOBJECT(NAME);


// -----------------------------------------
// property macros
#define net_parms(P1, P2) ,P1 ,P2

#define DEFAULT_PROPERTY(TYPE, NAME, BINDING_OR_OBSERVER1, PARMS1, BINDING_OR_OBSERVER2) \
private:\
    FIELD(private, TYPE, m_##NAME);\
    TYPE get_##NAME() const {\
        return m_##NAME;\
    }\
    void set_##NAME(TYPE value) {\
        m_##NAME = value;\
    }\
/* bindings */\
BINDING_OR_OBSERVER1##_BINDING_PROPERTY_DEFAULT(TYPE, NAME)\
BINDING_OR_OBSERVER2##_BINDING_PROPERTY_DEFAULT(TYPE, NAME)\
/* observers */\
RX_PROPERTY(TYPE, NAME, BINDING_OR_OBSERVER1##_OBSERVER_PROPERTY_DEFAULT(TYPE, NAME, PARMS1), BINDING_OR_OBSERVER2##_OBSERVER_PROPERTY_DEFAULT(TYPE, NAME))

// bindings switch/spezialiation: we want bindings for lua
#define _BINDING_PROPERTY_DEFAULT(TYPE, NAME) // nothing
#define lua_BINDING_PROPERTY_DEFAULT(TYPE, NAME) LUA_DEFAULT_PROPERTY(TYPE, NAME) // bind this class to lua
#define net_BINDING_PROPERTY_DEFAULT(TYPE, NAME) // nothing

// observers switch/spezialiation: we want an observer for net
#define _OBSERVER_PROPERTY_DEFAULT(TYPE, NAME) // nothing
#define lua_OBSERVER_PROPERTY_DEFAULT(TYPE, NAME) // nothing
#define net_OBSERVER_PROPERTY_DEFAULT(TYPE, NAME, PARMS1) \
	/* add a new network observer to the property list of the parent c++ object */ \
	static unsigned int property_position = getClassStatic().getProperties().size(); \
	owner()->__observers__##NAME.push_back(new network_observer<TYPE, ClassType>(property_position, owner() PARMS1));


// -----------------------------------------
// observers which gets called from the __property__ template class within the c++ class
// or from the __observer_property__ handler when the static class reflection and an object
// pointer is used
template<typename VALUE_TYPE>
struct property_observer
{	
	virtual void set(VALUE_TYPE value) = 0;
};

// todo: clean this up somehow, functors instead of notify_property_changed?
// user implementations for the network observer?
template<typename VALUE_TYPE, class CLASS_TYPE>
struct network_observer : property_observer<VALUE_TYPE>
{
	network_observer(unsigned int Position, const CLASS_TYPE * ThisClass, int para1 = 0, int para2 = 0) 
		: mPosition(Position), mThisClass(const_cast<CLASS_TYPE*>(ThisClass)) 
	{}

	virtual void set(VALUE_TYPE value)
	{ 
		mThisClass->notify_property_changed(mPosition, value);
	}

	unsigned int mPosition;
	CLASS_TYPE * mThisClass;
};

// -----------------------------------------
// specialised property handler class which notifies the according observers
// this hooks into the reflected properties (access to the field by means of reflection::property)
#define __OBSERVER_PROPERTY__(PROPERTY_NAME) \
template <class C, class VALUE_TYPE> struct __observer_property__##PROPERTY_NAME : public __property__<C, VALUE_TYPE> \
{ \
		/* types */ \
		typedef __property__<C, VALUE_TYPE>  base; \
		typedef std::vector< property_observer<VALUE_TYPE>* > ObserverList; \
		/* default constructor */ \
    __observer_property__##PROPERTY_NAME(Getter g, Setter s) : base(g, s) {} \
    /* set */ \
    virtual void set(void * object, VALUE_TYPE value) const \
		{ \
      base::set(object, value); \
			for(ObserverList::const_iterator iter(reinterpret_cast<C*>(object)->__observers__##PROPERTY_NAME.begin()); \
				iter != reinterpret_cast<C*>(object)->__observers__##PROPERTY_NAME.end(); iter++) \
			{ (*iter)->set(value); } \
    } \
};

// -----------------------------------------
// the actual property
#define RX_PROPERTY(TYPE, NAME, OBSERVER1, OBSERVER2)\
private:\
		/* observers for this property */ \
		typedef std::vector<property_observer<TYPE> *> ObserverList##NAME;\
		ObserverList##NAME __observers__##NAME;\
		\
		/* specialized property handler which notifies the observers */ \
		__OBSERVER_PROPERTY__(NAME)\
		friend __observer_property__##NAME<ClassType, TYPE>;\
		\
	  template <class T> class __property__##NAME {\
      public:\
        typedef __property__##NAME<T> Type;\
        __property__##NAME<T>()\
				{\
						/* create observers */ \
						OBSERVER1\
						OBSERVER2\
						/* register this property to the static reflected class using the optimal method */ \
						if(owner()->__observers__##NAME.empty())\
							static reflection::__register_property__ reg(new __property__<ClassType, T>(&get_##NAME, &set_##NAME), getClassStaticPtr(), typeid(TYPE), #TYPE, #NAME);\
						else\
							static reflection::__register_property__ reg(new __observer_property__##NAME<ClassType, T>(&get_##NAME, &set_##NAME), getClassStaticPtr(), typeid(TYPE), #TYPE, #NAME);\
        }\
        inline TYPE get() const {\
            return owner()->get_##NAME();\
        }\
        inline void set(TYPE value)\
				{\
				    /* notify observers, this calls the observer, if the user modifies the property directly */ \
						for(ObserverList##NAME::iterator iter = owner()->__observers__##NAME.begin();\
								iter != owner()->__observers__##NAME.end(); iter++)\
						{ (*iter)->set(value); }\
            owner()->set_##NAME(value);\
        }\
        inline operator TYPE () const {\
            return get();\
        }\
        inline bool operator == (TYPE value) const {\
            return get() == value;\
        }\
        inline bool operator != (TYPE value) const {\
            return get() != value;\
        }\
        inline bool operator < (TYPE value) const {\
            return get() < value;\
        }\
        inline bool operator > (TYPE value) const {\
            return get() > value;\
        }\
        inline bool operator <= (TYPE value) const {\
            return get() <= value;\
        }\
        inline bool operator >= (TYPE value) const {\
            return get() >= value;\
        }\
        inline Type &operator = (TYPE value) {\
            set(value);\
            return *this;\
        }\
        inline Type &operator = (const Type &prop) {\
            if (&prop != this) set(prop.get());\
            return *this;\
        }\
        inline Type &operator += (TYPE value) {\
            set(get() + value);\
            return *this;\
        }\
        inline Type &operator -= (TYPE value) {\
            set(get() - value);\
            return *this;\
        }\
        inline Type &operator *= (TYPE value) {\
            set(get() * value);\
            return *this;\
        }\
        inline Type &operator /= (TYPE value) {\
            set(get() / value);\
            return *this;\
        }\
        inline Type &operator %= (TYPE value) {\
            set(get() % value);\
            return *this;\
        }\
        inline Type &operator <<= (int bits) {\
            set(get() << bits);\
            return *this;\
        }\
        inline Type &operator >>= (int bits) {\
            set(get() >> bits);\
            return *this;\
        }\
        inline Type &operator &= (TYPE value) {\
            set(get() & value);\
            return *this;\
        }\
        inline Type &operator |= (TYPE value) {\
            set(get() | value);\
            return *this;\
        }\
        inline Type &operator ^= (TYPE value) {\
            set(get() ^ value);\
            return *this;\
        }\
        inline TYPE operator ~() {\
            return ~get();\
        }\
        inline TYPE operator ++() {\
            TYPE val = get();\
            set(val + 1);\
            return val;\
        }\
        inline TYPE operator ++(int i) {\
            TYPE val = get();\
            set(val + 1);\
            return get();\
        }\
        inline TYPE operator --() {\
            TYPE val = get();\
            set(val - 1);\
            return val;\
        }\
        inline TYPE operator --(int i) {\
            TYPE val = get();\
            set(val - 1);\
            return get();\
        }\
      private:\
        ClassType *owner() {\
            return (ClassType *)(((const char *)this) - __OFFSET__(ClassType, NAME));\
        }\
        const ClassType *owner() const {\
            return (const ClassType *)(((const char *)this) - __OFFSET__(ClassType, NAME));\
        }\
    };\
		friend class __property__##NAME< TYPE >;\
public:\
    __property__##NAME< TYPE > NAME


// -----------------------------------------
// property helpers
unsigned int getPropertyPosition(const Class & class_, const Property & prop_);

unsigned int getPropertyPosition(const Class & class_, const char * prop_);

Property & getPropertyAt(const Class & class_, const int pos_);

template< typename Value_, class Object_ >
Value_ getPropertyValue(const Object_ obj_, const Property & prop_)
{
	Value_ value;
	prop_.get(value, obj_);
	return value;
}

template< typename Value_, class Object_ >
Value_ getPropertyValueAt(const Object_ obj_, const int pos_)
{
	Value_ value;
	getPropertyAt(obj_->getClass(), pos_).get(value, obj_);
	return value;
}

// -----------------------------------------
// net helpers
#define size_in_bits(T) ( sizeof(T) * 8 ) // returns size of type T in bits

// -----------------------------------------
// net base class
class Replicatable : public TNL::NetObject
{
public: // types
  enum MaskBits 
  {
     InitialMask  = (1 << 0),
     PropertyMask = (1 << 1)
  };

public: // constructor
	Replicatable() : m_IsClientScopeObject(false)
	{	mNetFlags.set(Ghostable);	}

public: // rx methods
	virtual const Class & getRxClass() const = 0;
	const Class & getClass() const { return getRxClass(); }

	typedef std::list< std::pair<unsigned int, int> > DirtyInts;
	DirtyInts m_DirtyInts;

	// todo bytebuffer
	template<typename VALUE_TYPE>
	void notify_property_changed(unsigned int pos_, VALUE_TYPE value_) 
	{
		// is this the scope object on the client or an object on the server?
		if(isClientScopeObject()) 
		{ // client sends update to server
			TNL::ByteBuffer tnl_buf(reinterpret_cast<TNL::U8*>(&value_), sizeof(VALUE_TYPE));
			c2sPropertyChanged(tnl_buf, pos_);
			std::cout << "set(" << value_ << ")" << std::endl;
		}
		else if(isServerScopeObject())
		{ // server sends update to all clients
			setMaskBits(PropertyMask);
			m_DirtyInts.push_back(std::make_pair(pos_, value_));
		}
	}

public: // network methods
	bool isClientScopeObject() const { return m_IsClientScopeObject; }
	bool isServerScopeObject() const { return !isGhost(); }
	bool isClientGhostObject() const { return isGhost() && !isClientScopeObject(); }

	TNL_DECLARE_RPC(c2sPropertyChanged, (TNL::ByteBufferRef value_, TNL::Int<32> position_));

	void clear()
	{
		m_DirtyInts.clear();
	}

	// server side
	TNL::U32 packUpdate(TNL::GhostConnection * connection_, TNL::U32 updateMask_, TNL::BitStream * stream_)
	{
		std::cout << "packUpdate()\n";
		// scope object on server for this client?
    if(stream_->writeFlag(updateMask_ & InitialMask)) 
			stream_->writeFlag(connection_->getScopeObject() == this);
		// dirty properties?
    if(stream_->writeFlag(updateMask_ & PropertyMask))
		{
			// count
			stream_->write(static_cast<unsigned int>(m_DirtyInts.size()));
			// debug
			std::cout << " dirty properties = " << static_cast<unsigned int>(m_DirtyInts.size()) << std::endl;
			// values and positions
			for(DirtyInts::iterator iter(m_DirtyInts.begin()); iter != m_DirtyInts.end(); iter++)
			{
				stream_->write(iter->first);
				stream_->write(iter->second);
			}
		}
    return 0;
	}

	// client side
	void unpackUpdate(TNL::GhostConnection * connection_, TNL::BitStream * stream_)
	{
		std::cout << "unpackUpdate()\n";
		// scope object on server for this client?
		if(stream_->readFlag() && stream_->readFlag()) 
			m_IsClientScopeObject = true; 
		// dirty properties?
		if(stream_->readFlag())
		{	// client scope object does not need updates
			if(isClientScopeObject())
			{ // correct client scope object values here
				unsigned int count = stream_->readInt(size_in_bits(unsigned int));
				// debug
				std::cout << " client scope object dirty properties = " << count << std::endl;
				for(unsigned int i = 0; i < count; i++ )
				{
					stream_->readInt(size_in_bits(unsigned int)); 
					stream_->readInt(size_in_bits(int));
				}
			}
			else if(isClientGhostObject())
			{ // set the values for ghosts on other clients
				unsigned int count = stream_->readInt(size_in_bits(unsigned int));
				// debug
				std::cout << " ghost dirty properties = " << count << std::endl;
				for(unsigned int i = 0; i < count; i++ )
				{
					unsigned int position = stream_->readInt(size_in_bits(unsigned int)); 
					int value = stream_->readInt(size_in_bits(int));
					getPropertyAt(getClass(), position).set(this, value);
					// debug
					std::cout << " new value = " << getPropertyValueAt<int>(this, position) << std::endl;
				}
			}
		}
	}

private: // data
		bool m_IsClientScopeObject;
};

#endif // _BIND_NETWORK_H_