#include <yake/base/yake.h>
#include <yake/thread/yakeThread.h>

class Data 
{
	Data() : count(0)
	{}
	static Data ms_instance;
public:
	static Data& instance()
	{
		return ms_instance;
	}
	void inc()
	{
		m.acquire();
		++count;
		m.release();
	}
	size_t getCount()
	{
		size_t val = 0;
		m.acquire();
		val = count;
		m.release();
		return val;
	}
private:
	yake::thread::Mutex m;
	size_t				count;
};
Data Data::ms_instance;

struct ExTask : public yake::thread::IRunnable
{
	ExTask(int n) : n_(n) {}
	virtual void run()
	{
		for (size_t i=0; i<5; ++i)
		{
			Data::instance().inc();
			std::cout << "ExTask " << n_ << " inc()'s!\n";
		}
		std::cout << "ExTask " << n_ << " : data.getCount() = " << Data::instance().getCount() << "\n";
	}
	int	n_;
};

int main()
{
	try 
	{
		using namespace yake::thread;

		ThreadPtr pThread1 = Thread::create( new ExTask(1) );
		ThreadPtr pThread2 = Thread::create( new ExTask(2) );
		pThread1->wait();
		pThread2->wait();
		delete pThread1;
		delete pThread2;
		std::cout << std::endl << "Waiting for you...";
		std::cin.get();
		std::cout << "FINAL: data.getCount() = " << Data::instance().getCount() << "\n";
#if defined( YAKE_DEBUG_BUILD )
		std::cout << std::endl << "Waiting for you...";
		std::cin.get();
#endif
	}
	catch ( yake::Exception & e )
	{
		std::cout << "YAKE EXCEPTION: " << e.getSource() << std::endl;
		std::cout << "File: " << e.getFile() << " @ " << e.getLine() << std::endl;
		std::cout << "Reason: " << e.getMessage() << std::endl << std::endl;
	}

	return 0;
}