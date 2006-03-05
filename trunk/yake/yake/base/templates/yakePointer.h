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
#ifndef YAKE_BASE_TEMPLATES_POINTER_H
#define YAKE_BASE_TEMPLATES_POINTER_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#	include "../yakePrerequisites.h"
#endif
// Boost
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/assert.hpp>
// Yake
#include <yake/base/yakeException.h>
#include <yake/base/mpl/yakeCast.h>
#include <yake/base/mpl/yakeTraits.h>
#include <yake/base/mpl/yakeBuildArguments.h>
#include <yake/base/mpl/null_type.h>


// todo : use loki and add create() iteration

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

	template< typename T >
	class AutoPtr : public std::auto_ptr< T >
	{
	public:
		AutoPtr(): std::auto_ptr< T >()  {}

		template< class Y >
		AutoPtr( AutoPtr<Y> const& r ): std::auto_ptr< T >( r )  {}

		template< class Y >
		explicit AutoPtr( Y* p ): std::auto_ptr< T >( p ) {}

		~AutoPtr() {}
	};

	template< typename T >
	class SharedPtr : public boost::shared_ptr< T >
	{
	// Class
	public:
		SharedPtr(): boost::shared_ptr< T >()  {}

		SharedPtr( boost::shared_ptr< T > const& r ) : boost::shared_ptr< T >( r ) {}

		template< class Y >
		SharedPtr( SharedPtr<Y> const& r ): boost::shared_ptr< T >( r )  {}

		template< class Y >
		explicit SharedPtr( Y* p ): boost::shared_ptr< T >( p ) {}

		template<class Y>
		SharedPtr(SharedPtr<Y> const & r, ::boost::detail::dynamic_cast_tag d): 
		boost::shared_ptr< T >( r, d ) {}

		~SharedPtr() {}
	};

	template<class T, class U>
    bool operator==(SharedPtr<T> const & a, SharedPtr<U> const & b)
	{
		return (::boost::shared_ptr<T>(a) == ::boost::shared_ptr<U>(b) );
	}

	template<class T, class U> SharedPtr<T> dynamic_pointer_cast(SharedPtr<U> const & r)
	{
		return SharedPtr<T>(r, ::boost::detail::dynamic_cast_tag());
	}

	template<typename T>
	class WeakPtr : public ::boost::weak_ptr< T >
	{
	public:
		typedef T element_Type;
		WeakPtr() {}

		WeakPtr( boost::weak_ptr< T > const& r ) : boost::weak_ptr< T >( r ) {}

		template<class Y>
		WeakPtr(const SharedPtr<Y>& r) :
			::boost::weak_ptr<T>( ::boost::weak_ptr<Y>(r) )
		{}

		WeakPtr(const WeakPtr& r) :
			::boost::weak_ptr<T>( ::boost::weak_ptr<T>(r) )
		{}

		template<class Y>
		WeakPtr(const WeakPtr<Y>& r) :
			::boost::weak_ptr<T>( ::boost::weak_ptr<Y>(r) )
		{}

/*		WeakPtr& operator=(WeakPtr const & r)
		{
			return WeakPtr( r );
		}*/
		template<class Y> WeakPtr & operator=(WeakPtr<Y> const & r)
		{
			return ::boost::weak_ptr<T>( ::boost::weak_ptr<Y>( r ) );
		}
		template<class Y> WeakPtr<T> & operator=(SharedPtr<Y> const & r)
		{
			return WeakPtr<T>( ::boost::weak_ptr<T>( ::boost::weak_ptr<Y>( r ) ) );
		}

		SharedPtr<T> lock() const
		{
			return ::boost::weak_ptr<T>::lock();
		}

		bool expired() const
		{
			return ::boost::weak_ptr<T>::expired();
		}
	};

} // yake

#endif // YAKE_BASE_TEMPLATES_POINTER_H
