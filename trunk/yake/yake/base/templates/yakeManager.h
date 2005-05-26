/**------------------------------------------------------------------------------------
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
 */
#ifndef YAKE_BASE_TEMPLATES_MANAGER_H
#define YAKE_BASE_TEMPLATES_MANAGER_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#	include "../yakePrerequisites.h"
#endif
// Yake
#include <yake/base/templates/yakeFastMap.h>
#include <yake/base/mpl/null_type.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace templates
{

// todo cleanup
	// this is too complex

/* Error policy */
template< typename Identifier_, class Value_ >
struct DefaultManagerError
{      
YAKE_DECLARE_CLASS( yake::base::templates::DefaultManagerError<> )
    static Value_ OnUnknownType( Identifier_ )
    {
      YAKE_DECLARE_FUNCTION( OnUnknownType )
      throw yake::Exception( "Unknown Type.", YAKE_HERE );
    }

    static void OnNotFound( Identifier_ )
    {
      YAKE_DECLARE_FUNCTION( OnNotFound )
      throw yake::Exception( "Unkown Identifier_.", YAKE_HERE );
    }
};

/* Container policy */
template< typename Identifier_, class Value_ >
struct DefaultContainer
{
  typedef AssocVector< Identifier_, Value_ > Type;
};

template< typename Identifier_, class Value_ >
struct STLContainer
{
	typedef std::map< Identifier_, Value_ > Type;
};

/* Naming policy */
struct DefaultFunctionsNames
{
	template
	< 
		typename Identifier_, 
		class Value_,
		template< typename, class > class ManagerContainerPolicy_,
		template< typename, class > class ManagerErrorPolicy_ 
	>
	struct Map : ManagerErrorPolicy_< Identifier_, Value_ >
	{
		YAKE_DECLARE_CLASS( yake::base::templates::DefaultFunctionsNames::Map<> )
		// Class
		virtual ~Map() {}

		// Types
		typedef typename ManagerContainerPolicy_< Identifier_, Value_ >::Type Container;

		// Methods
		bool doAdd( const Identifier_&, Value_ );
		bool doDelete( const Identifier_& );
		void doClear();
		Value_ getObject( const Identifier_& ) const;
		std::vector< Identifier_ > getIdentifiers() const;

		// Data
		protected:
			Container mContainer;
	};

	template< typename Identifier_ >
	struct Vector
	{
	YAKE_DECLARE_CLASS( yake::base::templates::DefaultFunctionsNames::Vector<> )
		// Class
		virtual ~Vector() {}

    // Types
		typedef std::vector< Identifier_ > Container;

		// Methods
		void doAdd( const Identifier_& );
		void doDelete( const Identifier_& );
		void doClear();

		// Data
		protected:
			Container mContainer;
	};
};

struct RegisterFunctionsNames
{
	template
	< 
		typename Identifier_, 
		class Value_,
		template< typename, class > class ManagerContainerPolicy_,
		template< typename, class > class ManagerErrorPolicy_ 
	>
	struct Map : ManagerErrorPolicy_< Identifier_, Value_ >
	{
		YAKE_DECLARE_CLASS( yake::base::templates::RegisterFunctionsNames::Map<> )
		// Class
		virtual ~Map() {}

		// Types
		typedef typename ManagerContainerPolicy_< Identifier_, Value_ >::Type Container;

		// Methods
		bool doRegister( const Identifier_&, Value_ );
		Value_ getObject( const Identifier_& ) const;
		std::vector< Identifier_ > /*const &*/ getIdentifiers() const; // todo

		// Data
		// todo protected:
	public:
			Container mContainer;
	};

	template< typename Identifier_ >
	struct Vector
	{
	YAKE_DECLARE_CLASS( yake::base::templates::RegisterFunctionsNames::Vector<> )
		// Class
		virtual ~Vector() {}

    // Types
		typedef std::vector< Identifier_ > Container;

		// Methods
		void doRegister( const Identifier_& );

		// Data
		protected:
			Container mContainer;
	};
};

/* Policy builder */
template
< 
	typename Identifier_, 
	class Value_ = mpl::null_type,
	class ManagerNamingPolicy_ = DefaultFunctionsNames,
	template< typename, class > class ManagerContainerPolicy_ = DefaultContainer,
	template< typename, class > class ManagerErrorPolicy_ = DefaultManagerError
>
class Manager : public ManagerNamingPolicy_::template Map< Identifier_, Value_, ManagerContainerPolicy_, DefaultManagerError >
{
// Class
public:
	Manager() {}
	virtual ~Manager() {}

	typedef typename ManagerContainerPolicy_< Identifier_, Value_ >::Type Container;
	typedef typename Container::iterator iterator;

	iterator begin() 
	{ return ManagerContainerPolicy_< Identifier_, Value_ >::mContainer.begin(); }

	iterator end() 
	{ return ManagerContainerPolicy_< Identifier_, Value_ >::mContainer.end(); }
};

// Todo: nifty container, function parameters
/* Policy Builder */
template< typename Identifier_, class ManagerNamingPolicy_ >
class Manager< Identifier_, mpl::null_type, ManagerNamingPolicy_ > : public ManagerNamingPolicy_::template Vector< Identifier_ >
{
// Class
public:
	Manager() {}
	virtual ~Manager() {}
};

} // templates
} // base
} // yake

// Include inline implementation.
#include "yakeManager.inl"

#endif // YAKE_BASE_TEMPLATES_MANAGER_H
