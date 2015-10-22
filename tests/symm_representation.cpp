/*
 *  This file is a part of TiledArray.
 *  Copyright (C) 2015  Virginia Tech
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
 *  Edward Valeev
 *  Department of Chemistry, Virginia Tech
 *
 *  symm_repreentation.cpp
 *  September 12, 2015
 *
 */

#include <random>
#include <chrono>
#include <iostream>

#include "TiledArray/symm/permutation_group.h"
#include "TiledArray/symm/representation.h"
#include "unit_test_config.h"

using TiledArray::symmetry::Representation;
using TiledArray::symmetry::PermutationGroup;
using TiledArray::symmetry::SymmetricGroup;
using TiledArray::symmetry::Permutation;

struct GroupRepresentationFixture {

  GroupRepresentationFixture() :
    generator(std::chrono::system_clock::now().time_since_epoch().count()),
    uniform_int_distribution(0,100)
  {
  }

  ~GroupRepresentationFixture() { }

  template <size_t N>
  std::array<int, N> random_index() {
    std::array<int, N> result;
    for(auto& value : result)
      value = uniform_int_distribution(generator);
    return result;
  }

  // random number generation
  std::default_random_engine generator;
  std::uniform_int_distribution<int> uniform_int_distribution;

}; // GroupRepresentationFixture

// describes operators in group PKOper generated by parity symmetry (negation operation) and complex conjugation
// isomorphic to D2 in complex plane
struct PKOper {
    enum operator_type {_i = 0, _n = 1, _cc = 2, _n_cc = 3}; // bitwise encoding; multiplication = XOR
  public:
    PKOper(operator_type t = _i) : type_(t) {}
    PKOper(const PKOper&) = default;

    static PKOper identity;
    static PKOper negate;
    static PKOper complex_conjugate;
    static PKOper negate_complex_conjugate;

    // computes *this * rhs
    PKOper operator*(const PKOper& rhs) const {
      return PKOper(static_cast<operator_type>(type_ ^ rhs.type_));
    }
    // compares *this and rhs
    bool operator==(const PKOper& rhs) const {
      return type_ == rhs.type_;
    }

  private:
    operator_type type_;
};

PKOper PKOper::identity = PKOper{_i};
PKOper PKOper::negate = PKOper{_n};
PKOper PKOper::complex_conjugate = PKOper{_cc};
PKOper PKOper::negate_complex_conjugate = PKOper{_n_cc};

namespace TiledArray{
  namespace symmetry {
    template <> PKOper identity<PKOper>() { return PKOper::identity; }
  }
}

BOOST_FIXTURE_TEST_SUITE( symm_representation_suite, GroupRepresentationFixture )

BOOST_AUTO_TEST_CASE( constructor )
{

  // representation for permutation symmetry
  // <01||23> = -<10||23> = -<01||32> = <10||32> = <23||01>* = -<23||10>* = -<32||01>* = <32||10>*
  {

    // generators are permutations (in cycle notation): (0,1), (2,3), and (0,2)(1,3)
    // the corresponding operators are negate, negate, and compl_conjugate
    std::map<Permutation, PKOper> genops;
    genops[Permutation{1,0,2,3}] = PKOper::negate;
    genops[Permutation{0,1,3,2}] = PKOper::negate;
    genops[Permutation{2,3,0,1}] = PKOper::complex_conjugate;

    Representation<PermutationGroup, PKOper> rep(genops);

    BOOST_CHECK_EQUAL(rep.order(), 8u);

    for(const auto& g_op_pair: rep.representatives()) {
        auto g = g_op_pair.first;
        auto op = g_op_pair.second;

        if (g == Permutation{0,1,2,3}) BOOST_CHECK(op == PKOper::identity);
        if (g == Permutation{1,0,2,3}) BOOST_CHECK(op == PKOper::negate);
        if (g == Permutation{0,1,3,2}) BOOST_CHECK(op == PKOper::negate);
        if (g == Permutation{1,0,3,2}) BOOST_CHECK(op == PKOper::identity);
        if (g == Permutation{2,3,0,1}) BOOST_CHECK(op == PKOper::complex_conjugate);
        if (g == Permutation{2,3,1,0}) BOOST_CHECK(op == PKOper::negate_complex_conjugate);
        if (g == Permutation{3,2,0,1}) BOOST_CHECK(op == PKOper::negate_complex_conjugate);
        if (g == Permutation{3,2,1,0}) BOOST_CHECK(op == PKOper::complex_conjugate);
    }
  }

}

BOOST_AUTO_TEST_SUITE_END()