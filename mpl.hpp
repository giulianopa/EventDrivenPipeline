#pragma once
#include "boost/mpl/fold.hpp"
#include "boost/mpl/inherit.hpp"
#include "boost/mpl/inherit_linearly.hpp"
#include "boost/mpl/insert.hpp"
#include "boost/mpl/placeholders.hpp"
#include "boost/mpl/set.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/mpl/vector_c.hpp"
#include "boost/mpl/for_each.hpp"
#include "boost/variant.hpp"


//! Collection of Meta-functions.
/*!
	\author Giuliano Pasqualotto
*/
namespace viper
{
	namespace mpl
	{
		//-------------
		// Inheritance
		//-------------

		//! Use some mpl to inherit uniquely given a parameter pack that allows duplicates.
		/*!
			\author Giuliano Pasqualotto
		*/
		template <typename ...Args>
		struct inherit_uniquely
		{
			//! Push the data types in the variadic template pack in a set, removing duplicates.
			typedef typename boost::mpl::fold<
				boost::mpl::vector<Args...>,
				boost::mpl::set0<>,
				boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
				>::type unique_bases;

			//! Inherit the class list computed in the previous step
			typedef typename boost::mpl::inherit_linearly<
				unique_bases,
				boost::mpl::inherit<boost::mpl::_1, boost::mpl::_2>
				>::type type;
		};


		//-----------
		// Sequences
		//-----------

		//! Compile-time wrapper for a generic sequence of integer.
		template<int... Is>
		struct seq { };

		//! Compile-time integer sequence generator (recursion).
		template<int N, int... Is>
		struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

		//! Compile-time integer sequence generator (base).
		template<int... Is>
		struct gen_seq<0, Is...> : seq<Is...>{};
	}
}