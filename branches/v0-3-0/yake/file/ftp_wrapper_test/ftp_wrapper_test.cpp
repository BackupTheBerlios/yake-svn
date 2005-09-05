#include <cassert>
#include <ftp_wrapper/inc/ftp_client.hpp>

int main()
{
	using namespace filesystem::virtualfs;

	// connect
	ftp_client client( "ftp://myfriend:myfriend@machine1.myip.us" );

	// file list
	file_list list;
	client.list_files( ".", list );
	for( file_list::iterator iter( list.begin() ); 
		iter != list.end();
		iter++ )
	{
		assert( client.exists( *iter ) );
		std::cout << *iter << std::endl;
	}

	// get the file, throws if local file already exists
	try{ client.get_file( list[0], "c:/" + list[0] ); }
	catch( ... ){}


	std::cin.get();

	return 0;
}

