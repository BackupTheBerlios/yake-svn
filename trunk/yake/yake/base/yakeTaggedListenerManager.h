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
		assert( listener );
		listeners_.push_back( listener_list::value_type(listener,mode) );
		if (!tag.empty()) //@todo add check for existing tag!
			listenerMap_[ tag ] = listener;
	}

} // namespace yake

#endif
