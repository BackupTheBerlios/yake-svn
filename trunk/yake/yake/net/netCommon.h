#ifndef NET_COMMON_H
#define NET_COMMON_H

namespace net {

	typedef uint8 ChannelId;
	typedef uint32 PeerId;

	enum Reliability
	{
		R_RELIABLE = 0,
		R_UNRELIABLE
	};
	enum Ordering
	{
		O_ORDERED = 0,
		O_UNORDERED
	};

	struct NET_API SendOptions
	{
		SendOptions();
		SendOptions(const SendOptions&);
		~SendOptions();
		SendOptions& operator=(const SendOptions&);

		SendOptions& setPeerId(const PeerId);
		SendOptions& setReliability(const Reliability);
		SendOptions& setOrdering(const Ordering);
		SendOptions& setChannel(const ChannelId);

		PeerId			peerId;
		Reliability		reliability;
		Ordering		ordering;
		ChannelId		channelId;
	};

	struct NET_API Address
	{
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

	NET_API std::string ipToString(const uint32);

	namespace native {
		NET_API void sleep(const uint32 ms);
	} // namespace native

	struct NET_API Exception : public std::exception
	{
		Exception(const std::string& msg, const char* file = 0, const int line = 0);
		virtual ~Exception() throw() {}
		virtual const char* what() const throw();
	private:
		std::string	msg_;
	};

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
