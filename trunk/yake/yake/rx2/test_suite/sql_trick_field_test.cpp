// test.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <iostream>

template< typename T >
struct sql
{
public:
T & operator=( T const & value ) 
{ return value_; }

T value_;


};

template< typename T >
struct none
{
	inline void on_value_change() 
	{ std::cout << "1"; }
};


template< typename T, template< typename > class wrapper = none >
struct B : public wrapper<T>
{

public:
T & operator=( T const & value ) 
{ 
	on_value_change();
	return value_; 
}

T value_;


};

int main()
{
	B< int > test;
	test = 123 ;

	std::cin.get();

	return 0;
}

