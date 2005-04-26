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
#ifndef YAKE_BASE_TEMPLATES_REGISTRY_H
#define YAKE_BASE_TEMPLATES_REGISTRY_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#include "../yakePrerequisites.h"
#endif
// Yake
#include <yake/base/yakeException.h>
#include <yake/base/templates/yakeSmartAssert.h>
#include <yake/base/templates/yakePointer.h>
#include <yake/base/templates/yakeManager.h>
#include <yake/base/mpl/yakeBuildArguments.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace templates
{

/* Helpers */
#define FUNCTION(number) \
	YAKE_TYPES_ONE_FREE(number) \
	SharedPtr<T> create(typename T::RegistryType::Id id, YAKE_ARGUMENTS_ONE_FREE(number)) \
	{ return T::getRegistry().getObject(id)->create(YAKE_USE_ARGUMENTS(number)); }
YAKE_IMPLEMENT_FUNCTION(FUNCTION)
#undef FUNCTION

#define FUNCTION(number) \
		YAKE_TYPES_ONE_FREE(number) \
		SharedPtr<T> create_default(YAKE_ARGUMENTS_ONE_FREE(number)) \
		{ return T::getRegistry().getDefaultCreator()->create(YAKE_USE_ARGUMENTS(number)); }
YAKE_IMPLEMENT_FUNCTION(FUNCTION)
#undef FUNCTION

/* Registry */
// todo: functors instead of creators
// todo: if we cannot simplify manager template, remove it
template <class ConfigClass>
class Registry : public Manager< typename ConfigClass::Id, SharedPtr< typename ConfigClass::ICreator >, RegisterFunctionsNames >
{
YAKE_DECLARE_CLASS( yake::base::templates::Registry )
public: // types
  typedef ConfigClass Config;
  typedef typename Config::Base Base;
  typedef typename Config::Id Id;
  typedef typename Config::ICreator ICreator;
 
public: // constructors
	Registry() : m_pDefaultCreator() {}

private: // no copy
  Registry( const Registry & );
  Registry & operator=( const Registry & );

public: // methods
#define FUNCTION( number ) \
	YAKE_TYPES( number ) \
	SharedPtr< Base > create( Id id, YAKE_ARGUMENTS_ONE_FREE( number ) ) \
	{ return getObject( id )->create( YAKE_USE_ARGUMENTS( number ) ); }
YAKE_IMPLEMENT_FUNCTION( FUNCTION )
#undef FUNCTION

	SharedPtr< ICreator > getDefaultCreator()
	{
		YAKE_ASSERT( getIdentifiers().size() > 0 ).debug( "No default creator available." ); // todo: does not work in release mode, should throw exception
		if( !m_pDefaultCreator ) m_pDefaultCreator = getObject( *getIdentifiers().begin() ); // todo: (if(!m_pDefaultCreator) m_pDefaultCreator = front();
		return m_pDefaultCreator;
	}

	void setDefaultCreator( const SharedPtr< ICreator > pCreator )
	{	m_pDefaultCreator = pCreator; }

private: // data
	SharedPtr< ICreator > m_pDefaultCreator;
};


// Note: there already has to be a registry configuration class called RegistryConfig.
// Note: this macro has to be used inside the class declaration.
#define YAKE_DECLARE_REGISTRY() \
  public: \
    typedef yake::base::templates::Registry< RegistryConfig > RegistryType; \
    static RegistryType& getRegistry();

// Note: you have to use DECLARE_REGISTRY() before.
#define YAKE_IMPLEMENT_REGISTRY( Class ) \
  Class::RegistryType& Class::getRegistry() \
  { \
    static RegistryType* registry_s = 0; \
    if( !registry_s ) registry_s = new RegistryType; \
    return *registry_s; \
  }


// Note: this macro has to be used inside the class declaration.
#define YAKE_DECLARE_CONCRETE( Concrete, id ) \
  public: \
    static RegistryType::Id getId() \
    { \
        return id; \
    } \
    static void Register() \
    { \
			getRegistry().doRegister( getId(), yake::SharedPtr< RegistryConfig::ConcreteCreator< Concrete > >( new RegistryConfig::ConcreteCreator< Concrete > ) ); \
    }

// 			getRegistry().doAdd( getId(), yake::SharedPtr< RegistryConfig::ConcreteCreator< Concrete > >::create() ); \ todo

// Registers a concrete implementation which has been declared before.
#define YAKE_REGISTER_CONCRETE( Concrete ) \
	namespace \
	{ \
	static struct Concrete##Initor \
	{ \
		Concrete##Initor() \
		{ \
			static int counter = 0; \
			if( counter++ > 0 ) return; \
			typedef Concrete ConcreteT; \
			ConcreteT::Register(); \
		} \
	} g_##Concrete##Initor; \
	} // nameless

} // templates
} // base
} // yake

// Declares a registry for types with an empty constructor.
#define YAKE_DECLARE_REGISTRY_0(BaseClass, IdClass) \
  public: \
	  struct RegistryConfig \
	  { \
	    typedef BaseClass Base; \
	    typedef IdClass Id; \
	    struct ICreator \
	    { \
	      virtual yake::SharedPtr< Base > create() = 0; \
	    }; \
	    template <typename T> \
	    struct ConcreteCreator : public ICreator \
	    { \
	      yake::SharedPtr<Base> create() \
	      { return yake::SharedPtr<T>(new T); } \
	    }; \
	  }; \
  YAKE_DECLARE_REGISTRY()

	// 	          return yake::SharedPtr< T >::create(); \ Todo


// Declares a registry for types with a constructor that takes one parameter.
#define YAKE_DECLARE_REGISTRY_1(BaseClass, IdClass, Param1) \
  public: \
	  struct RegistryConfig \
	  { \
	    typedef BaseClass Base; \
	    typedef IdClass Id; \
	    struct ICreator \
	    { \
				virtual yake::SharedPtr<Base> create(Param1 p1) = 0; \
	    }; \
	    template <typename T> \
	    struct ConcreteCreator : public ICreator \
	    { \
	      yake::SharedPtr<Base> create(Param1 p1) \
	      { return yake::SharedPtr<T>(new T(p1)); } \
	    }; \
	  }; \
  YAKE_DECLARE_REGISTRY()

// Declares a registry for types with a constructor that takes one parameter.
#define YAKE_DECLARE_REGISTRY_01(BaseClass, IdClass, Param1) \
  public: \
	  struct RegistryConfig \
	  { \
	    typedef BaseClass Base; \
	    typedef IdClass Id; \
	    struct ICreator \
	    { \
				virtual yake::SharedPtr<Base> create() = 0; \
				virtual yake::SharedPtr<Base> create(Param1 p1) = 0; \
	    }; \
	    template <typename T> \
	    struct ConcreteCreator : public ICreator \
	    { \
	      yake::SharedPtr<Base> create() \
	      { return yake::SharedPtr<T>(new T()); } \
				\
	      yake::SharedPtr<Base> create(Param1 p1) \
	      { return yake::SharedPtr<T>(new T(p1)); } \
	    }; \
	  }; \
  YAKE_DECLARE_REGISTRY()

#endif // YAKE_BASE_TEMPLATES_REGISTRY_H
