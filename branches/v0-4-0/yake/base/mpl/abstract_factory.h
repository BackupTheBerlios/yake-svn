/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright � 2004 The YAKE Team
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
#ifndef YAKE_BASE_MPL_ABSTRACT_FACTORY_H
#define YAKE_BASE_MPL_ABSTRACT_FACTORY_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include "yakeFunctions.h"
#include "yakeLambda.h"
#include <yake/base/mpl/sequences.h>
#include <yake/base/mpl/inherit_linear.h>
#include <yake/base/mpl/inherit_multiple.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace mpl
{

template <class AbstractProduct>
struct abstract_factory_unit
{
	virtual AbstractProduct * do_create(functions::identity<AbstractProduct>) = 0;
	virtual ~abstract_factory_unit() {}
};

template
<
	class Types,
	template <class> class FactoryUnit = abstract_factory_unit
>
struct abstract_factory : 
	inherit_multiple
	< 
		Types, 
		typename lambda::lambda< FactoryUnit<lambda::_> >::type
	>::type
{     
	typedef Types ProductList;
        
	// todo: free parameters
	template <class AbstractProduct> 
	AbstractProduct * create()
	{
		// do_create functions are declared within different namespaces ...
		FactoryUnit<AbstractProduct> & unit = *this;
		return unit.do_create(functions::identity<AbstractProduct>());
	}
};

template <class Base, class ConcreteProduct>
struct op_new_factory_unit : public Base
{
private:
	typedef typename Base::ProductList BaseProductList;
    
protected:
	typedef typename sequences::pop_front<BaseProductList>::type ProductList;
    
public:
	typedef typename sequences::front<BaseProductList>::type AbstractProduct;

	ConcreteProduct * do_create(functions::identity<AbstractProduct>)
	{
		return new ConcreteProduct;
	}
};

template
<
	class AbstractFactory,
	class Types = typename AbstractFactory::ProductList,
	template <class, class> class Creator = op_new_factory_unit
>
struct concrete_factory
	: inherit_linear
	< 
		Types, //typename boost::mpl::reverse<Types>::type, // Using fold does not require abstract and concrete product list adjustment
		typename lambda::lambda< Creator<lambda::_, lambda::_> >::type, 
		AbstractFactory
	>::type
{
	typedef typename AbstractFactory::ProductList ProductList;
	typedef Types ConcreteProductList;
};

} // mpl
} // base
} // yake

#endif // YAKE_BASE_MPL_ABSTRACT_FACTORY_H
