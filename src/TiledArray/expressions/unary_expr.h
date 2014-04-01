/*
 *  This file is a part of TiledArray.
 *  Copyright (C) 2013  Virginia Tech
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
 *  Justus Calvin
 *  Department of Chemistry, Virginia Tech
 *
 *  unary_expr.h
 *  Apr 1, 2014
 *
 */

#ifndef TILEDARRAY_EXPRESSIONS_UNARY_EXPR_H__INCLUDED
#define TILEDARRAY_EXPRESSIONS_UNARY_EXPR_H__INCLUDED

#include <TiledArray/expressions/expr.h>
#include <TiledArray/dist_eval/unary_eval.h>

namespace TiledArray {
  namespace expressions {

    template <typename Derived>
    class UnaryExpr : public Expr<Derived> {
    public:
      typedef typename Derived::argument_type argument_type; ///< The expression type

    private:

      argument_type arg_; ///< The argument expression

    public:
      /// Constructor

      /// \param arg The argument expression
      UnaryExpr(const argument_type& arg) : arg_(arg) { }


      /// Copy constructor

      /// \param other The expression to be copied
      UnaryExpr(const UnaryExpr<Derived>& other) : arg_(other.arg_) { }

      /// Argument expression accessor

      /// \return A const reference to the argument expression object
      const argument_type& arg() const { return arg_; }

    }; // class UnaryExpr

  }  // namespace expressions
} // namespace TiledArray

#endif // TILEDARRAY_EXPRESSIONS_UNARY_EXPR_H__INCLUDED
