#include <set>

#include <inc/resolve.hpp>
#include <yake/base/templates/yakeRegistry.h>

namespace filesystem
{
namespace virtualfs
{

//file_system_ptr resolve::resolve_fs( const path & ph )
//{

  // use registry identifier
  /*path current = *ph.begin();
	for( path::iterator iter( ph.begin() ); 
		iter != ph.end();
		iter++, current /= *iter )
	{
    if( nativefs::exists( current ) ) continue;

		for( system_list::iterator iter_sys( system_list_.begin() );
			iter_sys != system_list_.end();
			iter_sys++ )
		{
			if( boost::get<0>( *iter_sys ) == file_system::registry_resolving &&
				nativefs::exists( path( current.string() + "." + boost::get<1>( *iter_sys ) ) ) )
			{
				return boost::get<2>( *iter_sys );
			}
		}	
	}

	// use implemented method
	for( system_list::iterator iter_sys( system_list_.begin() );
		iter_sys != system_list_.end();
		iter_sys++ )
	{
		if( boost::get<0>( *iter_sys ) == file_system::implementation_resolving &&
      boost::get<2>( *iter_sys )->exists( ph ) )
		{	return boost::get<2>( *iter_sys ); }
	}*/

	/*static std::set< std::string > exlusion;

	for( system_list::iterator iter_sys( system_list_.begin() );
		iter_sys != system_list_.end();
		iter_sys++ )
	{
		std::cout << "-------------------------------" << std::endl;
		std::cout << "current: " << iter_sys->first << std::endl;

		if( exlusion.find( iter_sys->first ) == exlusion.end() )
		{
			exlusion.insert( iter_sys->first );
      if( iter_sys->second->exists( ph ) )
			{
				std::cout << "found: " << iter_sys->first << std::endl << std::endl;
				exlusion.clear();
				return iter_sys->second;
			}
		}
		else
		{
			std::cout << "exluded: " << iter_sys->first << std::endl;
		}
	}

	// unknown
	return file_system_ptr();  		
}*/

file_system_ptr resolve::resolve_fs_for( const path & ph )
{
	std::cout << "resolve::resolve_fs_for " << ph.string() << std::endl;

	static std::set< std::string > exlusion;

	for( system_list::iterator iter_sys( system_list_.begin() );
		iter_sys != system_list_.end();
		iter_sys++ )
	{
		std::cout << "-------------------------------" << std::endl;
		std::cout << "current: " << iter_sys->first << std::endl;

		if( exlusion.find( iter_sys->first ) == exlusion.end() )
		{
			exlusion.insert( iter_sys->first );
      if( iter_sys->second->exists( ph ) )
			{
				std::cout << "found: " << iter_sys->first << std::endl << std::endl;
				exlusion.clear();
				return iter_sys->second;
			}
		}
		else
		{
			std::cout << "exluded: " << iter_sys->first << std::endl;
		}
	}

	// unknown
	exlusion.clear();
	return file_system_ptr(); 

/*	file_system_ptr file_sys = resolve_fs( ph );
	if( file_sys.get() != NULL ) return file_sys;
	else{ std::cout << "resolve_fs_for: nothing" << std::endl; throw exception();  	 }	*/	
}

file_system_ptr resolve::resolve_fs_for_extension( const std::string & ph )
{
  for( system_list::iterator iter_sys( system_list_.begin() );
		iter_sys != system_list_.end();
		iter_sys++ )
	{
    if( iter_sys->first == ph  )
		{
      return iter_sys->second;
		}
	}
	throw exception(); // todo use filesystem_error everywhere
}

/*bool resolve::exists_fs_for( const path & ph )
{
	std::cout << "resolve::exists_fs_for" << std::endl;

	return resolve_fs( ph ).get() != NULL;
}*/

} // virtualfs
} // filesystem