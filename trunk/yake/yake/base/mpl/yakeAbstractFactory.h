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
#ifndef YAKE_BASE_MPL_ABSTRACTFACTORY_H
#define YAKE_BASE_MPL_ABSTRACTFACTORY_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include "yakeFunctions.h"
#include "yakeLambda.h"
#include <yake/base/mpl/sequences.h>

#include "yakeInheritLinear.h"
#include "yakeInheritMultiple.h"

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace mpl
{

template< class AbstractProduct_ >
class AbstractFactoryUnit
{
public:
	virtual AbstractProduct_* doCreate( functions::identity< AbstractProduct_ > ) = 0;
	virtual ~AbstractFactoryUnit() {}
};

template
<
	class Types_,
	template <class> class FactoryUnit_ = AbstractFactoryUnit
>
class AbstractFactory : 
	public InheritMultiple
	< 
		Types_, 
		typename lambda::lambda< FactoryUnit_< lambda::_ > >::type
	>::type
{     
public:
	typedef Types_ ProductList;
        
	// todo: free parameters
	template <class AbstractProduct_> AbstractProduct_* create()
	{
		// DoCreate functions are declared within different namespaces ...
		FactoryUnit_< AbstractProduct_ >& unit = *this;
		return unit.doCreate( functions::identity< AbstractProduct_ >() );
	}
};

template< class Base_, class ConcreteProduct_ >
class OpNewFactoryUnit : public Base_
{
	typedef typename Base_::ProductList BaseProductList;
    
protected:
	typedef typename sequences::pop_front< BaseProductList >::type ProductList;
    
public:
	typedef typename sequences::front< BaseProductList >::type AbstractProduct;

	ConcreteProduct_* doCreate( functions::identity< AbstractProduct > )
	{
		return new ConcreteProduct_;
	}
};

template
<
	class AbstractFactory_,
	template <class, class> class Creator_ = OpNewFactoryUnit,
	class Types_ = typename AbstractFactory_::ProductList
>
class ConcreteFactory
	: public InheritLinear
	< 
		Types_, //typename boost::mpl::reverse< Types_ >::type, // Using fold does not require abstract and concrete product list adjustment
		typename lambda::lambda< Creator_< lambda::_, lambda::_ > >::type, 
		AbstractFactory_ 
	>::type
{
public:
	typedef typename AbstractFactory_::ProductList ProductList;
	typedef Types_ ConcreteProductList;
};

} // mpl
} // base
} // yake

#endif // YAKE_BASE_MPL_ABSTRACTFACTORY_H
