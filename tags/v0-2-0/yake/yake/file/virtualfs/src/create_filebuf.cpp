#include <inc/create_filebuf.hpp>
#include <nativefs/inc/operations.hpp>

namespace filesystem
{
namespace virtualfs
{

std::streambuf & create_filebuf( const path & ph, std::ios_base::openmode 
										 mode, force_system force_virtual )
{
	if( !force_virtual && nativefs::exists( ph ) )
	{ // create std file buffer
		nativefs::filebuf* filebuf = new nativefs::filebuf();
		if( !filebuf->open( ph, mode ) )
			throw exception();
		return (*filebuf);
	}

	// todo
	//if( force_virtual || resolve::instance().exists_fs_for( ph ) )
	//{
		// create virtual file buffer

	std::cout << "create_filebuf " << ph.string() << std::endl;

		virtualfs::filebuf * filebuf = new virtualfs::filebuf( ph, mode );
		//if( !filebuf->open( ph, mode ) ) throw exception();

	std::cout << "create_filebuf done" << std::endl;

		return (*filebuf);
	//}

	//throw exception(); // todo fs error
}

} // virtualfs
} // filesystem