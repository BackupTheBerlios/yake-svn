#ifndef YAKE_TAGGED_LISTENER_MANAGER_H
#define YAKE_TAGGED_LISTENER_MANAGER_H

namespace yake {

	template<typename T_listener>
	struct ListenerManager
	{
		virtual ~ListenerManager() {}
		typedef T_listener listener_type;
		enum AttachMode
		{
			kTransferOwnership = 0,
			kKeepOwnership = 1
		};
		void attachListener(listener_type*,const std::string& tag = "",const AttachMode mode = kTransferOwnership);

		//listener_type* detachListener(listener_type*);
		//listener_type* detachListener(const std::string& tag);

		//void destroyListener(listener_type*);
		//void destroyListener(const std::string& tag);

		listener_type* getListener(const std::string& tag) const;
	protected:
		typedef std::map<std::string,listener_type*> tag_listener_map;
		tag_listener_map	listenerMap_;

		typedef std::deque<std::pair<listener_type*,AttachMode> > listener_list;
		listener_list		listeners_;
	};
	/** @todo implement iterator and const_iterator.
	*/
	template<typename T_listener>
	void ListenerManager<T_listener>::attachListener(T_listener* listener,const std::string& tag,const AttachMode mode)
	{
		YAKE_ASSERT( listener );
		if (!listener)
			return;
		listeners_.push_back( listener_list::value_type(listener,mode) );
		if (!tag.empty())
		{
			tag_listener_map::const_iterator it = listenerMap_.find(tag);
			YAKE_ASSERT( it == listenerMap_.end() );
			if (it != listenerMap_.end())
				listenerMap_.insert( tag_listener_map::value_type( tag, listener ) );
		}
	}
	template<typename T_listener>
	typename ListenerManager<T_listener>::listener_type* ListenerManager<T_listener>::getListener(const std::string& tag) const
	{
			tag_listener_map::const_iterator it = listenerMap_.find(tag);
			YAKE_ASSERT( it != listenerMap_.end() );
			if (it == listenerMap_.end())
				return 0;
			return it->second;
	}

} // namespace yake

#endif
