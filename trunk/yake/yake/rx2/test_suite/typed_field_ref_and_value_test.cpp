#include <vector>
#include <iostream>

#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/remove_reference.hpp>


struct field_base
{
	virtual ~field_base() {}
};

template< typename T >
struct field : public field_base
{
	field() 
		: value_( boost::remove_reference<T>::type() ),
			ref_( value_ )
	{}

 	typename boost::remove_reference<T>::type value_;
	typename boost::add_reference<T>::type ref_;
};

struct object
{
	std::vector<field_base*> fields_;

	template< typename T >
	field<T> & get()
	{
    return static_cast< field<T> & >(*fields_[0]);
	}
};

int main()
{
	int test = 100;
	field<int> f1;
	f1.ref_ = test;
	object obj;
	obj.fields_.push_back( &f1 );

	field<int> f2 = obj.get<int>();

	std::cout << f2.value_;
	std::cin.get();



	return 0;
}

