#ifndef _FORWARD_BUFFER_H_
#define _FORWARD_BUFFER_H_

// what is causing this mess?
#ifdef realloc
# undef realloc
#endif

#include <iostream>
#include <string>
#include <boost/io/stream_facade.hpp>
#include <boost/io/categories.hpp>
#include <boost/function.hpp>


struct forward_sink 
{
    typedef char char_type;
    typedef boost::io::sink_tag category;
		typedef boost::function<void (const std::string &)> forward_fnc;

		forward_sink(forward_fnc fnc)
			: m_forward_fnc( fnc )
		{}

    void write(const char * s, std::streamsize n) 
    {	m_forward_fnc(std::string(s, n)); }

private:
	forward_fnc m_forward_fnc;
};

typedef boost::io::streambuf_facade<forward_sink> forward_buf;

#endif // _FORWARD_BUFFER_H_