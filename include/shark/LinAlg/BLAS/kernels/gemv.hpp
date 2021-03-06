/*!
 *  \author O. Krause
 *  \date 2012
 *
 *  \par Copyright (c) 1998-2011:
 *      Institut f&uuml;r Neuroinformatik<BR>
 *      Ruhr-Universit&auml;t Bochum<BR>
 *      D-44780 Bochum, Germany<BR>
 *      Phone: +49-234-32-25558<BR>
 *      Fax:   +49-234-32-14209<BR>
 *      eMail: Shark-admin@neuroinformatik.ruhr-uni-bochum.de<BR>
 *      www:   http://www.neuroinformatik.ruhr-uni-bochum.de<BR>
 *      <BR>
 *
 *
 *  <BR><HR>
 *  This file is part of Shark. This library is free software;
 *  you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software
 *  Foundation; either version 3, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef SHARK_LINALG_BLAS_UBLAS_KERNELS_GEMV_HPP
#define SHARK_LINALG_BLAS_UBLAS_KERNELS_GEMV_HPP

#include "default/gemv.hpp"

#ifdef SHARK_USE_ATLAS
#include "atlas/gemv.hpp"
#else
// if no bindings are included, we have to provide the default has_optimized_gemv 
// otherwise the binding will take care of this
namespace shark { namespace blas { namespace bindings{
template<class M1, class M2, class M3>
struct  has_optimized_gemv
: public boost::mpl::false_{};
}}}
#endif
	
namespace shark { namespace blas {namespace kernels{
	
///\brief Well known GEneral Matrix-Vector product kernel M+=alpha*E1*e2.
///
/// If bindings are included and the matrix/vector combination allows for a specific binding
/// to be applied, the binding is called automatically from {binding}/gemv.h
/// otherwise default/gemv.h is used which is fully implemented for all dense/sparse combinations.
/// if a combination is optimized, bindings::has_optimized_gemv<M,E1,E2>::type evaluates to boost::mpl::true_
/// The kernels themselves are implemented in blas::bindings::gemv.
template<class M, class E1, class E2>
void gemv(
	matrix_expression<E1> const& e1,
	vector_expression<E2> const& e2,
	vector_expression<M>& m,
	typename M::value_type alpha
) {
	SIZE_CHECK(m().size() == e1().size1());
	SIZE_CHECK(e1().size2() == e2().size());
	
	bindings::gemv(
		e1, e2, m,alpha,
		typename bindings::has_optimized_gemv<M,E1,E2>::type()
	);
}

}}}
#endif