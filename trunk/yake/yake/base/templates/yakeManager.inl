//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace templates
{

/* Map default */
template< typename Identifier_, class Value_,	template< typename, class > class ManagerContainerPolicy_,	template< typename, class > class ManagerErrorPolicy_ >
bool DefaultFunctionsNames::Map< Identifier_, Value_, ManagerContainerPolicy_, ManagerErrorPolicy_ >::doAdd( const Identifier_& rIdentifier, Value_ rValue )
{
	return mContainer.insert( Container::value_type( rIdentifier, rValue ) ).second;
}

template< typename Identifier_, class Value_,	template< typename, class > class ManagerContainerPolicy_,	template< typename, class > class ManagerErrorPolicy_ >
bool DefaultFunctionsNames::Map< Identifier_, Value_, ManagerContainerPolicy_, ManagerErrorPolicy_ >::doDelete( const Identifier_& rIdentifier )
{
	return mContainer.erase( rIdentifier ) == 1;
}

template< typename Identifier_, class Value_,	template< typename, class > class ManagerContainerPolicy_,	template< typename, class > class ManagerErrorPolicy_ >
void DefaultFunctionsNames::Map< Identifier_, Value_, ManagerContainerPolicy_, ManagerErrorPolicy_ >::doClear()
{
    mContainer.clear();
}

template< typename Identifier_, class Value_,	template< typename, class > class ManagerContainerPolicy_,	template< typename, class > class ManagerErrorPolicy_ >
Value_ DefaultFunctionsNames::Map< Identifier_, Value_, ManagerContainerPolicy_, ManagerErrorPolicy_ >::getObject( const Identifier_& rIdentifier ) const
{
	typename Container::const_iterator iter = mContainer.find( rIdentifier );
    if( iter != mContainer.end() )
    {
       return iter->second;
    }
	return OnUnknownType( rIdentifier );
}

template< typename Identifier_, class Value_,	template< typename, class > class ManagerContainerPolicy_,	template< typename, class > class ManagerErrorPolicy_ >
std::vector< Identifier_ > DefaultFunctionsNames::Map< Identifier_, Value_, ManagerContainerPolicy_, ManagerErrorPolicy_ >::getIdentifiers() const
{
	std::vector< Identifier_ > result;

	for( typename Container::const_iterator iter = mContainer.begin(); iter != mContainer.end(); iter++ )
		result.push_back( ( *iter ).first );

	return result;
}

/* Vector default */
template< typename Identifier_ >
void DefaultFunctionsNames::Vector< Identifier_ >::doAdd( const Identifier_& rIdentifier ) 
{
	mContainer.push_back( rIdentifier );
}

template< typename Identifier_ >
void DefaultFunctionsNames::Vector< Identifier_ >::doDelete( const Identifier_& rIdentifier ) 
{
	YAKE_DECLARE_FUNCTION( doDelete )

	if( std::find( mContainer.begin(), mContainer.end(), rIdentifier ) == mContainer.end() )
		throw yake::base::Exception( "Unknown Identifier_.", YAKE_HERE );

	mContainer.erase( std::find( mContainer.begin(), mContainer.end(), rIdentifier ) );
}

template< typename Identifier_ >
void DefaultFunctionsNames::Vector< Identifier_ >::doClear() 
{
	mContainer.clear();
}

/* Map register */
template< typename Identifier_, class Value_,	template< typename, class > class ManagerContainerPolicy_,	template< typename, class > class ManagerErrorPolicy_ >
bool RegisterFunctionsNames::Map< Identifier_, Value_, ManagerContainerPolicy_, ManagerErrorPolicy_ >::doRegister( const Identifier_& rIdentifier, Value_ rValue )
{
	return mContainer.insert( Container::value_type( rIdentifier, rValue ) ).second;
}

template< typename Identifier_, class Value_,	template< typename, class > class ManagerContainerPolicy_,	template< typename, class > class ManagerErrorPolicy_ >
Value_ RegisterFunctionsNames::Map< Identifier_, Value_, ManagerContainerPolicy_, ManagerErrorPolicy_ >::getObject( const Identifier_& rIdentifier ) const
{
	typename Container::const_iterator iter = mContainer.find( rIdentifier );
    if( iter != mContainer.end() )
    {
       return iter->second;
    }
	return OnUnknownType( rIdentifier );
}

template< typename Identifier_, class Value_,	template< typename, class > class ManagerContainerPolicy_,	template< typename, class > class ManagerErrorPolicy_ >
std::vector< Identifier_ > RegisterFunctionsNames::Map< Identifier_, Value_, ManagerContainerPolicy_, ManagerErrorPolicy_ >::getIdentifiers() const
{
	std::vector< Identifier_ > result;

	for( typename Container::const_iterator iter = mContainer.begin(); iter != mContainer.end(); iter++ )
		result.push_back( ( *iter ).first );

	return result;
}

/* Vector register */
template< typename Identifier_ >
void RegisterFunctionsNames::Vector< Identifier_ >::doRegister( const Identifier_& rIdentifier ) 
{
	mContainer.push_back( rIdentifier );
}

} // templates
} // base
} // yake
