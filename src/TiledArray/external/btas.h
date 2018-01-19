/*
 *  This file is a part of TiledArray.
 *  Copyright (C) 2018  Virginia Tech
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  btas.h
 *  Jul 11, 2017
 *
 */

#ifndef TILEDARRAY_MATH_BTAS_H__INCLUDED
#define TILEDARRAY_MATH_BTAS_H__INCLUDED

#include "TiledArray/config.h"

#include <btas/features.h>
#include <btas/tensor.h>
#include <btas/generic/permute.h>
#include <btas/generic/axpy_impl.h>

#include <madness/world/archive.h>

namespace TiledArray {
/// Computes the result of applying permutation \c perm to \c arg
template<typename T, typename Range, typename Storage>
btas::Tensor<T, Range, Storage> permute(
    const btas::Tensor<T, Range, Storage>& arg,
    const TiledArray::Permutation& perm) {
  btas::Tensor<T, Range, Storage> result;
  std::vector<size_t> p(perm.dim());
  std::copy(perm.begin(), perm.end(), p.begin());
  btas::permute(arg, p, result);
  return result;
}

/// result[i] += arg[i]
template<typename T, typename Range, typename Storage>
void add_to(btas::Tensor<T, Range, Storage>& result,
            const btas::Tensor<T, Range, Storage>& arg) {
  btas::axpy(1.0, arg, result);
}


/// result[i] = arg1[i] * arg2[i]
template<typename T, typename Range, typename Storage>
btas::Tensor<T, Range, Storage> mult(
    const btas::Tensor<T, Range, Storage>& arg1,
    const btas::Tensor<T, Range, Storage>& arg2) {
  assert(false);
}

/// result[perm ^ i] = arg1[i] * arg2[i]
template<typename T, typename Range, typename Storage>
btas::Tensor<T, Range, Storage> mult(
    const btas::Tensor<T, Range, Storage>& arg1,
    const btas::Tensor<T, Range, Storage>& arg2,
    const TiledArray::Permutation& perm) {
  assert(false);
}

/// result[i] *= arg[i]
template<typename T, typename Range, typename Storage>
btas::Tensor<T, Range, Storage>&
mult_to(btas::Tensor<T, Range, Storage>& result,
        const btas::Tensor<T, Range, Storage>& arg) {
  assert(false);
}

template<typename T, typename Range, typename Storage>
btas::Tensor<T, Range, Storage> gemm(
    const btas::Tensor<T, Range, Storage>& left,
    const btas::Tensor<T, Range, Storage>& right, T factor,
    const TiledArray::math::GemmHelper& gemm_helper) {
  // Check that the arguments are not empty and have the correct ranks
  TA_ASSERT(!left.empty());
  TA_ASSERT(left.range().rank() == gemm_helper.left_rank());
  TA_ASSERT(!right.empty());
  TA_ASSERT(right.range().rank() == gemm_helper.right_rank());

  // Construct the result Tensor
  typedef btas::Tensor<T, Range, Storage> Tensor;
  Tensor result(gemm_helper.make_result_range<Range>(left.range(), right.range()));

  // Check that the inner dimensions of left and right match
  TA_ASSERT(gemm_helper.left_right_congruent(std::cbegin(left.range().lobound()),
                                            std::cbegin(right.range().lobound())));
  TA_ASSERT(gemm_helper.left_right_congruent(std::cbegin(left.range().upbound()),
                                            std::cbegin(right.range().upbound())));
  TA_ASSERT(gemm_helper.left_right_congruent(std::cbegin(left.range().extent()),
                                            std::cbegin(right.range().extent())));

  // Compute gemm dimensions
  integer m = 1, n = 1, k = 1;
  gemm_helper.compute_matrix_sizes(m, n, k, left.range(), right.range());

  // Get the leading dimension for left and right matrices.
  const integer lda = (gemm_helper.left_op() == madness::cblas::NoTrans ? k : m);
  const integer ldb = (gemm_helper.right_op() == madness::cblas::NoTrans ? n : k);

  TiledArray::math::gemm(gemm_helper.left_op(), gemm_helper.right_op(), m, n, k, factor,
                         left.data(), lda, right.data(), ldb, T(0), result.data(), n);

  return result;
}

template<typename T, typename Range, typename Storage>
void gemm(btas::Tensor<T, Range, Storage>& result,
          const btas::Tensor<T, Range, Storage>& left,
          const btas::Tensor<T, Range, Storage>& right, T factor,
          const TiledArray::math::GemmHelper& gemm_helper) {
  // Check that this tensor is not empty and has the correct rank
  TA_ASSERT(!result.empty());
  TA_ASSERT(result.range().rank() == gemm_helper.result_rank());

  // Check that the arguments are not empty and have the correct ranks
  TA_ASSERT(!left.empty());
  TA_ASSERT(left.range().rank() == gemm_helper.left_rank());
  TA_ASSERT(!right.empty());
  TA_ASSERT(right.range().rank() == gemm_helper.right_rank());

  // Check that the outer dimensions of left match the the corresponding
  // dimensions in result
  TA_ASSERT(gemm_helper.left_result_congruent(std::cbegin(left.range().lobound()),
                                             std::cbegin(result.range().lobound())));
  TA_ASSERT(gemm_helper.left_result_congruent(std::cbegin(left.range().upbound()),
                                             std::cbegin(result.range().upbound())));
  TA_ASSERT(gemm_helper.left_result_congruent(std::cbegin(left.range().extent()),
                                             std::cbegin(result.range().extent())));

  // Check that the outer dimensions of right match the the corresponding
  // dimensions in result
  TA_ASSERT(gemm_helper.right_result_congruent(std::cbegin(right.range().lobound()),
                                              std::cbegin(result.range().lobound())));
  TA_ASSERT(gemm_helper.right_result_congruent(std::cbegin(right.range().upbound()),
                                              std::cbegin(result.range().upbound())));
  TA_ASSERT(gemm_helper.right_result_congruent(std::cbegin(right.range().extent()),
                                              std::cbegin(result.range().extent())));

  // Check that the inner dimensions of left and right match
  TA_ASSERT(gemm_helper.left_right_congruent(std::cbegin(left.range().lobound()),
                                            std::cbegin(right.range().lobound())));
  TA_ASSERT(gemm_helper.left_right_congruent(std::cbegin(left.range().upbound()),
                                            std::cbegin(right.range().upbound())));
  TA_ASSERT(gemm_helper.left_right_congruent(std::cbegin(left.range().extent()),
                                            std::cbegin(right.range().extent())));

  // Compute gemm dimensions
  integer m, n, k;
  gemm_helper.compute_matrix_sizes(m, n, k, left.range(), right.range());

  // Get the leading dimension for left and right matrices.
  const integer lda =
      (gemm_helper.left_op() == madness::cblas::NoTrans ? k : m);
  const integer ldb =
      (gemm_helper.right_op() == madness::cblas::NoTrans ? n : k);

  TiledArray::math::gemm(gemm_helper.left_op(), gemm_helper.right_op(), m, n, k, factor,
                         left.data(), lda, right.data(), ldb, T(1), result.data(), n);
}

template<typename Perm>
TiledArray::Range permute(const TiledArray::Range& r, const Perm& p) {
  TiledArray::Permutation pp(p.begin(), p.end());
  return pp * r;
}

//
// these are not used, but still must be declared for expressions to work
//
template<typename T, typename Range, typename Storage>
typename btas::Tensor<T, Range, Storage>::value_type
trace(const btas::Tensor<T, Range, Storage>& arg);
// foreach(i) result += arg[i]
template<typename T, typename Range, typename Storage>
typename btas::Tensor<T, Range, Storage>::value_type
sum(const btas::Tensor<T, Range, Storage>& arg);
// foreach(i) result *= arg[i]
template<typename T, typename Range, typename Storage>
typename btas::Tensor<T, Range, Storage>::value_type
product(const btas::Tensor<T, Range, Storage>& arg);
// foreach(i) result += arg[i] * arg[i]
template<typename T, typename Range, typename Storage>
typename btas::Tensor<T, Range, Storage>::value_type
squared_norm(const btas::Tensor<T, Range, Storage>& arg);
// sqrt(squared_norm(arg))
template<typename T, typename Range, typename Storage>
typename btas::Tensor<T, Range, Storage>::value_type
norm(const btas::Tensor<T, Range, Storage>& arg);
// foreach(i) result = max(result, arg[i])
template<typename T, typename Range, typename Storage>
typename btas::Tensor<T, Range, Storage>::value_type
max(const btas::Tensor<T, Range, Storage>& arg);
// foreach(i) result = min(result, arg[i])
template<typename T, typename Range, typename Storage>
typename btas::Tensor<T, Range, Storage>::value_type
min(const btas::Tensor<T, Range, Storage>& arg);
// foreach(i) result = max(result, abs(arg[i]))
template<typename T, typename Range, typename Storage>
typename btas::Tensor<T, Range, Storage>::value_type
abs_max(const btas::Tensor<T, Range, Storage>& arg);
// foreach(i) result = max(result, abs(arg[i]))
template<typename T, typename Range, typename Storage>
typename btas::Tensor<T, Range, Storage>::value_type
abs_min(const btas::Tensor<T, Range, Storage>& arg);
}  // namespace TiledArray

