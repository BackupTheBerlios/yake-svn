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
#ifndef YAKE_BASE_MPL_INHERIT_MULTIPLE_H
#define YAKE_BASE_MPL_INHERIT_MULTIPLE_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <boost/mpl/apply.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/lambda.hpp>

#include "yakeNullType.h"
#include "yakeFunctions.h"

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace mpl
{
namespace detail
{

// The product of GenScatterHierarchy will always be one of these,
// with leaves being the classes in the type list.
template <class L, class R> 
struct inherit_two : public L, public R
{
  typedef L Left;
  typedef R Right;
};

// F is the transformation that will be bound into
// this function object and applied to each element;
// think of this as a parameterized function adaptor.
template <class F> 
struct inherit_transformed
{
    // This is the function object's operator()()
    template <class Product, class Element> 
    struct apply
    {
        // first transform the type
				typedef typename functions::apply<F, Element>::type transformed_type;
				// OR typedef typename F::apply< Element >::type transformed_type;

        // This is the return value: we inherit from the
        // current element and everything accumulated so far
        typedef inherit_two<transformed_type, Product> type;
    };
};

} // detail

// Now we can define GenScatterHierarchy
template <class Sequence, class Transformation> 
struct inherit_multiple 
	: algorithms::fold
	< 
			Sequence, 
			null_type, 
			detail::inherit_transformed<Transformation> 
	>
{};

} // mpl
} // base
} // yake

#endif // YAKE_BASE_MPL_INHERIT_MULTIPLE_H
