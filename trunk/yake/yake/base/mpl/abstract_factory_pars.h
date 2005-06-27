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
#ifndef YAKE_BASE_MPL_ABSTRACT_FACTORY_H
#define YAKE_BASE_MPL_ABSTRACT_FACTORY_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// yake
#include "yakeFunctions.h"
#include "yakeLambda.h"
#include <yake/base/mpl/sequences.h>
#include <yake/base/mpl/inherit_linear.h>
#include <yake/base/mpl/inherit_multiple.h>
#include <yake/base/mpl/null_type.h>
// boost
#include <boost/mpl/size.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/static_assert.hpp>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace mpl
{
// ---------------------------------------------------------
// factory unit with any arguments which are down casted by the concrete impl
// todo: im concrete casten
template <class AbstractProduct>
struct abstract_factory_unit_any
{
	/*virtual AbstractProduct * do_create(functions::identity<AbstractProduct>, boost::any) = 0;
	virtual AbstractProduct * do_create(functions::identity<AbstractProduct>, boost::any, boost::any) = 0;
	virtual AbstractProduct * do_create(functions::identity<AbstractProduct>, boost::any, boost::any, boost::any) = 0;*/
};

// ---------------------------------------------------------
// factory unit with a predefined set of constructor arguments
namespace // unnamed
{
	template <class AbstractProduct, class Parameters, int ParametersSize = boost::mpl::size<Parameters>::value>
	struct do_create_function; // throw compile-time error

	template <class AbstractProduct, class Parameters>
	struct do_create_function<AbstractProduct, Parameters, 0>
	{
		virtual AbstractProduct * do_create(functions::identity<AbstractProduct>) = 0;
	};

	template <class AbstractProduct, class Parameters>
	struct do_create_function<AbstractProduct, Parameters, 1>
	{
		virtual AbstractProduct * do_create(functions::identity<AbstractProduct>, 
		 	typename boost::mpl::at<Parameters, boost::mpl::int_<0> >::type) = 0;
	};

	template <class AbstractProduct, class Parameters>
	struct do_create_function<AbstractProduct, Parameters, 2>
	{
		virtual AbstractProduct * do_create(functions::identity<AbstractProduct>,
			typename boost::mpl::at<Parameters, boost::mpl::int_<0> >::type, 
			typename boost::mpl::at<Parameters, boost::mpl::int_<1> >::type) = 0;
	};

} // namespace unnamed

template <class AbstractProduct, class ParametersFunction>
struct abstract_factory_unit_pars : do_create_function<AbstractProduct, typename ParametersFunction::type>
{
	BOOST_STATIC_ASSERT((boost::mpl::is_sequence<typename ParametersFunction::type>::type::value == true));
	virtual ~abstract_factory_unit_pars() {}
};

// ---------------------------------------------------------
// factory unit without arguments at all
template <class AbstractProduct, class ParametersFunction = null_type>
struct abstract_factory_unit_pure
{
	virtual AbstractProduct * do_create(functions::identity<AbstractProduct>) = 0;
	virtual ~abstract_factory_unit_pure() {}
};

// ---------------------------------------------------------
// abstract factory with constructor arguments
template <class Products, class Product, class Parameters>
struct find_product_parameters
{
	typedef typename boost::mpl::at_c
		<
			Parameters,
			boost::mpl::distance
			<
				typename boost::mpl::begin<Products>::type, 
				typename boost::mpl::find<Products, Product>::type
			>::value
		>::type type;
};

/*
	abstract_factory
	<
		list<product1, product2>, // abstract products
		abstract_factory_unit_pars, // creator declaration
		list // lists of constructor arguments for each abstract product
		<
		  list<int>,
			list<bool>
		>
	>
*/
template
<
	class Types,
	template <class, class> class FactoryUnit = abstract_factory_unit_pure,
	class Parameters = null_type
>
struct abstract_factory : 
	inherit_multiple
	< 
		Types, 
		typename lambda::lambda
		< 
			FactoryUnit
			<
				lambda::_1,
				find_product_parameters<Types, lambda::_1, Parameters> // note: find_product_parameters<...>::type would 'execute' the function with _1 instead of Product, this happens later in FactoryUnit
			>
		>::type
	>::type
{     
	typedef Types ProductList;
	typedef Parameters ParameterList;
        
	template <class AbstractProduct> 
	AbstractProduct * create()
	{
		// DoCreate functions are declared within different namespaces ...
		FactoryUnit
		<
			AbstractProduct, 
			typename find_product_parameters<Types, AbstractProduct, Parameters>::type
		> & unit = *this;
		return unit.do_create(functions::identity<AbstractProduct>());
	}

	template <class AbstractProduct, typename P1> 
	AbstractProduct * create(P1 p1)
	{
		// do_create functions are declared within different namespaces ...
		FactoryUnit
		<
			AbstractProduct, 
			typename find_product_parameters<Types, AbstractProduct, Parameters>::type
		> & unit = *this;
		return unit.do_create(functions::identity<AbstractProduct>(), p1);
	}

	template <class AbstractProduct, typename P1, typename P2> 
	AbstractProduct * create(P1 p1, P2 p2)
	{
		// do_create functions are declared within different namespaces ...
		FactoryUnit
		<
			AbstractProduct, 
			typename find_product_parameters<Types, AbstractProduct, Parameters>::type // todo: shouldn't this be find_product_parameters<Types, AbstractProduct, Parameters> like above?!
		> & unit = *this;
		return unit.do_create(functions::identity<AbstractProduct>(), p1, p2);
	}
};

// ---------------------------------------------------------
// abstract factory (partial specialization) without constructor arguments
template
<
	class Types,
	template <class, class> class FactoryUnit
>
struct abstract_factory<Types, FactoryUnit, null_type> : 
	inherit_multiple
	< 
		Types, 
		typename lambda::lambda< FactoryUnit<lambda::_, null_type> >::type
	>::type
{     
	typedef Types ProductList;
	typedef null_type ParameterList;
        
	// todo: free parameters
	template <class AbstractProduct> 
	AbstractProduct * create()
	{
		// DoCreate functions are declared within different namespaces ...
		FactoryUnit<AbstractProduct> & unit = *this;
		return unit.do_create(functions::identity<AbstractProduct>());
	}
};

// ---------------------------------------------------------
// factory unit creator policy with constructor arguments
namespace // unnamed
{
	template 
	<
		class Base, 
		class ConcreteProduct,
		int ParametersSize = boost::mpl::size< typename boost::mpl::front<typename Base::ParameterList>::type >::type::value
	>
	struct op_new_factory_unit_pars_helper; // throw compile-time error

	template <class Base, class ConcreteProduct>
	struct op_new_factory_unit_pars_helper<Base, ConcreteProduct, 0> : Base
	{
	private:
		typedef typename Base::ProductList BaseProductList;
		typedef typename Base::ParameterList BaseParameterList;
    
	public:
		typedef typename sequences::pop_front<BaseProductList>::type ProductList;
		typedef typename sequences::pop_front<BaseParameterList>::type ParameterList;
    
		typedef typename sequences::front<BaseProductList>::type AbstractProduct;
		typedef typename sequences::front<BaseParameterList>::type Parameters;

		ConcreteProduct * do_create(functions::identity<AbstractProduct>)
		{ return new ConcreteProduct; }
	};

	template <class Base, class ConcreteProduct>
	struct op_new_factory_unit_pars_helper<Base, ConcreteProduct, 1> : Base
	{
	private:
		typedef typename Base::ProductList BaseProductList;
		typedef typename Base::ParameterList BaseParameterList;
    
	public:
		typedef typename sequences::pop_front<BaseProductList>::type ProductList;
		typedef typename sequences::pop_front<BaseParameterList>::type ParameterList;
    
		typedef typename sequences::front<BaseProductList>::type AbstractProduct;
		typedef typename sequences::front<BaseParameterList>::type Parameters;

		ConcreteProduct * do_create(functions::identity<AbstractProduct>, 
			typename boost::mpl::at<Parameters, boost::mpl::int_<0> >::type p1)
		{ return new ConcreteProduct(p1); }
	};

	template <class Base, class ConcreteProduct>
	struct op_new_factory_unit_pars_helper<Base, ConcreteProduct, 2> : Base
	{
	private:
		typedef typename Base::ProductList BaseProductList;
		typedef typename Base::ParameterList BaseParameterList;
	    
	public:
		typedef typename sequences::pop_front<BaseProductList>::type ProductList;
		typedef typename sequences::pop_front<BaseParameterList>::type ParameterList;
    
		typedef typename sequences::front<BaseProductList>::type AbstractProduct;
		typedef typename sequences::front<BaseParameterList>::type Parameters;

		ConcreteProduct * do_create(functions::identity<AbstractProduct>,
			typename boost::mpl::at<Parameters, boost::mpl::int_<0> >::type p1,
			typename boost::mpl::at<Parameters, boost::mpl::int_<1> >::type p2)
		{ return new ConcreteProduct(p1, p2); }
	};

} // namespace unnamed

// ---------------------------------------------------------
// factory unit creator policy without constructor arguments
struct op_new_factory_unit
{

	template <class Base,	class ConcreteProduct>
	struct with_pars : op_new_factory_unit_pars_helper<Base, ConcreteProduct>
	{};

	template <class Base, class ConcreteProduct>
	struct without_pars : Base
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
};

// ---------------------------------------------------------
// concrete factory with constructor parameters
template
<
	class AbstractFactory,
	class Types = typename AbstractFactory::ProductList,
	class Creator = op_new_factory_unit,
	class ParametersCheck = typename AbstractFactory::ParameterList
>
struct concrete_factory
	: inherit_linear
	< 
		Types, //typename boost::mpl::reverse<Types>::type, // Using fold does not require abstract and concrete product list adjustment
		typename lambda::lambda< typename Creator::with_pars<lambda::_, lambda::_> >::type, 
		AbstractFactory
	>::type
{
	typedef typename AbstractFactory::ProductList ProductList;
	typedef Types ConcreteProductList;
};

// ---------------------------------------------------------
// concrete factory (partial specialization) without constructor parameters
template
<
	class AbstractFactory,
	class Types,
	class Creator
>
struct concrete_factory<AbstractFactory, Types, Creator, null_type>
	: inherit_linear
	< 
		Types, //typename boost::mpl::reverse<Types>::type, // Using fold does not require abstract and concrete product list adjustment
		typename lambda::lambda< typename Creator::without_pars<lambda::_, lambda::_> >::type, 
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
