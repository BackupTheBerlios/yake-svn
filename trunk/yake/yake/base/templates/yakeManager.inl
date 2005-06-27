/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/

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
		throw yake::Exception( "Unknown Identifier_.", YAKE_HERE );

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
	return mContainer.insert( typename Container::value_type( rIdentifier, rValue ) ).second;
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
