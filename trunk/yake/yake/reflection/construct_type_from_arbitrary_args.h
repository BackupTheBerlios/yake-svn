#ifndef _CONSTRUCT_TYPE_FROM_ARBITRARY_ARGS_H_
#define _CONSTRUCT_TYPE_FROM_ARBITRARY_ARGS_H_

#include <boost/mpl/if.hpp>

// todo: use yake empty class
struct null {};

// meta functions: accepts any number of arguments and returns the according type selected by the holder
template< template<int, typename, typename, typename> class holder, typename T1 = null, typename T2 = null, typename T3 = null>
struct construct_type_from_arbitrary_args
{
	// type dispatching according to the number of arguments
	typedef typename boost::mpl::if_
	< 
		boost::is_same<T1, null>, 
		typename holder<0, T1, T2, T3>::type,
		typename boost::mpl::if_
		< 
			boost::is_same<T2, null>, 
			typename holder<1, T1, T2, T3>::type,
			typename boost::mpl::if_
			< 
				boost::is_same<T3, null>, 
				typename holder<2, T1, T2, T3>::type,
				typename holder<3, T1, T2, T3>::type
			>::type
		>::type
	>::type type;
};

#endif // _CONSTRUCT_TYPE_FROM_ARBITRARY_ARGS_H_