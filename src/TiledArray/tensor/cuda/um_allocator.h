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
 *  Eduard Valeyev
 *  Department of Chemistry, Virginia Tech
 *  Jan 31, 2018
 *
 */

#ifndef TILEDARRAY_TENSOR_CUDA_UM_ALLOCATOR_H___INCLUDED
#define TILEDARRAY_TENSOR_CUDA_UM_ALLOCATOR_H___INCLUDED

#include <TiledArray/config.h>

#ifdef TILEDARRAY_HAS_CUDA

#include <cuda_runtime.h>

#include <memory>
#include <stdexcept>

namespace TiledArray {

/// CUDA UM allocator, based on boilerplate by Howard Hinnant
/// (https://howardhinnant.github.io/allocator_boilerplate.html)
template <class T>
class cuda_um_allocator_impl {
 public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;

  cuda_um_allocator_impl() noexcept {}

  template <class U>
  cuda_um_allocator_impl(const cuda_um_allocator_impl<U>&) noexcept {}

  value_type* allocate(size_t n) {
    value_type* result = nullptr;

    cudaError_t error =
        cudaMallocManaged(&result, n * sizeof(T), cudaMemAttachGlobal);

    if (error != cudaSuccess) {
      throw std::bad_alloc();
    }

    return result;
  }

  void deallocate(value_type* ptr, size_t) {
    cudaError_t error = cudaFree(ptr);

    if (error != cudaSuccess) {
      throw std::bad_alloc();
    }
  }
};  // class cuda_um_allocator

template <class T1, class T2>
bool operator==(const cuda_um_allocator_impl<T1>&,
                const cuda_um_allocator_impl<T2>&) noexcept {
  return true;
}

template <class T1, class T2>
bool operator!=(const cuda_um_allocator_impl<T1>& lhs,
                const cuda_um_allocator_impl<T2>& rhs) noexcept {
  return !(lhs == rhs);
}

/// see
/// https://stackoverflow.com/questions/21028299/is-this-behavior-of-vectorresizesize-type-n-under-c11-and-boost-container/21028912#21028912
template <typename T, typename A = std::allocator<T>>
class default_init_allocator : public A {
  using a_t = std::allocator_traits<A>;

 public:
  using reference = typename A::reference;  // std::allocator<T>::reference
                                            // deprecated in C++17, but thrust
                                            // still relying on this
  using const_reference = typename A::const_reference;  // ditto

  template <typename U>
  struct rebind {
    using other =
        default_init_allocator<U, typename a_t::template rebind_alloc<U>>;
  };

  using A::A;

  template <typename U>
  void construct(U* ptr) noexcept(
      std::is_nothrow_default_constructible<U>::value) {
    ::new (static_cast<void*>(ptr)) U;
  }
  template <typename U, typename... Args>
  void construct(U* ptr, Args&&... args) {
    a_t::construct(static_cast<A&>(*this), ptr, std::forward<Args>(args)...);
  }
};

template <typename T>
using cuda_um_allocator = default_init_allocator<T, cuda_um_allocator_impl<T>>;

}  // namespace TiledArray

#endif  // TILEDARRAY_HAS_CUDA

#endif  // TILEDARRAY_TENSOR_CUDA_UM_ALLOCATOR_H___INCLUDED