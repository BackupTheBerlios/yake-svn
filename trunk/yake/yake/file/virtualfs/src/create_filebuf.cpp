#include <inc/create_filebuf.hpp>

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

	//if( force_virtual || resolve::instance().exists_fs_for( ph ) )
	//{
		// create virtual file buffer
		virtualfs::filebuf* filebuf = new virtualfs::filebuf();
		if( !filebuf->open( ph, mode ) ) throw exception();
		return (*filebuf);
	//}

	//throw exception(); // todo fs error
}

} // virtualfs
} // filesystem