namespace TiledArray {
namespace detail {

template <typename T, typename ... Args>
struct is_tensor_helper<btas::Tensor<T, Args...> > : public std::true_type { };

template <typename T, typename ... Args>
struct is_contiguous_tensor_helper<btas::Tensor<T, Args...> > : public std::true_type { };

}
}

/// implement conversions from btas::Tensor to TA::Tensor
namespace TiledArray {
  template <typename T, typename Allocator, typename ... Args>
  struct Cast<TiledArray::Tensor<T, Allocator>, btas::Tensor<T, Args...>> {
    auto operator()(const btas::Tensor<T, Args...>& arg) const {
      TiledArray::Tensor<T> result(arg.range());
      std::copy(btas::cbegin(arg), btas::cend(arg), begin(result));
      return result;
    }
  };
}

namespace madness {
  namespace archive {

#ifdef BTAS_HAS_BOOST_CONTAINER
  template <class Archive, typename T, std::size_t N, typename A>
  struct ArchiveLoadImpl<Archive, boost::container::small_vector<T, N, A>> {
    static inline void load(const Archive& ar,
                            boost::container::small_vector<T, N, A>& x) {
      std::size_t n{};
      ar& n;
      x.resize(n);
      for (auto& xi : x) ar& xi;
    }
  };

