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
#ifndef YAKE_BASE_TEMPLATES_FUNCTOR_H
#define YAKE_BASE_TEMPLATES_FUNCTOR_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Boost
#include <dependencies/boost/boost/function.hpp>


//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace templates
{
	using namespace boost;

/*template< typename ResultType, typename ParamList >
struct FunctorImpl
{
    typedef ResultType ResultType;
    typedef ParamList ParamList;

    typedef typename ParamList::TypeAt< 0 >::NonStrict< NullType >::Result Param1;
    typedef typename ParamList::TypeAt< 1 >::NonStrict< NullType >::Result Param2;
    typedef typename ParamList::TypeAt< 2 >::NonStrict< NullType >::Result Param3;
    typedef typename ParamList::TypeAt< 3 >::NonStrict< NullType >::Result Param4;
    typedef typename ParamList::TypeAt< 4 >::NonStrict< NullType >::Result Param5;
    typedef typename ParamList::TypeAt< 5 >::NonStrict< NullType >::Result Param6;
    typedef typename ParamList::TypeAt< 6 >::NonStrict< NullType >::Result Param7;
    typedef typename ParamList::TypeAt< 7 >::NonStrict< NullType >::Result Param8;
    typedef typename ParamList::TypeAt< 8 >::NonStrict< NullType >::Result Param9;
    typedef typename ParamList::TypeAt< 9 >::NonStrict< NullType >::Result Param10;

    template< int n >
    struct ResultImpl;

    template<>
    struct ResultImpl< 0 >
    {
        virtual ResultType operator()() = 0;
        virtual ResultImpl< 0 >* Clone() const = 0;
    };

    template<>
    struct ResultImpl< 1 >
    {
        virtual ResultType operator()( Param1& param1 ) = 0;
        virtual ResultImpl< 1 >* Clone() const = 0;
    };

    template<>
    struct ResultImpl< 2 >
    {
        virtual ResultType operator()( Param1& param1, Param2& param2 ) = 0;
        virtual ResultImpl< 2 >* Clone() const = 0;
    };

    template<>
    struct ResultImpl< 3 >
    {
        virtual ResultType operator()( Param1& param1, Param2& param2, Param3& param3 ) = 0;
        virtual ResultImpl< 3 >* Clone() const = 0;
    };

    typedef ResultImpl< ParamList::length > Result;
};

template< class ParentFunctor, typename T >
class FunctorHandler : public FunctorImpl< typename ParentFunctor::ResultType, typename ParentFunctor::ParamList >::Result
{
    T functor_;

public:
    typedef typename ParentFunctor::ResultType ResultType;

    FunctorHandler( const T& t )
        : functor_( t )
    {
    }

    FunctorHandler* Clone() const
    {
        return new FunctorHandler( *this );
    }

    ResultType operator()()
    {
        return functor_();
    }

    ResultType operator()( typename ParentFunctor::Param1& p1 )
    {
        return functor_( p1 );
    }

    ResultType operator()( typename ParentFunctor::Param1& p1, typename ParentFunctor::Param2& p2 )
    {
        return functor_( p1, p2 );
    }

    ResultType operator()( typename ParentFunctor::Param1& p1, typename ParentFunctor::Param2& p2, typename ParentFunctor::Param3& p3 )
    {
        return functor_( p1, p2, p3 );
    }
};


template< class ParentFunctor, typename Class, typename Function >
class MemberFunctionHandler : public FunctorImpl< typename ParentFunctor::ResultType, typename ParentFunctor::ParamList >::Result
{
    Class     m_Class;
    Function  m_Function;

public:
    typedef typename ParentFunctor::ResultType ResultType;

    MemberFunctionHandler( Class tClass, Function tFunction )
        : m_Class( tClass ), m_Function( tFunction )
    {
    }

    MemberFunctionHandler* Clone() const
    {
        return new MemberFunctionHandler( *this );
    }

    ResultType operator()()
    {
        return ( m_Class->*m_Function )();
    }

    ResultType operator()( typename ParentFunctor::Param1& p1 )
    {
        return ( m_Class->*m_Function )( p1 );
    }

    ResultType operator()( typename ParentFunctor::Param1& p1, typename ParentFunctor::Param2& p2 )
    {
        return ( m_Class->*m_Function )( p1, p2 );
    }

    ResultType operator()( typename ParentFunctor::Param1& p1, typename ParentFunctor::Param2& p2, typename ParentFunctor::Param3& p3 )
    {
        return ( m_Class->*m_Function )( p1, p2, p3 );
    }
};

template< typename T >
T ForFunction( T t )
{
    return t;
}

template< typename T >
const T& ForInstance( const T& t )
{
    return t;
}

template< typename Class, typename Function >
struct ForMemberFunctionImpl
{
    ForMemberFunctionImpl( Class tClass, Function tFunction )
        : m_Class( tClass ), m_Function( tFunction )
    {}

    Class    m_Class;
    Function m_Function;
};

template< typename Class, typename Function >
ForMemberFunctionImpl< Class, Function > ForMemberFunction( Class tClass, Function tFunction )
{
    return ForMemberFunctionImpl< Class, Function >( tClass, tFunction );
}


template< typename ResultType, typename ParamList >
class Functor
{
public:

		typedef ResultType ResultType;
    typedef ParamList ParamList;

    typedef typename ParamList::TypeAt< 0 >::NonStrict< NullType >::Result Param1;
    typedef typename ParamList::TypeAt< 1 >::NonStrict< NullType >::Result Param2;
    typedef typename ParamList::TypeAt< 2 >::NonStrict< NullType >::Result Param3;
    typedef typename ParamList::TypeAt< 3 >::NonStrict< NullType >::Result Param4;
    typedef typename ParamList::TypeAt< 4 >::NonStrict< NullType >::Result Param5;
    typedef typename ParamList::TypeAt< 5 >::NonStrict< NullType >::Result Param6;
    typedef typename ParamList::TypeAt< 6 >::NonStrict< NullType >::Result Param7;
    typedef typename ParamList::TypeAt< 7 >::NonStrict< NullType >::Result Param8;
    typedef typename ParamList::TypeAt< 8 >::NonStrict< NullType >::Result Param9;
    typedef typename ParamList::TypeAt< 9 >::NonStrict< NullType >::Result Param10;

    Functor()
        : mImpl( 0 )
    {
    }

    Functor( const Functor& functor )
        : mImpl( functor.mImpl->Clone() )
    {
    }

    Functor( FunctorImpl< ResultType, ParamList >::Result* impl )
        : mImpl( impl )
    {
    }

    template< typename T >
    Functor( const T& t )
        : mImpl( new FunctorHandler< Functor, T >( t ) )
    {
    }

    template< typename Class, typename Function >
    Functor( ForMemberFunctionImpl< Class, Function > member )
        : mImpl( new MemberFunctionHandler< Functor, Class, Function >( member.m_Class, member.m_Function ) )
    {
    }

    ~Functor()
    {
        delete mImpl;
    }


    Functor& operator=( const Functor& functor )
    {
        delete mImpl;
        mImpl = functor.mImpl->Clone();
        return *this;
    }


    ResultType operator()()
    {
        return ( *mImpl )();
    }

    ResultType operator()( const Param1& p1 )
    {
        return ( *mImpl )( const_cast< Param1& >( p1 ) );
    }

    ResultType operator()( const Param1& p1, const Param2& p2 )
    {
        return ( *mImpl )( const_cast< Param1& >( p1 ), const_cast< Param2& >( p2 ) );
    }

    ResultType operator()( const Param1& p1, const Param2& p2, const Param3& p3 )
    {
        return ( *mImpl )( const_cast< Param1& >( p1 ), const_cast< Param2& >( p2 ), const_cast< Param3& >( p3 ) );
    }

private:
    FunctorImpl< ResultType, ParamList >::Result* mImpl;
};*/

} // templates
} // base
} // yake

#endif // YAKE_BASE_TEMPLATES_FUNCTOR_H
