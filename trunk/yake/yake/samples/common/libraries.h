#ifndef _YAKE_SAMPLES_COMMON_LIBRARIES_H_
#define _YAKE_SAMPLES_COMMON_LIBRARIES_H_

// stl
#include <vector>
#include <string>

namespace yake 
{
namespace samples 
{
namespace common 
{

struct libraries
{
	libraries & operator<<(const char * lib)
	{ m_libs.push_back(lib); return *this; }

	operator std::vector<std::string>()
	{ return m_libs; }

private:
	std::vector<std::string> m_libs;
};

} // namespace common
} // namespace samples
} // namespace yake

#endif // _YAKE_SAMPLES_COMMON_LIBRARIES_H_