  template <class Archive, typename T, std::size_t N, typename A>
  struct ArchiveStoreImpl<Archive, boost::container::small_vector<T, N, A>> {
    static inline void store(const Archive& ar,
                            const boost::container::small_vector<T, N, A>& x) {
      ar& x.size();
      for (const auto& xi : x) ar & xi;
    }
  };
#endif

  template <class Archive, typename T>
  struct ArchiveLoadImpl<Archive, btas::varray<T>> {
    static inline void load(const Archive& ar, btas::varray<T>& x) {
      typename btas::varray<T>::size_type n{};
      ar& n;
      x.resize(n);
      for (typename btas::varray<T>::value_type& xi : x) ar& xi;
    }
    };

    template<class Archive, typename T>
    struct ArchiveStoreImpl<Archive, btas::varray<T> > {
        static inline void store(const Archive& ar, const btas::varray<T>& x) {
          ar & x.size();
          for (const typename btas::varray<T>::value_type& xi : x)
            ar & xi;
        }
    };

    template <class Archive, CBLAS_ORDER _Order, typename _Index>
    struct ArchiveLoadImpl<Archive, btas::BoxOrdinal<_Order, _Index>> {
      static inline void load(const Archive& ar,
                              btas::BoxOrdinal<_Order, _Index>& o) {
        typename btas::BoxOrdinal<_Order, _Index>::stride_type stride{};
        typename btas::BoxOrdinal<_Order, _Index>::value_type offset{};
        bool cont{};
        ar& stride& offset& cont;
        o = btas::BoxOrdinal<_Order, _Index>(
            std::move(stride), std::move(offset), std::move(cont));
      }
    };

    template <class Archive, CBLAS_ORDER _Order, typename _Index>
    struct ArchiveStoreImpl<Archive, btas::BoxOrdinal<_Order, _Index>> {
      static inline void store(const Archive& ar,
                               const btas::BoxOrdinal<_Order, _Index>& o) {
        ar& o.stride() & o.offset() & o.contiguous();
      }
    };

    template <class Archive, CBLAS_ORDER _Order, typename _Index,
              typename _Ordinal>
    struct ArchiveLoadImpl<Archive, btas::RangeNd<_Order, _Index, _Ordinal>> {
      static inline void load(const Archive& ar,
                              btas::RangeNd<_Order, _Index, _Ordinal>& r) {
        typedef typename btas::BaseRangeNd<
            btas::RangeNd<_Order, _Index, _Ordinal>>::index_type index_type;
        index_type lobound{}, upbound{};
        _Ordinal ordinal{};
        ar& lobound& upbound& ordinal;
        r = btas::RangeNd<_Order, _Index, _Ordinal>(
            std::move(lobound), std::move(upbound), std::move(ordinal));
      }
    };

    template <class Archive, CBLAS_ORDER _Order, typename _Index,
              typename _Ordinal>
    struct ArchiveStoreImpl<Archive, btas::RangeNd<_Order, _Index, _Ordinal>> {
      static inline void store(
          const Archive& ar, const btas::RangeNd<_Order, _Index, _Ordinal>& r) {
        ar & r.lobound() & r.upbound() & r.ordinal();
      }
    };

    template <class Archive, typename _T, class _Range, class _Store>
    struct ArchiveLoadImpl<Archive, btas::Tensor<_T, _Range, _Store>> {
      static inline void load(const Archive& ar,
                              btas::Tensor<_T, _Range, _Store>& t) {
        _Range range{};
        _Store store{};
        ar& range& store;
        t = btas::Tensor<_T, _Range, _Store>(std::move(range),
                                             std::move(store));
      }
    };

    template <class Archive, typename _T, class _Range, class _Store>
    struct ArchiveStoreImpl<Archive, btas::Tensor<_T, _Range, _Store>> {
      static inline void store(const Archive& ar,
                              const btas::Tensor<_T, _Range, _Store>& t) {
        ar & t.range() & t.storage();
      }
    };
  }
}



#endif  /* TILEDARRAY_MATH_BTAS_H__INCLUDED */
