#ifndef NET_COMMON_H
#define NET_COMMON_H

#include <yake/net/netPrerequisites.h>

namespace net {

	/** Used to define packet transport channels. */
	typedef uint8 ChannelId;

	/** Used to identify network peers. */
	typedef uint32 PeerId;
	const PeerId PEERID_BROADCAST = 0xffffffff;

	/** Specifies the reliability method a packet will be transferred with. */
	enum Reliability
	{
		R_RELIABLE = 0,
		R_UNRELIABLE
	};
	/** Specifies the ordering method a packet will be transferred with. */
	enum Ordering
	{
		O_ORDERED = 0,
		O_UNORDERED
	};

	/** Represents a specifically configured set of options.
		It used for specifying additional information and hints
		when sending data/packets.
		Usage example: conn.send(buf.data(),buf.size(), SendOptions().setOrdering(O_ORDERED).setReliability(R_RELIABLED));
	*/
	struct NET_API SendOptions
	{
		SendOptions();
		SendOptions(const SendOptions&);
		~SendOptions();
		SendOptions& operator=(const SendOptions&);

		/** Set the target peer id.
			@Remarks This setting is ignored by broadcast functions!
		*/
		SendOptions& peerId(const PeerId);
		SendOptions& reliability(const Reliability);
		SendOptions& ordering(const Ordering);
		SendOptions& channel(const ChannelId);

		PeerId getPeerId() const;
		Reliability getReliability() const;
		Ordering getOrdering() const;
		ChannelId getChannel() const;
	private:
		PeerId			peerId_;
		Reliability		reliability_;
		Ordering		ordering_;
		ChannelId		channelId_;
	};

	/** Represents a network address (IP + port pair).
	*/
	struct NET_API Address
	{
		/** Initializes the IP with "127.0.0.1". */
		Address(const uint16 port);
		Address(const std::string& ip = std::string("127.0.0.1"), const uint16 port = 0);
		Address(const Address&);
		~Address();
		Address& operator=(const Address&);

		void setIp(const std::string& ip);
		const std::string& ip() const;
		void setPort(const uint16 port);
		const uint16 port() const;
	private:
		std::string		ip_;
		uint16			port_;
	};

	/** Converts the 32-bit integer representation of an IPv4 IP
		to a string representation.
	*/
	NET_API std::string ipToString(const uint32);

	namespace native {
		/** Sleeps the active thread for a certain number of milliseconds.
			This also yields the thread! (For example, a Win32 implementation
			may use ::Sleep().)
		*/
		NET_API void sleep(const uint32 ms);
	} // namespace native

	/** Represents an exception in the network component. */
	struct NET_API Exception : public std::exception
	{
		Exception(const std::string& msg, const char* file = 0, const int line = 0);
		virtual ~Exception() throw() {}
		virtual const char* what() const throw();
	private:
		std::string	msg_;
	};

	/** Cross-platform timer interface.
	*/
	class NET_API Timer
	{
	public:
		Timer();
		~Timer();
		void start();
		void stop();
		void pause();
		void resume();
		void reset();
		double getTime() const;
	private:
		double					start_;
		mutable double			time_;
		bool					paused_;
	};

} // namespace net

#endif
