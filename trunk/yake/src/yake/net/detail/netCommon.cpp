#include <yake/net/pch.h>
#include <sstream>
#include <yake/net/net.h>

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

namespace net {

	//-------------------------------------------------------------------------
	// CallbackConnection
	//-------------------------------------------------------------------------

	CallbackConnection::CallbackConnection(const CallbackHandle h, const DisconnectFn& fn) :
		handle_(h), disconnectFn_(fn)
	{
	}
		CallbackConnection::CallbackConnection(const CallbackConnection& other) :
		handle_(other.handle_), disconnectFn_(other.disconnectFn_)
	{
	}
	CallbackConnection& CallbackConnection::operator=(const CallbackConnection& rhs)
	{
		if (this == &rhs)
			return *this;
		handle_ = rhs.handle_;
		disconnectFn_ = rhs.disconnectFn_;
		return *this;
	}
	CallbackConnection::~CallbackConnection() {}; // does nothing!
	void CallbackConnection::disconnect()
	{
		if (handle_ != 0 && !disconnectFn_.empty())
			disconnectFn_(handle_);
		handle_ = 0;
	}

	//-------------------------------------------------------------------------
	// sleep(...)
	//-------------------------------------------------------------------------

	namespace native {
		void sleep(const uint32 ms)
		{
			::Sleep(ms);
		}
	} // namespace native

	//-------------------------------------------------------------------------
	// ipToString()
	//-------------------------------------------------------------------------

	std::string ipToString(const uint32 ip)
	{
		std::stringstream ips;
		ips << "" /*is necessary to avoid ambigious overload problem*/ << int(ip&0xff) << "." << int((ip>>8)&0xff) << "." << int((ip>>16)&0xff) << "." << int((ip>>24)&0xff);
		return ips.str();
	}

	//-------------------------------------------------------------------------
	// Address
	//-------------------------------------------------------------------------

	Address::Address(const uint16 port) : ip_("127.0.0.1"), port_(port)
	{}
	Address::Address(const std::string& ip, const uint16 port) : ip_(ip), port_(port)
	{}
	Address::Address(const Address& other) : ip_(other.ip_), port_(other.port_)
	{}
	Address::~Address()
	{}
	Address& Address::operator =(const net::Address& rhs)
	{
		if (this == &rhs)
			return *this;
		ip_ = rhs.ip_;
		port_ = rhs.port_;
		return *this;
	}
	void Address::setIp(const std::string& ip)
	{
		ip_ = ip;
	}
	const std::string& Address::ip() const
	{
		return ip_;
	}
	void Address::setPort(const uint16 port)
	{
		port_ = port;
	}
	const uint16 Address::port() const
	{
		return port_;
	}

	//-------------------------------------------------------------------------
	// Exception
	//-------------------------------------------------------------------------
	Exception::Exception(const std::string& msg, const char* file, const int line)
	{
		std::stringstream ss;
		ss << "net exception: '" << msg << "'\n";
		if (file)
			ss << "location: " << file << ":" << line << "\n";
		msg_ = ss.str();
	}
	const char* Exception::what() const
	{
		return msg_.c_str();
	}

	//-------------------------------------------------------------------------
	// SendOptions
	//-------------------------------------------------------------------------
	SendOptions::SendOptions() : 
		peerId(0xffffffff),
		reliability(R_RELIABLE),
		ordering(O_ORDERED),
		channelId(0)
	{}
	SendOptions::SendOptions(const SendOptions& other) :
		peerId(other.peerId),
		reliability(other.reliability),
		ordering(other.ordering),
		channelId(other.channelId)
	{}
	SendOptions::~SendOptions()
	{}
	SendOptions& SendOptions::operator=(const SendOptions& rhs)
	{
		if (this == &rhs)
			return *this;
		peerId = rhs.peerId;
		reliability = rhs.reliability;
		ordering = rhs.ordering;
		channelId = rhs.channelId;
		return *this;
	}
	SendOptions& SendOptions::setPeerId(const PeerId id)
	{
		peerId = id;
		return *this;
	}
	SendOptions& SendOptions::setReliability(const Reliability rel)
	{
		reliability = rel;
		return *this;
	}
	SendOptions& SendOptions::setOrdering(const Ordering ord)
	{
		ordering = ord;
		return *this;
	}
	SendOptions& SendOptions::setChannel(const ChannelId id)
	{
		channelId = id;
		return *this;
	}

	//-------------------------------------------------------------------------
	// NativeTimer & Timer
	//-------------------------------------------------------------------------
	namespace detail {
		class NativeTimer
		{
		public:
			NativeTimer()
			{
				mCurrent = getTime();
				mLast = mCurrent;
			}
			~NativeTimer()  {}

			void tick()
			{
				mLast = mCurrent;
				mCurrent = getTime();
			}

			double getDiff()
			{
				return mCurrent - mLast;
			}

			double getTime()
			{
				QueryPerformanceFrequency( (LARGE_INTEGER*)&freq );
				QueryPerformanceCounter( (LARGE_INTEGER*)&time );
				return (double)(time) / (double)freq;
			}

		private:
			__int64  time;
			__int64  freq;
			double	mLast;
			double	mCurrent;
		};
	} // namespace detail
	Timer::Timer() : timer_( new detail::NativeTimer() ), start_(0.), paused_(false), time_(0.)
	{
	}
	Timer::~Timer()
	{
		if (timer_)
			delete timer_;
	}
	void Timer::start()
	{
		start_ = timer_->getTime();
		time_ = 0.;
		paused_ = false;
	}
	void Timer::stop()
	{
		paused_ = true;
	}
	void Timer::pause()
	{
		paused_ = true;
	}
	void Timer::resume()
	{
		const double dt = timer_->getTime() - time_;
		start_ += dt;
		paused_ = false;
	}
	void Timer::reset()
	{
		stop();
		start();
	}
	double Timer::getTime() const
	{
		if (!paused_)
			time_ = (timer_->getTime() - start_);
		return time_;
	}

} // namespace net
