#ifndef _POINT_H_
#define _POINT_H_

#include <boost/function.hpp>

namespace yake
{
namespace gui
{

class button_base;

struct point 
{
	typedef boost::function<void (const point &)> on_change_functor;

	point() {}
	point(float this_x, float this_y) : x(this_x), y(this_y) {}

protected:
	friend button_base;
	point(float this_x, float this_y, on_change_functor functor)
		: x(this_x), y(this_y), on_change(functor) {}

public:
	void set_x(float this_x) { x = this_x; on_change(*this); }
	float get_x() const { return x; }

	void set_y(float this_y) { y = this_y; on_change(*this); }
	float get_y() const { return y; }

	float x, y;

	on_change_functor on_change;
};

} // namespace gui
} // namespace yake

#endif // _POINT_H_