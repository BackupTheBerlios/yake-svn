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

static const std::string FILE_CONTENT = "tastes";
static unsigned char FILE_CONTENT_RAW[] = "tastes";
static const size_t FILE_SIZE = FILE_CONTENT.size();

static const std::string FTP = "ftp://myfriend:myfriend@machine1.myip.us";
static const std::string FTP_DIR = "yake_ftp";
static const std::string FTP_FILE = "yake_ftp/ftp_cheeze";

// util ---------------------------------------------------------//
#define YAKE_CHECK(exp)\
  ( (exp)\
    ? static_cast<void>(0)\
	  : ( std::cout << "exp: " << #exp << std::endl\
		<< "file: " << __FILE__ << std::endl\
		<< "line: " << __LINE__ << std::endl ) )

#define YAKE_CHECK_EXP(fon, exp)\
	bool #fon()\
	{\
		using namespace filesystem;\
		try	{ #exp }\
		catch ( ... )\
		{ return true; } { return false; }\
	}
	

// common: exceptions ---------------------------------------------------------//
bool test_exceptions()
{
	using namespace filesystem;
	try	{ throw( filesystem_error( "main", "doh" ) ); }
	catch ( ... ) 
	{ return true; } { return false; }
}

// common: paths ---------------------------------------------------------//
bool test_path_normalize()
{
	using namespace filesystem;
	path test_path( "foobar/cheeze/" );
	return ( test_path.string() == test_path.normalize().string() );	
}

// native: operations ---------------------------------------------------------//
void create_dummy_file( filesystem::path ph )
{
  using namespace filesystem;
	using namespace filesystem::nativefs;

	nativefs::handle file_handle = open_file( 
		ph, std::ios_base::out );
	assert( write_file( file_handle, 
		FILE_CONTENT_RAW, FILE_SIZE ) == 6 );
	close_file( file_handle );
	if ( !exists( ph ) ) 
		throw(	
			filesystem_error( "create_dummy_file",
				"File does not exist." ) );
}

bool test_native_operations()
{
	YAKE_LOG( "test_native_operations()" );

	using namespace filesystem;
	using namespace filesystem::nativefs;

	remove_all( "test_data" );
	create_directory( "test_data" );
	path filename( "test_data/cheeze" );

	try
	{		
		{ // write
			create_dummy_file( filename );
		}		
		{ // read
			nativefs::handle file_handle = open_file( 
				filename, std::ios_base::in );
			unsigned char* buffer = new unsigned char[ FILE_SIZE + 1 ];
			assert( read_file( file_handle, buffer, FILE_SIZE ) == 6 );
			buffer[ 6 ] = '\0';
			std::cout << buffer << std::endl;
			delete[] buffer;
			close_file( file_handle );	
		}
	}
	catch ( ... ) 
	{ return false; } { return true; }
}

bool test_native_streams()
{
	YAKE_LOG( "test_native_streams()" );

	using namespace filesystem;
	namespace nfs = filesystem::nativefs;

	nfs::remove_all( "test_data" );
	nfs::create_directory( "test_data" );
	path native_filename( "test_data/cheeze" );

	{ // out
		nfs::ofstream file( native_filename );
		file << FILE_CONTENT;
		file.close();
		if ( !nfs::exists( native_filename ) )
			throw exception();
	}

	{ // in
		nfs::ifstream file( native_filename );
		std::string data;
		file >> data;
		std::cout << data << std::endl;
		return( data == FILE_CONTENT );
	}
}

bool test_native_iterator()
{
	YAKE_LOG(  "test_native_iterator()" );

	using namespace filesystem;
	using namespace filesystem::nativefs;

	path ph( "test_data" );

	remove_all( ph );
	create_directory( ph );
	create_dummy_file( ph / "hello1" );
	create_dummy_file( ph / "hello2" );
	create_dummy_file( ph / "hello3" );

	int count = 0;
	nativefs::directory_iterator end_itr;
	for( nativefs::directory_iterator iter( ph );
		iter !=  end_itr;
		++iter, count++ )
	{
		std::cout << iter->string() << std::endl;
	}

	return( count == 3 );
}

// virtual: root management ---------------------------------------------------------//
bool test_root_absolute_path()
{
	using namespace filesystem;	
	using namespace filesystem::virtualfs;

	try { add_root_path( path( "test_data/" ) ); }
	catch ( ... ) 
	{ return true; } { return false; }
}

bool test_root_remove_path()
{
	using namespace filesystem;
	using namespace filesystem::virtualfs;

	// add
	path root_path
#ifdef YAKE_WINDOWS
	( "c:/", native );
#else
	( "/etc" );
#endif
	add_root_path( root_path );
	// remove
	try { remove_root_path( root_path ); }
	catch ( ... ) 
	{ return false; } { return true; }
}

bool test_create_archive()
{
  YAKE_LOG(  "test_create_archive()" );

	using namespace filesystem;
	using namespace nativefs;

	// create files
	path native_files( "test_data", no_check );
	remove_all( native_files );
	create_directory( native_files );
	create_dummy_file( native_files / "cheeze" );
	create_dummy_file( native_files / "hello1" );
	create_dummy_file( native_files / "hello2" );
	create_dummy_file( native_files / "hello3" );

	// create archive
	virtualfs::create_archive_from( 
		path( "virtual_test_data.vfs", no_check ), 
			native_files );

	// exists?
	return nativefs::exists( "virtual_test_data.vfs" );
}

bool test_virtual_file()
{
  YAKE_LOG(  "test_virtual_file()" );

	using namespace filesystem;
	virtualfs::ifstream file( path( "virtual_test_data/cheeze" ) ); 
	std::string data;
	file >> data;
	std::cout << data << std::endl;
	return data == FILE_CONTENT;
}

bool test_virtual_streams()
{
	YAKE_LOG(  "test_virtual_streams()" );

	using namespace filesystem;
	namespace nfs = filesystem::nativefs;
	namespace vfs = filesystem::virtualfs;

	nfs::remove_all( "test_data" );
	nfs::create_directory( "test_data" );
	path native_filename( "test_data/cheeze" );
	path virtual_filename( "virtual_test_data/cheeze" );

	{ // out
		nfs::ofstream file( native_filename );
		file << FILE_CONTENT;
		file.close();
		if ( !nfs::exists( native_filename ) )
			throw exception();
	}

	bool ok = true;

	{ // virtual filesystem streams using native filebuffer
		vfs::ifstream file( native_filename );
		std::string data;
		file >> data;
		std::cout << data << std::endl;
		if( data != FILE_CONTENT ) ok = false;
	}

	{ // virtual filesystem stream using explicitly instanced virtual filebuffer
		std::streambuf & buf = vfs::create_filebuf( native_filename, 
			std::ios_base::in, vfs::force_virtual );
		vfs::ifstream file( buf );
		std::string data;
		file >> data;
		std::cout << data << std::endl;
		if( data != FILE_CONTENT ) ok = false;
	}

	{ // virtual filesystem stream using virtual filebuffer	  
		vfs::ifstream file( virtual_filename ); 
		std::string data;
		file >> data;
		std::cout << data << std::endl;
		if( data != FILE_CONTENT ) ok = false;
	}

	return ok;
}

bool test_virtual_nfs_iterator()
{
	YAKE_LOG(  "test_virtual_nfs_iterator()" );

	using namespace filesystem;
	using namespace filesystem::virtualfs;

	path native_files( "test_data", no_check );
	remove_all( native_files );

	// the virtualfs needs a more detailed path here 
	// in order to resolve the nativefs
	create_directory( "../Test" / native_files );

	create_dummy_file( native_files / "cheeze" );
	create_dummy_file( native_files / "hello1" );
	create_dummy_file( native_files / "hello2" );
	create_dummy_file( native_files / "hello3" );

	int count = 0;
	virtualfs::directory_iterator end_itr;
	for( virtualfs::directory_iterator iter( native_files );
		iter != end_itr;
		++iter, count++ )
	{
		std::cout << iter->string() << std::endl;
	}

	return( count == 4 );
}

bool test_virtual_vfs_iterator()
{
	YAKE_LOG(  "test_virtual_vfs_iterator()" );

	using namespace filesystem;
	using namespace filesystem::virtualfs;

	path ph( "virtual_test_data" );

	int count = 0;
	virtualfs::directory_iterator end_itr;
	for( virtualfs::directory_iterator iter( ph );
		iter != end_itr;
		++iter, count++ )
	{
		std::cout << iter->string() << std::endl;
	}

	return( count == 4 );
}

// todo
bool test_virtual_seek()
{
	YAKE_LOG(  "test_virtual_seek()" );

	using namespace filesystem;
	using namespace filesystem::virtualfs;

	return true;
}

// virtual: ftp ---------------------------------------------------------//
bool test_ftp_exists()
{
	YAKE_LOG( "test_ftp_exists()" );

	using namespace filesystem;
	using namespace filesystem::virtualfs;

	// add path
	add_root_path( FTP, shared_path );

	// exists file on ftp?
	return exists( path( FTP_FILE, no_check ) );
}

bool test_ftp_file()
{
  YAKE_LOG( "test_ftp_file()" );

	using namespace filesystem;
	virtualfs::ifstream file( FTP_FILE ); 
	std::string data;
	file >> data;
	std::cout << data << std::endl;
	return data == FILE_CONTENT;
}

bool test_ftp_iterator()
{
	YAKE_LOG( "test_ftp_iterator()" );

	using namespace filesystem;
	using namespace filesystem::virtualfs;

	virtualfs::directory_iterator end_itr;
	for( virtualfs::directory_iterator iter( FTP_DIR );
		iter != end_itr;
		++iter )
	{
		std::cout << iter->string() << std::endl;
	}

	return true;
}

bool test_ftp_vfs_mixing()
{
	YAKE_LOG( "test_ftp_vfs_mixing()" );

	using namespace filesystem;
	using namespace filesystem::virtualfs;

	// mix the virtual_file_system and ftp
	virtualfs::ifstream file( path( FTP_DIR 
		+ "/virtual_test_data/cheeze" ) ); 
	std::string data;
	file >> data;
	std::cout << data << std::endl;
	return data == FILE_CONTENT;

	return true;
}

// main ---------------------------------------------------------//
int main()
{	
	YAKE_LOG(  "## filesystem test" );
	try
	{
		// common
		YAKE_CHECK( test_exceptions() );
		YAKE_CHECK( test_path_normalize() );

		// native
		YAKE_USE_LIB( native_file_system )
		YAKE_CHECK( test_native_operations() );
		YAKE_CHECK( test_native_streams() );
		YAKE_CHECK( test_native_iterator() );

		// virtual
		YAKE_USE_LIB( virtual_file_system )
		YAKE_CHECK( test_root_absolute_path() );
		YAKE_CHECK( test_root_remove_path() );
		YAKE_CHECK( test_create_archive() );
		YAKE_CHECK( test_virtual_file() );
		YAKE_CHECK( test_virtual_streams() );
		YAKE_CHECK( test_virtual_nfs_iterator() );
		YAKE_CHECK( test_virtual_vfs_iterator() );
		YAKE_CHECK( test_virtual_seek() );

		// ftp
		YAKE_USE_LIB( ftp_file_system )
		YAKE_CHECK( test_ftp_exists() );
		YAKE_CHECK( test_ftp_file() );
		YAKE_CHECK( test_ftp_iterator() );
		//YAKE_CHECK( test_ftp_vfs_mixing() );
	}
	catch( const filesystem::filesystem_error & err )
	{
		std::cout << err.who() << err.what();
	}
	catch( ... )
	{
		std::cout << "error: exception occured" << std::endl;
	}
	std::cin.get();
	return 0;
}