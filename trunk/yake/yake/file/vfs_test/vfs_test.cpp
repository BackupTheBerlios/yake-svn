// stl
#include <iostream>
#include <iomanip>
#include <cassert>
#include <vector>

// yake
#include <yake/base/yake.h>

// common
#include <common/inc/exception.hpp>
#include <common/inc/path.hpp>

// native
#include <nativefs/inc/operations.hpp>
#include <nativefs/inc/ofstream.hpp>
#include <nativefs/inc/ifstream.hpp>

// virtual
#include <virtualfs/inc/root.hpp>
#include <virtualfs/inc/filebuf.hpp>
#include <virtualfs/inc/ifstream.hpp>
#include <virtualfs/inc/create_filebuf.hpp>

int main()
{
	YAKE_USE_LIB( virtual_file_system )
	{
		using namespace filesystem;
		using namespace filesystem::virtualfs;

		// create archive
		create_archive_from( path( "created_foobar.vfs", no_check ), path( "test_data", no_check ) );

		// load it
		virtualfs::ifstream file( path( "created_foobar/cheeze" ) ); 
		std::string data;
		file >> data;
		std::cout << data << std::endl;
		assert( data == "tastes" );

		// iterate
		path ph( "created_foobar" );
		virtualfs::directory_iterator end_itr;
		for( virtualfs::directory_iterator iter( ph );
			iter !=  end_itr;
			++iter )
		{
			std::cout << iter->string() << std::endl;
		}

		// wait
		std::cin.get();
	}
	return 0;
}