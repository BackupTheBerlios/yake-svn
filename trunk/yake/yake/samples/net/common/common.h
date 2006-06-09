#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <conio.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <boost/thread/mutex.hpp>
#include "config.h"

#define COUTLN(X) \
{ \
	std::stringstream ss; \
	ss << X << "\n"; \
	std::cout << ss.str(); \
}

template<typename T>
struct safe_var
{
	safe_var(const T& v = T()) : value_(v)
	{}
	void set(const T& v)
	{
		boost::mutex::scoped_lock lck(mtx_);
		value_ = v;
	}
	T getCopy() const
	{
		boost::mutex::scoped_lock lck(mtx_);
		return value_;
	}
	safe_var& operator = (const safe_var& rhs)
	{
		if (this == &rhs)
			return *this;
		set( rhs.getCopy() );
		return *this;
	}
private:
	T						value_;
	mutable boost::mutex	mtx_;
};

#endif
