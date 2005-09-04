#include "bind_network/bind_network.h"

unsigned int getPropertyPosition(const Class & class_, const Property & prop_)
{ 
	unsigned int pos = 0;
	for( Class::PropertyList::const_iterator iter = class_.getProperties().begin();
		iter != class_.getProperties().end();	iter++ )
	{	if( iter->getName() == prop_.getName() ) return pos; }
	throw std::exception();
}

unsigned int getPropertyPosition(const Class & class_, const char * prop_)
{	
	return getPropertyPosition(class_, class_.getProperty(prop_)); 
}

Property & getPropertyAt(const Class & class_, const int pos_)
{
	Class::PropertyList::const_iterator iter = class_.getProperties().begin();
	std::advance(iter, pos_);
	return const_cast<Property&>(*iter);
}

// send property update to server
TNL_IMPLEMENT_NETOBJECT_RPC(Replicatable, c2sPropertyChanged, (TNL::ByteBufferRef value_, TNL::Int<32> position_), 
   TNL::NetClassGroupGameMask, TNL::RPCGuaranteedOrdered, TNL::RPCToGhostParent, 0)
{
	Property & prop = getPropertyAt(getClass(), position_);
	if(prop.getTypeInfo() == typeid(int))
	{
		assert(sizeof(int) == value_.getBufferSize());
		prop.set(this, *reinterpret_cast<const int*>(value_.getBuffer()));
		std::cout << "c2sPropertyChanged( <int>" << *reinterpret_cast<const int*>(value_.getBuffer()) << ")" << std::endl;
	}
}