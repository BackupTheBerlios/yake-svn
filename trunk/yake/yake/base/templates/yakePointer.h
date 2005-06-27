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
	

/*namespace PointerConfigs
{
  struct Standard
  {
    template< typename T >
    struct LifeTimePolicy
    {
      static void OnPreCreate( const T* notValidYet )     {}
      static void OnPostCreate( const T* validNow )       {}
      static void OnPreRelease( const T* stillValid )     {}
      static void OnPostRelease( const T* invalidNow )    {}
    };

    template< typename T >
    struct SafetyPolicy
    {
      YAKE_DECLARE_CLASS( yake::base::PointerConfigs::Standard::SafetyPolicy<> )
      static void Check( const T* instance )              
			{ 
				YAKE_DECLARE_FUNCTION( Check ) 
				if( 0 == instance ) 
					throw yake::Exception( "Null pointer detected.", YAKE_HERE );
			}
    };
  };

  struct Fast
  {
    template< typename T >
    struct LifeTimePolicy
    {
      static void OnPreCreate( const T* notValidYet )     {}
      static void OnPostCreate( const T* validNow )       {}
      static void OnPreRelease( const T* stillValid )     {}
      static void OnPostRelease( const T* invalidNow )    {}
    };

    template< typename T >
    struct SafetyPolicy
    {
      static void Check( const T* instance )              {}
    };
  };
};

template< typename T, typename Config_ = PointerConfigs::Standard >
class Pointer
{
YAKE_DECLARE_CLASS( yake::base::templates::Pointer<> )
public:
  typedef Config_ Config;
  typedef Pointer< T, Config > Self;

  Pointer() : instance_( 0 )                                                    
	{}

  template< typename U, typename UConfig >
	Pointer( const Pointer< U, UConfig >& pointer )                             
	{ 
		BOOST_MPL_ASSERT_IS_SAME( Config, UConfig ); 
		instance_ = yake::base::templates::PolymorphicCast< T, U >::execute( Pointer< U, UConfig >::GetInstancePointer( pointer ) ); 
		AddReference_(); 
	}

  Pointer( const Self& pointer ) : instance_( pointer.instance_ )               
	{ AddReference_(); }

  ~Pointer()
	{ Release_(); }

  template< typename U, typename UConfig >
	Self& operator=( const Pointer< U, UConfig >& pointer )                     
	{ 
		BOOST_MPL_ASSERT_IS_SAME( Config, UConfig ); 
		if( instance_ != Pointer< U, UConfig >::GetInstancePointer( pointer ) ) 
		{ 
				Release_(); 
				instance_ = yake::base::templates::PolymorphicCast< T, U >::execute( Pointer< U, UConfig >::GetInstancePointer( pointer ) ); 
				AddReference_(); 
		} 
		return *this; 
	}
  
	Self& operator=( const Self& pointer )                                        
	{ 
		if( instance_ != pointer.instance_ ) 
		{ 
			Release_(); 
			instance_ = pointer.instance_; 
			AddReference_(); 
		} 
		return *this; 
	}

  bool operator==( const Self& pointer ) const
	{ return instance_ == pointer.instance_; }
  
	bool operator!=( const Self& pointer ) const
	{ return instance_ != pointer.instance_; }

  T* operator->() const
	{ Config::SafetyPolicy< T >::Check( instance_ ); return instance_; }

  T& operator*() const      
	{ Config::SafetyPolicy< T >::Check( instance_ ); return *instance_; }

  static const Self NULL_POINTER;

  static void Release( Self& pointer )             
	{ pointer.Release_(); }

  static T* GetInstancePointer( const Self& pointer )     
	{ return pointer.instance_; }

#define FUNCTION( number ) \
  YAKE_TYPES( number ) \
  static Pointer< T, Config > create( YAKE_ARGUMENTS( number ) ) \
  { \
    YAKE_DECLARE_FUNCTION( create ) \
	  int8* rawMemory = static_cast< int8* >( operator new( REFERENCE_COUNT_SIZE + sizeof( T ) ) ); \
	  T* instance = reinterpret_cast< T* >( rawMemory + REFERENCE_COUNT_SIZE ); \
    try \
    { \
      Config::LifeTimePolicy< T >::OnPreCreate( instance ); \
		  new( instance ) T ( YAKE_USE_ARGUMENTS( number ) ); \
      Config::LifeTimePolicy< T >::OnPostCreate( instance ); \
	  } \
	  catch( ... ) \
	  { \
		  operator delete( rawMemory ); \
		  throw; \
	  } \
		return Pointer< T, Config >( instance, yake::base::templates::NullType() ); \
  }
YAKE_IMPLEMENT_FUNCTION( FUNCTION )
#undef FUNCTION

private:
  enum { REFERENCE_COUNT_SIZE = ALIGN( sizeof( int ) ) };

	Pointer( T* instance, yake::base::templates::NullType nullType ) : instance_( instance )  { SetReferenceCount_( 1 ); }

  void AddReference_()                                                          
	{ 
		if( !instance_ ) return; 
		SetReferenceCount_( GetReferenceCount_() + 1 ); 
	}

  void Release_()                                                               
	{ 
		YAKE_DECLARE_FUNCTION( Release ) 
		if( instance_ ) 
		{ 
			SetReferenceCount_( GetReferenceCount_() - 1 ); 
			if( GetReferenceCount_() == 0 ) 
			{ 
				Config::LifeTimePolicy< T >::OnPreRelease( instance_ ); 
				void* base = GetBasePointer_(); 
				instance_->~T(); 
				operator delete( base ); 
				Config::LifeTimePolicy< T >::OnPostRelease( instance_ ); 
			} 
			instance_ = 0; 
		} 
	}

	void* GetBasePointer_() const                                                 
	{ 
		if( !instance_ ) return 0; 
		return reinterpret_cast< int8* >( yake::base::templates::getBasePointer( instance_ ) ) - REFERENCE_COUNT_SIZE; 
	}

  void SetReferenceCount_( int referenceCount )                                 
	{
		if( !instance_ ) return; 
		*reinterpret_cast< int* >( GetBasePointer_() ) = referenceCount; 
	}

  int GetReferenceCount_() const                                                
	{ 
		if( !instance_ ) return 0; 
		return *reinterpret_cast< int* >( GetBasePointer_() ); 
	}

	T* instance_;
};

template< typename T, typename Config >
const Pointer< T, Config > Pointer< T, Config >::NULL_POINTER;


template< typename T, typename Config, typename U, typename UConfig >
bool operator==( const Pointer< T, Config >& pointer1, const Pointer< U, UConfig >& pointer2 )  { BOOST_MPL_ASSERT_IS_SAME( Config, UConfig ); return Pointer< T, Config >::GetInstancePointer( pointer1 ) == Pointer< U, UConfig >::GetInstancePointer( pointer2 ); }

template< typename T, typename Config, typename U, typename UConfig >
bool operator!=( const Pointer< T, Config >& pointer1, const Pointer< U, UConfig >& pointer2 )  { BOOST_MPL_ASSERT_IS_SAME( Config, UConfig ); return Pointer< T, Config >::GetInstancePointer( pointer1 ) != Pointer< U, UConfig >::GetInstancePointer( pointer2 ); }

#define FUNCTION( number ) \
	YAKE_TYPES_ONE_FREE( number ) \
  Pointer< T > Create( YAKE_ARGUMENTS( number ) ) \
	{ return Pointer< T >::Create( YAKE_USE_ARGUMENTS( number ) ); }
YAKE_IMPLEMENT_FUNCTION( FUNCTION )
#undef FUNCTION

#define FUNCTION( number ) \
	YAKE_TYPES_TWO_FREE( number ) \
  Pointer< T > Create( YAKE_ARGUMENTS( number ) ) \
	{ return Pointer< T, TT >::Create( YAKE_USE_ARGUMENTS( number ) ); }
YAKE_IMPLEMENT_FUNCTION( FUNCTION )
#undef FUNCTION*/

} // yake

#endif // YAKE_BASE_TEMPLATES_POINTER_H
