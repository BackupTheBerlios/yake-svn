#include "class_registry.h"
#include "meta_class.h"

namespace rx
{

void register_class( const meta_class & meta_class_ )
{
		class_registry::instance().classes_.insert( 
			classes::value_type( 
				meta_class_.get_name(), &meta_class_ ) ); 
}

void unregister_class( const meta_class & meta_class_ )
{
	class_registry::instance().classes_.erase( meta_class_.get_name() );
}

const meta_class & get_class( const std::string & name )
{
	classes::iterator iter = class_registry::instance().classes_.find( name );
	if( iter == class_registry::instance().classes_.end() ) throw exception();
	return *iter->second;  
}

meta_class::traits::iterator find_event_traits( meta_class & meta_class_, const std::string & event_name )
{
	for( meta_class::traits::iterator iter( meta_class_.event_traits_.begin() );
		iter != meta_class_.event_traits_.end();
		iter++ )
		if( iter->first == event_name ) return iter;

  return meta_class_.event_traits_.end();
}

meta_class::traits::iterator find_handler_traits( meta_class & meta_class_, const std::string & handler_name )
{
	for( meta_class::traits::iterator iter( meta_class_.handler_traits_.begin() );
		iter != meta_class_.handler_traits_.end();
		iter++ )
		if( iter->first == handler_name ) return iter;

  return meta_class_.event_traits_.end();
}

bool validate_events( const std::string & event, const std::string & handler )
{
	// get class name and event
	std::string event_class_name( event.substr( 0, event.find( "::" ) ) );
	std::string event_name( event.substr( event.find( "::" ) + 2, event.length() - event_class_name.length() - 2 ) );

	// get class name and handler
	std::string handler_class_name( handler.substr( 0, handler.find( "::" ) ) );
	std::string handler_name( handler.substr( handler.find( "::" ) + 2, handler.length() - handler_class_name.length() - 2 ) );

	// do the classes exist?
  if( class_registry::instance().classes_.find( event_class_name ) == class_registry::instance().classes_.end() 
		|| class_registry::instance().classes_.find( handler_class_name ) == class_registry::instance().classes_.end() )
		return false;

	// get classes
	meta_class & event_class = const_cast<meta_class &>( get_class( event_class_name ) );
	meta_class & handler_class = const_cast<meta_class &>( get_class( handler_class_name ) );

	// do handler and event exist?
  if( find_event_traits( event_class, event_name ) == event_class.event_traits_.end() 
		|| find_handler_traits( handler_class, handler_name ) == handler_class.handler_traits_.end() )
		return false;

	// get traits
	meta_class::arg_types & args_handler = find_event_traits( handler_class, event_name )->second;
  meta_class::arg_types & args_event = find_handler_traits( event_class, handler_name )->second;

	// compare
  for( meta_class::arg_types::iterator traits_handler( args_handler.begin() ), traits_event( args_event.begin() );
    traits_handler != args_handler.end();
    traits_handler++, traits_event++ )
    if( *traits_handler != *traits_event ) return false;  

	// fine
	return true;
}

} // namespace rx