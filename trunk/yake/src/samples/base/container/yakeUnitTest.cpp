//****************************************************************************
//**
//**    UNITTEST.CPP
//**    Unit test
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Yake
#include <inc/base/yake.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
class IntSequence
{
private:
	int Value;
public:
	IntSequence( int Start ) : Value( Start )	{}
	int operator() ()	{ return Value++; }
};

//============================================================================
//    IMPLEMENTATION FUNCTIONS
//============================================================================
int main()
{
	yake::base::templates::Vector< int > myVector;

	// Push/Pop
	std::cout << "# Push/Pop # " << std::endl;
	myVector.push_back( 100 );
	std::cout << "size: " << (unsigned int) myVector.size() << std::endl;
	assert( myVector.size() == 1 );
	myVector.pop_back();
	std::cout << "size: " << (unsigned int) myVector.size() << std::endl;
	assert( myVector.size() == 0 );

	// Elements
	std::cout << std::endl << "# Elements # " << std::endl;
	myVector.insert( myVector.begin(), 200 );
	std::cout << "element: " << myVector[ 0 ] << " " << std::endl;
	assert(	( myVector[ 0 ] == myVector.front() ) 
		&& ( myVector.front() == myVector.back() ) 
		&& ( myVector.at( 0 ) == myVector.front() ) 
	);

	// Assign
	std::cout << std::endl << "# Assign # " << std::endl;
  yake::base::templates::Vector< int > myVector2 = myVector;
	
	myVector2.assign( 10, 111 );
	std::cout << "Size: " << (unsigned int) myVector2.size() << std::endl;
	assert( myVector2.size() == 10 );
	std::copy( myVector2.begin(), myVector2.end(), std::ostream_iterator< int >( std::cout, " " ) );
	std::cout << std::endl;

	// Erase
	std::cout << std::endl << "# Erase # " << std::endl;
	myVector2.clear();

	std::generate_n( std::back_inserter( myVector2 ), 9, IntSequence( 1 ) );
	std::copy( myVector2.begin(), myVector2.end(), std::ostream_iterator< int >( std::cout, " " ) );
	std::cout << std::endl;

	myVector2.erase( std::find( myVector2.begin(), myVector2.end(), 2 ) );
	std::copy( myVector2.begin(), myVector2.end(), std::ostream_iterator< int >( std::cout, " " ) );
	std::cout << std::endl;
	assert( myVector2.size() == 8 );

	myVector2.resize( 6 );
	assert( myVector2.size() == 6 );
	std::copy( myVector2.begin(), myVector2.end(), std::ostream_iterator< int >( std::cout, " " ) );
	std::cout << std::endl;

	// Iter
	std::cout << std::endl << "# Iter # " << std::endl;
	std::copy( myVector2.begin(), myVector2.end(), std::ostream_iterator< int >( std::cout, " " ) );
	std::cout << std::endl;
	std::copy( myVector2.rbegin(), myVector2.rend(), std::ostream_iterator< int >( std::cout, " " ) );
	std::cout << std::endl;


	// Wait for a keystroke (in debug builds only).
#if defined( YAKE_DEBUG_BUILD )
	std::cout << std::endl << "Waiting for you...";
	std::cin.get();
#endif

	return 0;
}