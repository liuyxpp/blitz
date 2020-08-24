// -*- C++ -*-
/***************************************************************************
 * blitz/array/expr.h     Array<T,N> expression templates
 *
 * $Id$
 *
 * Copyright (C) 1997-2011 Todd Veldhuizen <tveldhui@acm.org>
 *
 * This file is a part of Blitz.
 *
 * Blitz is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Blitz is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with Blitz.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Suggestions:          blitz-devel@lists.sourceforge.net
 * Bugs:                 blitz-support@lists.sourceforge.net    
 *
 * For more information, please see the Blitz++ Home Page:
 *    https://sourceforge.net/projects/blitz/
 *
 ****************************************************************************/
#ifndef BZ_ARRAYEXPR_H
#define BZ_ARRAYEXPR_H

#ifndef BZ_ARRAY_H
 #error <blitz/array/expr.h> must be included via <blitz/array.h>
#endif

#include <blitz/ops.h>
#include <blitz/prettyprint.h>
#include <blitz/shapecheck.h>
#include <blitz/numinquire.h>

/*
 * The array expression templates iterator interface is followed by
 * these classes:
 *
 * FastArrayIterator          <blitz/array/fastiter.h>
 * _bz_ArrayExpr              <blitz/array/expr.h>
 * _bz_ArrayExprUnaryOp               "
 * _bz_ArrayExprBinaryOp              "
 * _bz_ArrayExprTernaryOp             "
 * _bz_ArrayExprConstant              "
 * ArrayIndexMapping          <blitz/array/map.h>
 * _bz_ArrayExprReduce        <blitz/array/reduce.h>
 * _bz_StencilExpr            <blitz/array/stencil-et.h>
 *   ... and derived types            "
 * IndexPlaceholder           <blitz/indexexpr.h>
 * _bz_ArrayWhere             <blitz/array/where.h>
 * _bz_FunctorExpr            <blitz/array/functorExpr.h>
 * _bz_FunctorExpr2                   "
 * _bz_FunctorExpr3                   "
 */

BZ_NAMESPACE(blitz)

template<typename T1, typename T2>
class _bz_ExprPair {
public:
    _bz_ExprPair(const T1& a, const T2& b)
        : first_(a), second_(b)
    { }

    const T1& first() const
    { return first_; }

    const T2& second() const
    { return second_; }

protected:
    T1 first_;
    T2 second_;
};

template<typename T1, typename T2>
inline _bz_ExprPair<T1,T2> makeExprPair(const T1& a, const T2& b)
{
    return _bz_ExprPair<T1,T2>(a,b);
}

template<typename P_expr>
class _bz_ArrayExpr 
#ifdef BZ_NEW_EXPRESSION_TEMPLATES
    : public ETBase<_bz_ArrayExpr<P_expr> >
#endif
{

public:
    typedef P_expr T_expr;
    typedef _bz_typename T_expr::T_numtype T_numtype;
    typedef T_expr T_ctorArg1;
    typedef int    T_ctorArg2;    // dummy
  typedef _bz_ArrayExpr<_bz_typename P_expr::T_range_result> T_range_result;

    static const int 
        numArrayOperands = T_expr::numArrayOperands,
        numIndexPlaceholders = T_expr::numIndexPlaceholders,
        rank = T_expr::rank;

    _bz_ArrayExpr(const _bz_ArrayExpr<T_expr>& a)
#ifdef BZ_NEW_EXPRESSION_TEMPLATES
        : ETBase< _bz_ArrayExpr<T_expr> >(a), iter_(a.iter_)
#else
        : iter_(a.iter_)
#endif
    { }

#if defined(BZ_NEW_EXPRESSION_TEMPLATES) && ! defined(__MWERKS__)
    template<typename T>
    _bz_ArrayExpr(const T& a)
        : iter_(a)
    { }
#else

    _bz_ArrayExpr(BZ_ETPARM(T_expr) a)
        : iter_(a)
    { }
#if !defined(__MWERKS__)
    _bz_ArrayExpr(BZ_ETPARM(_bz_typename T_expr::T_ctorArg1) a)
        : iter_(a)
    { }
#endif
#endif

    template<typename T1, typename T2>
    _bz_ArrayExpr(BZ_ETPARM(T1) a, BZ_ETPARM(T2) b)
        : iter_(a, b)
    { }

    template<typename T1, typename T2, typename T3>
    _bz_ArrayExpr(BZ_ETPARM(T1) a, BZ_ETPARM(T2) b, BZ_ETPARM(T3) c)
        : iter_(a, b, c)
    { }

    template<typename T1, typename T2, typename T3, typename T4>
    _bz_ArrayExpr(BZ_ETPARM(T1) a, BZ_ETPARM(T2) b, BZ_ETPARM(T3) c,
        BZ_ETPARM(T4) d) : iter_(a, b, c, d)
    { }

    template<typename T1, typename T2>
    _bz_ArrayExpr(const _bz_ExprPair<T1,T2>& exprpair)
        : iter_(exprpair.first(), exprpair.second())
    { }

    T_numtype operator*() const { return *iter_; }

    T_numtype first_value() const { return iter_(iter_.lbound()); }

#ifdef BZ_ARRAY_EXPR_PASS_INDEX_BY_VALUE
    template<int N_rank>
    T_numtype operator()(const TinyVector<int, N_rank> i) const { return iter_(i); }
#else
    template<int N_rank>
    T_numtype operator()(const TinyVector<int, N_rank>& i) const { return iter_(i); }
#endif

  template<int N>
  T_range_result operator()(const RectDomain<N>& d) const
  {
    return T_range_result(iter_(d));
  }

    int ascending(const int rank) const { return iter_.ascending(rank); }
    int ordering(const int rank)  const { return iter_.ordering(rank);  }
    int lbound(const int rank)    const { return iter_.lbound(rank);    }
    int ubound(const int rank)    const { return iter_.ubound(rank);    }
    RectDomain<rank> domain() const { return iter_.domain(); }

    void push(int position)
    { iter_.push(position); }

    void pop(int position)
    { iter_.pop(position); }

    void advance()
    { iter_.advance(); }

    void advance(int n)
    { iter_.advance(n); }

    void loadStride(int rank)
    { iter_.loadStride(rank); }

    bool isUnitStride(int rank) const
    { return iter_.isUnitStride(rank); }

    void advanceUnitStride()
    { iter_.advanceUnitStride(); }

    bool canCollapse(int outerLoopRank, int innerLoopRank) const
    { 
        // BZ_DEBUG_MESSAGE("_bz_ArrayExpr<>::canCollapse()");
        return iter_.canCollapse(outerLoopRank, innerLoopRank); 
    }

    T_numtype operator[](int i) const
    { return iter_[i]; }

    T_numtype fastRead(int i) const
    { return iter_.fastRead(i); }

    // this is needed for the stencil expression fastRead to work
    void _bz_offsetData(sizeType i)
    { iter_._bz_offsetData(i); }

    // and these are needed for stencil expression shift to work
    void _bz_offsetData(sizeType offset, int dim)
    { iter_._bz_offsetData(offset, dim);}
  
    void _bz_offsetData(sizeType offset1, int dim1, sizeType offset2, int dim2)
    { iter_._bz_offsetData(offset1, dim1, offset2, dim2);}

    diffType suggestStride(int rank) const
    { return iter_.suggestStride(rank); }

    bool isStride(int rank, diffType stride) const
    { return iter_.isStride(rank,stride); }

    void prettyPrint(BZ_STD_SCOPE(string) &str) const
    {
        prettyPrintFormat format(true);  // Terse formatting by default
        iter_.prettyPrint(str, format);
    }

    void prettyPrint(BZ_STD_SCOPE(string) &str, 
        prettyPrintFormat& format) const
    { iter_.prettyPrint(str, format); }

    template<typename T_shape>
    bool shapeCheck(const T_shape& shape) const
    { return iter_.shapeCheck(shape); }

    template<int N>
    void moveTo(const TinyVector<int, N>& i)
    {
        iter_.moveTo(i);
    }

    T_numtype shift(int offset, int dim) const
    {
      return iter_.shift(offset, dim);
    }

    T_numtype shift(int offset1, int dim1,int offset2, int dim2) const
    {
      return iter_.shift(offset1, dim1, offset2, dim2);
    }

  // sliceinfo for expressions
  template<typename T1, typename T2 = nilArraySection, 
	   class T3 = nilArraySection, typename T4 = nilArraySection, 
	   class T5 = nilArraySection, typename T6 = nilArraySection, 
	   class T7 = nilArraySection, typename T8 = nilArraySection, 
	   class T9 = nilArraySection, typename T10 = nilArraySection, 
	   class T11 = nilArraySection>
  class SliceInfo {
  public:
    typedef typename T_expr::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_subexpr;
    typedef _bz_ArrayExpr<T_subexpr> T_slice;
};

  // slicing (experimental)

  // because _bz_ArrayExpr is the "top-level" expression class, it has
  // the burden of supplying enough variants of the operator to make
  // it user-friendly. Hence the large numbers that follow
  template<typename T1>
  typename SliceInfo<T1>::T_slice
  operator()(T1 r1) const
  {
    return typename SliceInfo<T1>::T_slice
      (iter_
       (r1, 
	nilArraySection(), nilArraySection(), nilArraySection(), nilArraySection(),
	nilArraySection(), nilArraySection(), nilArraySection(),
	nilArraySection(), nilArraySection(), nilArraySection()));
  }

  template<typename T1, typename T2>
  typename SliceInfo<T1,T2>::T_slice
  operator()(T1 r1, T2 r2) const
  {
    typedef typename SliceInfo<T1,T2>::T_slice slice;
    return slice(iter_
		 (r1, r2, nilArraySection(), nilArraySection(), nilArraySection(),
		  nilArraySection(), nilArraySection(), nilArraySection(),
		  nilArraySection(), nilArraySection(), nilArraySection()));
  }


    template<typename T1, typename T2, typename T3>
    typename SliceInfo<T1,T2,T3>::T_slice 
    operator()(T1 r1, T2 r2, T3 r3) const
    {
        typedef typename SliceInfo<T1,T2,T3>::T_slice slice;
        return slice(iter_(r1, r2, r3, nilArraySection(), nilArraySection(), 
			   nilArraySection(), nilArraySection(), nilArraySection(), 
			   nilArraySection(), nilArraySection(), nilArraySection()));
    }

    template<typename T1, typename T2, typename T3, typename T4>
    typename SliceInfo<T1,T2,T3,T4>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4) const
    {
        typedef typename SliceInfo<T1,T2,T3,T4>::T_slice slice;
        return slice(iter_(r1, r2, r3, r4, nilArraySection(), nilArraySection(),
			   nilArraySection(), nilArraySection(), nilArraySection(),
			   nilArraySection(), nilArraySection()));
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    typename SliceInfo<T1,T2,T3,T4,T5>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5) const
    {
        typedef typename SliceInfo<T1,T2,T3,T4,T5>::T_slice slice;
        return slice(iter_(r1, r2, r3, r4, r5, nilArraySection(),
			   nilArraySection(), nilArraySection(), nilArraySection(),
			   nilArraySection(), nilArraySection()));
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    typename SliceInfo<T1,T2,T3,T4,T5,T6>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6) const
    {
        typedef typename SliceInfo<T1,T2,T3,T4,T5,T6>::T_slice slice;
        return slice(iter_(r1, r2, r3, r4, r5, r6, nilArraySection(), 
			   nilArraySection(), nilArraySection(),
			   nilArraySection(), nilArraySection()));
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
        typename T7>
    typename SliceInfo<T1,T2,T3,T4,T5,T6,T7>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7) const
    {
        typedef typename SliceInfo<T1,T2,T3,T4,T5,T6,T7>::T_slice slice;
        return slice(iter_(r1, r2, r3, r4, r5, r6, r7, 
			   nilArraySection(), nilArraySection(),
			   nilArraySection(), nilArraySection()));
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
        typename T7, typename T8>
    typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8) const
    {
        typedef typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8>::T_slice slice;
        return slice(iter_(r1, r2, r3, r4, r5, r6, r7, r8,
			   nilArraySection(), nilArraySection(), nilArraySection()));
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
        typename T7, typename T8, typename T9>
    typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8, T9 r9) const
    {
        typedef typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9>::T_slice slice;
        return slice(iter_(r1, r2, r3, r4, r5, r6, r7, r8, r9, 
			   nilArraySection(), nilArraySection()));
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
        typename T7, typename T8, typename T9, typename T10>
    typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8, T9 r9, T10 r10) const
    {
        typedef typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::T_slice slice;
        return slice(iter_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, 
			   nilArraySection()));
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
        typename T7, typename T8, typename T9, typename T10, typename T11>
    typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8, T9 r9, T10 r10, T11 r11) const
    {
        typedef typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice slice;
        return slice(iter_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11));
    }

  // now complete slicings to a scalar, which can't be expressed with the above
  // unlike for arrays, these expressions are rvalues so we return by value
    T_numtype operator()(int i0) const
    { 
	return iter_(TinyVector<int, 1>(i0));
    }

  T_numtype operator()(int i0, int i1) const
    { 
	return iter_(TinyVector<int, 2>(i0, i1));
    }

  T_numtype operator()(int i0, int i1, int i2) const
    { 
      return iter_(TinyVector<int, 3>(i0, i1, i2));
    }

  T_numtype operator()(int i0, int i1, int i2, int i3) const
    { 
      return iter_(TinyVector<int, 4>(i0, i1, i2, i3));
    }

  T_numtype operator()(int i0, int i1, int i2, int i3, int i4) const
    { 
      return iter_(TinyVector<int, 5>(i0, i1, i2, i3, i4));
    }

  T_numtype operator()(int i0, int i1, int i2, int i3, int i4, int i5) const
    { 
      return iter_(TinyVector<int, 6>(i0, i1, i2, i3, i4, i5));
    }

  T_numtype operator()(int i0, int i1, int i2, int i3, int i4, int i5,
		       int i6) const
    { 
      return iter_(TinyVector<int, 7>(i0, i1, i2, i3, i4, i5, i6));
    }

  T_numtype operator()(int i0, int i1, int i2, int i3, int i4, int i5,
		       int i6, int i7) const
    { 
      return iter_(TinyVector<int, 8>(i0, i1, i2, i3, i4, i5, i6, i7));
    }

  T_numtype operator()(int i0, int i1, int i2, int i3, int i4, int i5,
		       int i6, int i7, int i8) const
    { 
      return iter_(TinyVector<int, 9>(i0, i1, i2, i3, i4, i5, i6, i7, i8));
    }

  T_numtype operator()(int i0, int i1, int i2, int i3, int i4, int i5,
		       int i6, int i7, int i8, int i9) const
    { 
      return iter_(TinyVector<int, 10>(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9));
    }

  T_numtype operator()(int i0, int i1, int i2, int i3, int i4, int i5,
		       int i6, int i7, int i8, int i9, int i10) const
    { 
      return iter_(TinyVector<int, 11>(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10));
    }

protected:
    _bz_ArrayExpr() { }

    T_expr iter_;
};

struct bounds {
    static int compute_ascending(const int BZ_DEBUG_PARAM(rank),
                                 const int ascending1, const int ascending2)
    {
        // The value INT_MIN indicates that there are no arrays
        // in a subtree of the expression.  This logic returns
        // whichever ascending is available.  If there are two
        // conflicting ascending values, this is an error.

        if (ascending1 == ascending2)
            return ascending1;
        else if (ascending1 == INT_MIN)
            return ascending2;
        else if (ascending2 == INT_MIN)
            return ascending1;

        BZ_DEBUG_MESSAGE("Two array operands have different"
            << endl << "ascending flags: for rank " << rank 
            << ", the flags are " << ascending1 << " and " 
            << ascending2 << endl);
        BZ_PRE_FAIL;
        return 0;
    }

    static int compute_ordering(const int BZ_DEBUG_PARAM(rank),
                                const int order1, const int order2)
    {
        // The value INT_MIN indicates that there are no arrays
        // in a subtree of the expression.  This logic returns
        // whichever ordering is available.  If there are two
        // conflicting ordering values, this is an error.

        if (order1 == order2)
            return order1;
        else if (order1 == INT_MIN)
            return order2;
        else if (order2 == INT_MIN)
            return order1;

        BZ_DEBUG_MESSAGE("Two array operands have different"
            << endl << "orders: for rank " << rank << ", the orders are "
            << order1 << " and " << order2 << endl);
        BZ_PRE_FAIL;
        return 0;
    }

    static int compute_lbound(const int BZ_DEBUG_PARAM(rank),
                              const int lbound1, const int lbound2)
    {
        // The value INT_MIN indicates that there are no arrays
        // in a subtree of the expression.  This logic returns
        // whichever lbound is available.  If there are two
        // conflicting lbound values, this is an error.

        if (lbound1 == lbound2)
            return lbound1;
        else if (lbound1 == INT_MIN)
            return lbound2;
        else if (lbound2 == INT_MIN)
            return lbound1;

        BZ_DEBUG_MESSAGE("Two array operands have different"
            << endl << "lower bounds: in rank " << rank << ", the bounds are "
            << lbound1 << " and " << lbound2 << endl);
        BZ_PRE_FAIL;
        return 0;
    }

    static int compute_ubound(const int BZ_DEBUG_PARAM(rank),
                              const int ubound1, const int ubound2)
    {
        // The value INT_MAX indicates that there are no arrays
        // in a subtree of the expression.  This logic returns
        // whichever ubound is available.  If there are two
        // conflicting ubound values, this is an error.

        if (ubound1 == ubound2)
            return ubound1;
        else if (ubound1 == INT_MAX)
            return ubound2;
        else if (ubound2 == INT_MAX)
            return ubound1;

        BZ_DEBUG_MESSAGE("Two array operands have different"
            << endl << "upper bounds: in rank " << rank << ", the bounds are "
            << ubound1 << " and " << ubound2 << endl);
        BZ_PRE_FAIL;
        return 0;
    }
};

template<typename P_expr, typename P_op>
class _bz_ArrayExprUnaryOp {
public:
    typedef P_expr T_expr;
    typedef P_op T_op;
    typedef _bz_typename T_expr::T_numtype T_numtype1;
    typedef _bz_typename T_op::T_numtype T_numtype;
    typedef T_expr T_ctorArg1;
    typedef int    T_ctorArg2;    // dummy
  typedef _bz_ArrayExprUnaryOp<_bz_typename P_expr::T_range_result,
			       P_op> T_range_result;

    static const int 
        numArrayOperands = T_expr::numArrayOperands,
        numIndexPlaceholders = T_expr::numIndexPlaceholders,
        rank = T_expr::rank;

    _bz_ArrayExprUnaryOp(const _bz_ArrayExprUnaryOp<T_expr, T_op>& a)
        : iter_(a.iter_)
    { }

    _bz_ArrayExprUnaryOp(BZ_ETPARM(T_expr) a)
        : iter_(a)
    { }

    _bz_ArrayExprUnaryOp(_bz_typename T_expr::T_ctorArg1 a)
        : iter_(a)
    { }

#if BZ_TEMPLATE_CTOR_DOESNT_CAUSE_HAVOC
    template<typename T1>
    explicit _bz_ArrayExprUnaryOp(BZ_ETPARM(T1) a)
        : iter_(a)
    { }
#endif

    int ascending(const int rank) const { return iter_.ascending(rank); }
    int ordering(const int rank)  const { return iter_.ordering(rank);  }
    int lbound(const int rank)    const { return iter_.lbound(rank);    }
    int ubound(const int rank)    const { return iter_.ubound(rank);    }
    RectDomain<rank> domain() const { return iter_.domain(); }

    T_numtype operator*() const { return T_op::apply(*iter_); }

    T_numtype first_value() const { return iter_(iter_.lbound()); }

#ifdef BZ_ARRAY_EXPR_PASS_INDEX_BY_VALUE
    template<int N_rank>
    T_numtype operator()(const TinyVector<int, N_rank> i) const
    { return T_op::apply(iter_(i)); }
#else
    template<int N_rank>
    T_numtype operator()(const TinyVector<int, N_rank>& i) const
    { return T_op::apply(iter_(i)); }
#endif

  template<int N>
  T_range_result operator()(const RectDomain<N>& d) const
  {
    return T_range_result(iter_(d));
  }

    void push(int position)
    {
        iter_.push(position);
    }

    void pop(int position)
    {
        iter_.pop(position);
    }

    void advance()
    {
        iter_.advance();
    }

    void advance(int n)
    {
        iter_.advance(n);
    }

    void loadStride(int rank)
    {
        iter_.loadStride(rank);
    }

    bool isUnitStride(int rank) const
    { return iter_.isUnitStride(rank); }

    void advanceUnitStride()
    {
        iter_.advanceUnitStride();
    }

  template<int N>
  void moveTo(const TinyVector<int,N>& i)
    {
        iter_.moveTo(i);
    }

    T_numtype shift(int offset, int dim) const
    {
      return T_op::apply(iter_.shift(offset, dim));
    }

    T_numtype shift(int offset1, int dim1,int offset2, int dim2) const
    {
      return T_op::apply(iter_.shift(offset1, dim1, offset2, dim2));
    }

    bool canCollapse(int outerLoopRank, int innerLoopRank) const
    { 
        // BZ_DEBUG_MESSAGE("_bz_ArrayExprUnaryOp<>::canCollapse");
        return iter_.canCollapse(outerLoopRank, innerLoopRank); 
    }

    T_numtype operator[](int i) const
    { return T_op::apply(iter_[i]); }

    T_numtype fastRead(int i) const
    { return T_op::apply(iter_.fastRead(i)); }

  // this is needed for the stencil expression fastRead to work
  void _bz_offsetData(sizeType i)
  {
    iter_._bz_offsetData(i);
  }

    // and these are needed for stencil expression shift to work
    void _bz_offsetData(sizeType offset, int dim)
    { iter_._bz_offsetData(offset, dim);}
  
    void _bz_offsetData(sizeType offset1, int dim1, sizeType offset2, int dim2)
    { iter_._bz_offsetData(offset1, dim1, offset2, dim2);}

    diffType suggestStride(int rank) const
    { return iter_.suggestStride(rank); }

    bool isStride(int rank, diffType stride) const
    { return iter_.isStride(rank,stride); }

    void prettyPrint(BZ_STD_SCOPE(string) &str, 
        prettyPrintFormat& format) const
    { T_op::prettyPrint(str, format, iter_); }

    template<typename T_shape>
    bool shapeCheck(const T_shape& shape) const
    { return iter_.shapeCheck(shape); }


  // sliceinfo for expressions
  template<typename T1, typename T2 = nilArraySection, 
	   class T3 = nilArraySection, typename T4 = nilArraySection, 
	   class T5 = nilArraySection, typename T6 = nilArraySection, 
	   class T7 = nilArraySection, typename T8 = nilArraySection, 
	   class T9 = nilArraySection, typename T10 = nilArraySection, 
	   class T11 = nilArraySection>
  class SliceInfo {
  public:
    typedef typename T_expr::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_slice1;
    typedef _bz_ArrayExprUnaryOp<T_slice1, T_op> T_slice;
};

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
        typename T7, typename T8, typename T9, typename T10, typename T11>
    typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8, T9 r9, T10 r10, T11 r11) const
    {
      return typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice
	(iter_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11));
    }

protected:
    _bz_ArrayExprUnaryOp() { }

    T_expr iter_;
};


template<typename P_expr1, typename P_expr2, typename P_op>

class _bz_ArrayExprBinaryOp {
public:
    typedef P_expr1 T_expr1;
    typedef P_expr2 T_expr2;
    typedef P_op T_op;
    typedef _bz_typename T_expr1::T_numtype T_numtype1;
    typedef _bz_typename T_expr2::T_numtype T_numtype2;
    typedef _bz_typename T_op::T_numtype T_numtype;
    typedef T_expr1 T_ctorArg1;
    typedef T_expr2 T_ctorArg2;
  typedef _bz_ArrayExprBinaryOp<_bz_typename P_expr1::T_range_result, 
				_bz_typename P_expr2::T_range_result, 
				P_op> T_range_result;

    static const int 
        numArrayOperands = T_expr1::numArrayOperands
                         + T_expr2::numArrayOperands,
        numIndexPlaceholders = T_expr1::numIndexPlaceholders
                             + T_expr2::numIndexPlaceholders,
        rank = (T_expr1::rank > T_expr2::rank) 
             ? T_expr1::rank : T_expr2::rank;

    _bz_ArrayExprBinaryOp(
        const _bz_ArrayExprBinaryOp<T_expr1, T_expr2, T_op>& a)
        : iter1_(a.iter1_), iter2_(a.iter2_)
    { }

    template<typename T1, typename T2>
    _bz_ArrayExprBinaryOp(BZ_ETPARM(T1) a, BZ_ETPARM(T2) b)
        : iter1_(a), iter2_(b)
    { }

    T_numtype operator*() const
    { return T_op::apply(*iter1_, *iter2_); }

#ifdef BZ_ARRAY_EXPR_PASS_INDEX_BY_VALUE
    template<int N_rank>
    T_numtype operator()(const TinyVector<int, N_rank> i) const
  { return T_op::apply(iter1_(i), iter2_(i)); }
#else
    template<int N_rank>
    T_numtype operator()(const TinyVector<int, N_rank>& i) const
  { return T_op::apply(iter1_(i), iter2_(i)); }
#endif

  template<int N>
  T_range_result operator()(const RectDomain<N>& d) const
  {
    return T_range_result(iter1_(d), iter2_(d));
  }

    int ascending(const int rank) const {
        return bounds::compute_ascending(rank, iter1_.ascending(rank), iter2_.ascending(rank));
    }

    int ordering(const int rank) const {
        return bounds::compute_ordering(rank, iter1_.ordering(rank), iter2_.ordering(rank));
    }

    int lbound(const int rank) const { 
        return bounds::compute_lbound(rank, iter1_.lbound(rank), iter2_.lbound(rank));
    }

    int ubound(const int rank) const {
        return bounds::compute_ubound(rank, iter1_.ubound(rank), iter2_.ubound(rank));
    }

  // defer calculation to lbound/ubound
  RectDomain<rank> domain() const 
  { 
    TinyVector<int, rank> lb, ub;
    for(int r=0; r<rank; ++r) {
      lb[r]=lbound(r); ub[r]=ubound(r); 
    }
    return RectDomain<rank>(lb,ub);
  }

    void push(int position)
    { 
        iter1_.push(position); 
        iter2_.push(position);
    }

    void pop(int position)
    { 
        iter1_.pop(position); 
        iter2_.pop(position);
    }

    void advance()
    { 
        iter1_.advance(); 
        iter2_.advance();
    }

    void advance(int n)
    {
        iter1_.advance(n);
        iter2_.advance(n);
    }

    void loadStride(int rank)
    { 
        iter1_.loadStride(rank); 
        iter2_.loadStride(rank);
    }
    
    bool isUnitStride(int rank) const
    { return iter1_.isUnitStride(rank) && iter2_.isUnitStride(rank); }

    void advanceUnitStride()
    { 
        iter1_.advanceUnitStride(); 
        iter2_.advanceUnitStride();
    }

    bool canCollapse(int outerLoopRank, int innerLoopRank) const
    { 
        // BZ_DEBUG_MESSAGE("_bz_ArrayExprBinaryOp<>::canCollapse");
        return iter1_.canCollapse(outerLoopRank, innerLoopRank)
            && iter2_.canCollapse(outerLoopRank, innerLoopRank);
    } 

    T_numtype operator[](int i) const
    { return T_op::apply(iter1_[i], iter2_[i]); }

    T_numtype fastRead(int i) const
    { return T_op::apply(iter1_.fastRead(i), iter2_.fastRead(i)); }

    // this is needed for the stencil expression fastRead to work
    void _bz_offsetData(sizeType i)
  {
    iter1_._bz_offsetData(i);
    iter2_._bz_offsetData(i);
  }

    // and these are needed for stencil expression shift to work
    void _bz_offsetData(sizeType offset, int dim)
    { 
      iter1_._bz_offsetData(offset, dim);
      iter2_._bz_offsetData(offset, dim);
    }
  
    void _bz_offsetData(sizeType offset1, int dim1, sizeType offset2, int dim2)
    { 
      iter1_._bz_offsetData(offset1, dim1, offset2, dim2);
      iter2_._bz_offsetData(offset1, dim1, offset2, dim2);
    }

    diffType suggestStride(int rank) const
    {
        diffType stride1 = iter1_.suggestStride(rank);
        diffType stride2 = iter2_.suggestStride(rank);
        return (stride1 > stride2) ? stride1 : stride2;
    }

    bool isStride(int rank, diffType stride) const
    {
        return iter1_.isStride(rank,stride) && iter2_.isStride(rank,stride);
    }

  template<int N>
  void moveTo(const TinyVector<int,N>& i)
    {
        iter1_.moveTo(i);
        iter2_.moveTo(i);
    }

    T_numtype shift(int offset, int dim) const
    {
      return T_op::apply(iter1_.shift(offset, dim),iter2_.shift(offset, dim));
    }

    T_numtype shift(int offset1, int dim1,int offset2, int dim2) const
    {
      return T_op::apply(iter1_.shift(offset1, dim1, offset2, dim2),
			 iter2_.shift(offset1, dim1, offset2, dim2));
    }

    void prettyPrint(BZ_STD_SCOPE(string) &str, 
        prettyPrintFormat& format) const
    {
        T_op::prettyPrint(str, format, iter1_, iter2_);
    }

    template<typename T_shape>
    bool shapeCheck(const T_shape& shape) const
    { return iter1_.shapeCheck(shape) && iter2_.shapeCheck(shape); }


  // sliceinfo for expressions
  template<typename T1, typename T2 = nilArraySection, 
	   class T3 = nilArraySection, typename T4 = nilArraySection, 
	   class T5 = nilArraySection, typename T6 = nilArraySection, 
	   class T7 = nilArraySection, typename T8 = nilArraySection, 
	   class T9 = nilArraySection, typename T10 = nilArraySection, 
	   class T11 = nilArraySection>
  class SliceInfo {
  public:
    typedef typename T_expr1::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_slice1;
    typedef typename T_expr2::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_slice2;
    typedef _bz_ArrayExprBinaryOp<T_slice1, T_slice2, T_op> T_slice;
};

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
        typename T7, typename T8, typename T9, typename T10, typename T11>
    typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8, T9 r9, T10 r10, T11 r11) const
    {
      return typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice
	(iter1_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11),
	 iter2_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11));
    }

protected:
    _bz_ArrayExprBinaryOp() { }

    T_expr1 iter1_;
    T_expr2 iter2_; 
};

template<typename P_expr1, typename P_expr2, typename P_expr3, typename P_op>
class _bz_ArrayExprTernaryOp {
public:
    typedef P_expr1 T_expr1;
    typedef P_expr2 T_expr2;
    typedef P_expr3 T_expr3;
    typedef P_op T_op;
    typedef _bz_typename T_expr1::T_numtype T_numtype1;
    typedef _bz_typename T_expr2::T_numtype T_numtype2;
    typedef _bz_typename T_expr3::T_numtype T_numtype3;
    typedef _bz_typename T_op::T_numtype T_numtype;
    typedef T_expr1 T_ctorArg1;
    typedef T_expr2 T_ctorArg2;
    typedef T_expr3 T_ctorArg3;
  typedef _bz_ArrayExprTernaryOp<_bz_typename P_expr1::T_range_result, 
				 _bz_typename P_expr2::T_range_result, 
				 _bz_typename P_expr3::T_range_result, P_op> T_range_result;

    static const int 
        numArrayOperands = T_expr1::numArrayOperands
                         + T_expr2::numArrayOperands
                         + T_expr3::numArrayOperands,
        numIndexPlaceholders = T_expr1::numIndexPlaceholders
                             + T_expr2::numIndexPlaceholders
                             + T_expr3::numIndexPlaceholders,
        rank = (T_expr1::rank > T_expr2::rank) 
             ? ((T_expr1::rank > T_expr3::rank)
                ? T_expr1::rank : T_expr3::rank)
             : ((T_expr2::rank > T_expr3::rank) 
                ? T_expr2::rank : T_expr3::rank);

    _bz_ArrayExprTernaryOp(
        const _bz_ArrayExprTernaryOp<T_expr1, T_expr2, T_expr3, T_op>& a)
        : iter1_(a.iter1_), iter2_(a.iter2_), iter3_(a.iter3_)
    { }

    template<typename T1, typename T2, typename T3>
    _bz_ArrayExprTernaryOp(BZ_ETPARM(T1) a, BZ_ETPARM(T2) b, BZ_ETPARM(T3) c)
        : iter1_(a), iter2_(b), iter3_(c)
    { }

    T_numtype operator*() const
    { return T_op::apply(*iter1_, *iter2_, *iter3_); }

#ifdef BZ_ARRAY_EXPR_PASS_INDEX_BY_VALUE
    template<int N_rank>
    T_numtype operator()(const TinyVector<int, N_rank> i) const
    { return T_op::apply(iter1_(i), iter2_(i), iter3_(i)); }
#else
    template<int N_rank>
    T_numtype operator()(const TinyVector<int, N_rank>& i) const
    { return T_op::apply(iter1_(i), iter2_(i), iter3_(i)); }
#endif

  template<int N>
  T_range_result operator()(const RectDomain<N>& d) const
  {
    return T_range_result(iter1_(d), iter2_(d), iter3_(d));
  }

    int ascending(const int rank) const {
        return bounds::compute_ascending(rank, bounds::compute_ascending(
            rank, iter1_.ascending(rank), iter2_.ascending(rank)),
            iter3_.ascending(rank));
    }

    int ordering(const int rank) const {
        return bounds::compute_ordering(rank, bounds::compute_ordering(
            rank, iter1_.ordering(rank), iter2_.ordering(rank)),
            iter3_.ordering(rank));
    }

    int lbound(const int rank) const { 
        return bounds::compute_lbound(rank, bounds::compute_lbound(
            rank, iter1_.lbound(rank), iter2_.lbound(rank)), 
            iter3_.lbound(rank));
    }

    int ubound(const int rank) const {
        return bounds::compute_ubound(rank, bounds::compute_ubound(
            rank, iter1_.ubound(rank), iter2_.ubound(rank)), 
            iter3_.ubound(rank));
    }

  // defer calculation to lbound/ubound
  RectDomain<rank> domain() const 
  { 
    TinyVector<int, rank> lb, ub;
    for(int r=0; r<rank; ++r) {
      lb[r]=lbound(r); ub[r]=ubound(r); 
    }
    return RectDomain<rank>(lb,ub);
  }

    void push(int position)
    { 
        iter1_.push(position); 
        iter2_.push(position);
        iter3_.push(position);
    }

    void pop(int position)
    { 
        iter1_.pop(position); 
        iter2_.pop(position);
        iter3_.pop(position);
    }

    void advance()
    { 
        iter1_.advance(); 
        iter2_.advance();
        iter3_.advance();
    }

    void advance(int n)
    {
        iter1_.advance(n);
        iter2_.advance(n);
        iter3_.advance(n);
    }

    void loadStride(int rank)
    { 
        iter1_.loadStride(rank); 
        iter2_.loadStride(rank);
        iter3_.loadStride(rank);
    }
    
    bool isUnitStride(int rank) const
    {
        return iter1_.isUnitStride(rank)
            && iter2_.isUnitStride(rank)
            && iter3_.isUnitStride(rank);
    }

    void advanceUnitStride()
    { 
        iter1_.advanceUnitStride(); 
        iter2_.advanceUnitStride();
        iter3_.advanceUnitStride();
    }

    bool canCollapse(int outerLoopRank, int innerLoopRank) const
    { 
        // BZ_DEBUG_MESSAGE("_bz_ArrayExprTernaryOp<>::canCollapse");
        return iter1_.canCollapse(outerLoopRank, innerLoopRank)
            && iter2_.canCollapse(outerLoopRank, innerLoopRank)
            && iter3_.canCollapse(outerLoopRank, innerLoopRank);
    } 

    T_numtype operator[](int i) const
    { return T_op::apply(iter1_[i], iter2_[i], iter3_[i]); }

    T_numtype fastRead(int i) const
    {
        return T_op::apply(iter1_.fastRead(i),
                           iter2_.fastRead(i),
                           iter3_.fastRead(i));
    }

    // this is needed for the stencil expression fastRead to work
    void _bz_offsetData(sizeType i)
    {
      iter1_._bz_offsetData(i);
      iter2_._bz_offsetData(i);
      iter3_._bz_offsetData(i);
    }

    // and these are needed for stencil expression shift to work
    void _bz_offsetData(sizeType offset, int dim)
    { 
      iter1_._bz_offsetData(offset, dim);
      iter2_._bz_offsetData(offset, dim);
      iter3_._bz_offsetData(offset, dim);
    }
  
    void _bz_offsetData(sizeType offset1, int dim1, sizeType offset2, int dim2)
    {
      iter1_._bz_offsetData(offset1, dim1, offset2, dim2);
      iter2_._bz_offsetData(offset1, dim1, offset2, dim2);
      iter3_._bz_offsetData(offset1, dim1, offset2, dim2);
    }

    diffType suggestStride(int rank) const
    {
        diffType stride1 = iter1_.suggestStride(rank);
        diffType stride2 = iter2_.suggestStride(rank);
        diffType stride3 = iter3_.suggestStride(rank);
        return stride1 > ( stride2 = (stride2>stride3 ? stride2 : stride3) ) ?
            stride1 : stride2;
    }

    bool isStride(int rank, diffType stride) const
    {
        return iter1_.isStride(rank,stride)
            && iter2_.isStride(rank,stride)
            && iter3_.isStride(rank,stride);
    }

    template<int N>
    void moveTo(const TinyVector<int,N>& i)
    {
        iter1_.moveTo(i);
        iter2_.moveTo(i);
        iter3_.moveTo(i);
    }

    void prettyPrint(BZ_STD_SCOPE(string) &str, 
        prettyPrintFormat& format) const
    {
        T_op::prettyPrint(str, format, iter1_, iter2_, iter3_);
    }

    template<typename T_shape>
    bool shapeCheck(const T_shape& shape) const
    {
        return iter1_.shapeCheck(shape)
            && iter2_.shapeCheck(shape)
            && iter3_.shapeCheck(shape);
    }


  // sliceinfo for expressions
  template<typename T1, typename T2 = nilArraySection, 
	   class T3 = nilArraySection, typename T4 = nilArraySection, 
	   class T5 = nilArraySection, typename T6 = nilArraySection, 
	   class T7 = nilArraySection, typename T8 = nilArraySection, 
	   class T9 = nilArraySection, typename T10 = nilArraySection, 
	   class T11 = nilArraySection>
  class SliceInfo {
  public:
    typedef typename T_expr1::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_slice1;
    typedef typename T_expr2::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_slice2;
    typedef typename T_expr3::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_slice3;
    typedef _bz_ArrayExprTernaryOp<T_slice1, T_slice2, T_slice3, T_op> T_slice;
};

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
        typename T7, typename T8, typename T9, typename T10, typename T11>
    typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8, T9 r9, T10 r10, T11 r11) const
    {
      return typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice
	(iter1_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11),
	 iter2_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11),
	 iter3_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11));
    }

protected:
    _bz_ArrayExprTernaryOp() { }

    T_expr1 iter1_;
    T_expr2 iter2_; 
    T_expr3 iter3_; 
};

#define BZ_MAX(a,b) (a)>(b) ? (a) : (b)

template<typename P_expr1, typename P_expr2, typename P_expr3,
	 typename P_expr4, typename P_op>
class _bz_ArrayExprQuaternaryOp {
public:
    typedef P_expr1 T_expr1;
    typedef P_expr2 T_expr2;
    typedef P_expr3 T_expr3;
    typedef P_expr4 T_expr4;
    typedef P_op T_op;
    typedef _bz_typename T_expr1::T_numtype T_numtype1;
    typedef _bz_typename T_expr2::T_numtype T_numtype2;
    typedef _bz_typename T_expr3::T_numtype T_numtype3;
    typedef _bz_typename T_expr4::T_numtype T_numtype4;
    typedef _bz_typename T_op::T_numtype T_numtype;
    typedef T_expr1 T_ctorArg1;
    typedef T_expr2 T_ctorArg2;
    typedef T_expr3 T_ctorArg3;
    typedef T_expr4 T_ctorArg4;
  typedef _bz_ArrayExprQuaternaryOp<_bz_typename P_expr1::T_range_result, 
				    _bz_typename P_expr2::T_range_result, 
				    _bz_typename P_expr3::T_range_result, 
				    _bz_typename P_expr4::T_range_result, 
				    P_op> T_range_result;

  static const int 
  numArrayOperands = T_expr1::numArrayOperands
    + T_expr2::numArrayOperands
    + T_expr3::numArrayOperands
    + T_expr4::numArrayOperands,

    numIndexPlaceholders = T_expr1::numIndexPlaceholders
    + T_expr2::numIndexPlaceholders
    + T_expr3::numIndexPlaceholders
    + T_expr4::numIndexPlaceholders,

    rank = BZ_MAX(BZ_MAX(T_expr1::rank, T_expr2::rank),
		  BZ_MAX(T_expr3::rank, T_expr4::rank));

    _bz_ArrayExprQuaternaryOp(
        const _bz_ArrayExprQuaternaryOp<T_expr1, T_expr2, T_expr3, T_expr4, T_op>& a)
        : iter1_(a.iter1_), iter2_(a.iter2_), iter3_(a.iter3_), iter4_(a.iter4_)
    { }

    template<typename T1, typename T2, typename T3, typename T4>
    _bz_ArrayExprQuaternaryOp(BZ_ETPARM(T1) a, BZ_ETPARM(T2) b, 
			      BZ_ETPARM(T3) c, BZ_ETPARM(T4) d)
        : iter1_(a), iter2_(b), iter3_(c), iter4_(d)
    { }

    T_numtype operator*() const
    { return T_op::apply(*iter1_, *iter2_, *iter3_, *iter4_); }

#ifdef BZ_ARRAY_EXPR_PASS_INDEX_BY_VALUE
    template<int N_rank>
    T_numtype operator()(const TinyVector<int, N_rank> i) const
    { return T_op::apply(iter1_(i), iter2_(i), iter3_(i), iter4_(i)); }
#else
    template<int N_rank>
    T_numtype operator()(const TinyVector<int, N_rank>& i) const
    { return T_op::apply(iter1_(i), iter2_(i), iter3_(i), iter4_(i)); }
#endif

  template<int N>
  T_range_result operator()(const RectDomain<rank>& d) const
  {
    return T_range_result(iter1_(d), iter2_(d), iter3_(d), iter4_(d));
  }

    int ascending(const int rank) const {
        return bounds::compute_ascending(rank, 
					 bounds::compute_ascending(rank, 
								   iter1_.ascending(rank), 
								   iter2_.ascending(rank)),
					 bounds::compute_ascending(rank, 
								   iter3_.ascending(rank),
								   iter4_.ascending(rank)));
    }

    int ordering(const int rank) const {
        return bounds::compute_ordering(rank, 
					bounds::compute_ordering(rank, 
								 iter1_.ordering(rank), 
								 iter2_.ordering(rank)),
					bounds::compute_ordering(rank, 
								 iter3_.ordering(rank),
								 iter4_.ordering(rank)));
    }

    int lbound(const int rank) const { 
        return bounds::compute_lbound(rank, 
				      bounds::compute_lbound(rank, 
							     iter1_.lbound(rank), 
							     iter2_.lbound(rank)),
				      bounds::compute_lbound(rank, 
							     iter3_.lbound(rank),
							     iter4_.lbound(rank)));
    }

    int ubound(const int rank) const {
        return bounds::compute_ubound(rank, 
				      bounds::compute_ubound(rank, 
							     iter1_.ubound(rank), 
							     iter2_.ubound(rank)),
				      bounds::compute_ubound(rank, 
							     iter3_.ubound(rank),
							     iter4_.ubound(rank)));
    }

  // defer calculation to lbound/ubound
  RectDomain<rank> domain() const 
  { 
    TinyVector<int, rank> lb, ub;
    for(int r=0; r<rank; ++r) {
      lb[r]=lbound(r); ub[r]=ubound(r); 
    }
    return RectDomain<rank>(lb,ub);
  }

    void push(int position)
    { 
        iter1_.push(position); 
        iter2_.push(position);
        iter3_.push(position);
        iter4_.push(position);
    }

    void pop(int position)
    { 
        iter1_.pop(position); 
        iter2_.pop(position);
        iter3_.pop(position);
        iter4_.pop(position);
    }

    void advance()
    { 
        iter1_.advance(); 
        iter2_.advance();
        iter3_.advance();
        iter4_.advance();
    }

    void advance(int n)
    {
        iter1_.advance(n);
        iter2_.advance(n);
        iter3_.advance(n);
        iter4_.advance(n);
    }

    void loadStride(int rank)
    { 
        iter1_.loadStride(rank); 
        iter2_.loadStride(rank);
        iter3_.loadStride(rank);
        iter4_.loadStride(rank);
    }
    
    bool isUnitStride(int rank) const
    {
        return iter1_.isUnitStride(rank)
            && iter2_.isUnitStride(rank)
            && iter3_.isUnitStride(rank)
            && iter4_.isUnitStride(rank);
    }

    void advanceUnitStride()
    { 
        iter1_.advanceUnitStride(); 
        iter2_.advanceUnitStride();
        iter3_.advanceUnitStride();
        iter4_.advanceUnitStride();
    }

    bool canCollapse(int outerLoopRank, int innerLoopRank) const
    { 
        // BZ_DEBUG_MESSAGE("_bz_ArrayExprQuaternaryOp<>::canCollapse");
        return iter1_.canCollapse(outerLoopRank, innerLoopRank)
            && iter2_.canCollapse(outerLoopRank, innerLoopRank)
            && iter3_.canCollapse(outerLoopRank, innerLoopRank)
            && iter4_.canCollapse(outerLoopRank, innerLoopRank);
    } 

    T_numtype operator[](int i)
    { return T_op::apply(iter1_[i], iter2_[i], iter3_[i], iter4_[i]); }

    T_numtype fastRead(int i) const
    {
        return T_op::apply(iter1_.fastRead(i),
                           iter2_.fastRead(i),
                           iter3_.fastRead(i),
                           iter4_.fastRead(i));
    }

  // this is needed for the stencil expression fastRead to work
  void _bz_offsetData(sizeType i)
  {
    iter1_._bz_offsetData(i);
    iter2_._bz_offsetData(i);
    iter3_._bz_offsetData(i);
    iter4_._bz_offsetData(i);
  }

    // and these are needed for stencil expression shift to work
    void _bz_offsetData(sizeType offset, int dim)
    { 
      iter1_._bz_offsetData(offset, dim);
      iter2_._bz_offsetData(offset, dim);
      iter3_._bz_offsetData(offset, dim);
      iter4_._bz_offsetData(offset, dim);
    }
  
    void _bz_offsetData(sizeType offset1, int dim1, sizeType offset2, int dim2)
    {
      iter1_._bz_offsetData(offset1, dim1, offset2, dim2);
      iter2_._bz_offsetData(offset1, dim1, offset2, dim2);
      iter3_._bz_offsetData(offset1, dim1, offset2, dim2);
      iter4_._bz_offsetData(offset1, dim1, offset2, dim2);
    }

    diffType suggestStride(int rank) const
    {
        diffType stride1 = iter1_.suggestStride(rank);
        diffType stride2 = iter2_.suggestStride(rank);
        diffType stride3 = iter3_.suggestStride(rank);
        diffType stride4 = iter4_.suggestStride(rank);

        //return stride1 > ( stride2 = (stride2>stride3 ? stride2 : stride3) ) ?
	//  stride1 : stride2;
	return std::max(std::max(stride1, stride2),
			std::max(stride3, stride4));
    }

    bool isStride(int rank, diffType stride) const
    {
        return iter1_.isStride(rank,stride)
            && iter2_.isStride(rank,stride)
            && iter3_.isStride(rank,stride)
            && iter4_.isStride(rank,stride);
    }

    template<int N>
    void moveTo(const TinyVector<int,N>& i)
    {
        iter1_.moveTo(i);
        iter2_.moveTo(i);
        iter3_.moveTo(i);
        iter4_.moveTo(i);
    }

    void prettyPrint(BZ_STD_SCOPE(string) &str, 
        prettyPrintFormat& format) const
    {
        T_op::prettyPrint(str, format, iter1_, iter2_, iter3_, iter4_);
    }

    template<typename T_shape>
    bool shapeCheck(const T_shape& shape) const
    {
        return iter1_.shapeCheck(shape)
            && iter2_.shapeCheck(shape)
            && iter3_.shapeCheck(shape)
            && iter4_.shapeCheck(shape);
    }


  // sliceinfo for expressions
  template<typename T1, typename T2 = nilArraySection, 
	   class T3 = nilArraySection, typename T4 = nilArraySection, 
	   class T5 = nilArraySection, typename T6 = nilArraySection, 
	   class T7 = nilArraySection, typename T8 = nilArraySection, 
	   class T9 = nilArraySection, typename T10 = nilArraySection, 
	   class T11 = nilArraySection>
  class SliceInfo {
  public:
    typedef typename T_expr1::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_slice1;
    typedef typename T_expr2::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_slice2;
    typedef typename T_expr3::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_slice3;
    typedef typename T_expr4::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_slice4;
    typedef _bz_ArrayExprQuaternaryOp<T_slice1, T_slice2, T_slice3, T_slice4, T_op> T_slice;
};

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
        typename T7, typename T8, typename T9, typename T10, typename T11>
    typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8, T9 r9, T10 r10, T11 r11) const
    {
      return typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice
	(iter1_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11),
	 iter2_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11),
	 iter3_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11),
	 iter4_(r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11));
    }

protected:
    _bz_ArrayExprQuaternaryOp() { }

    T_expr1 iter1_;
    T_expr2 iter2_; 
    T_expr3 iter3_; 
    T_expr4 iter4_; 
};

template<typename P_numtype>
class _bz_ArrayExprConstant {
public:
    typedef P_numtype T_numtype;
    typedef T_numtype T_ctorArg1;
    typedef int       T_ctorArg2;    // dummy
  typedef _bz_ArrayExprConstant<P_numtype> T_range_result;
    static const int 
        numArrayOperands = 0, 
        numIndexPlaceholders = 0, 
        rank = 0;

    _bz_ArrayExprConstant(const _bz_ArrayExprConstant<T_numtype>& a)
        : value_(a.value_)
    { }

    _bz_ArrayExprConstant(T_numtype value)
        : value_(BZ_NO_PROPAGATE(value))
    { 
    }

    // tiny() and huge() return the smallest and largest representable
    // integer values.  See <blitz/numinquire.h>
    // NEEDS_WORK: use tiny(int()) once numeric_limits<T> available on
    // all platforms

    int ascending(const int) const { return INT_MIN; }
    int ordering(const int)  const { return INT_MIN; }
    int lbound(const int)    const { return INT_MIN; }
    int ubound(const int)    const { return INT_MAX; }

  // there is no rank...
  RectDomain<10> domain() const 
  { 
    TinyVector<int, 10> lb(lbound(0)), ub(ubound(0));
    return RectDomain<rank>(lb,ub);
  }


    // NEEDS_WORK: use huge(int()) once numeric_limits<T> available on
    // all platforms

    T_numtype operator*()   const { return value_; }
    T_numtype first_value() const { return value_; }

#ifdef BZ_ARRAY_EXPR_PASS_INDEX_BY_VALUE
    template<int N_rank>
    T_numtype operator()(const TinyVector<int,N_rank>) const
    { return value_; }
#else
    template<int N_rank>
    T_numtype operator()(const TinyVector<int,N_rank>&) const
    { return value_; }
#endif

  template<int N_rank>
  const _bz_ArrayExprConstant& operator()(const RectDomain<N_rank>& d) const
  {
    return *this;
  }

    void push(int) { }
    void pop(int) { }
    void advance() { }
    void advance(int) { }
    void loadStride(int) { }

    bool isUnitStride(int) const
    { return true; }

    void advanceUnitStride()
    { }

    bool canCollapse(int,int) const 
    { return true; }

    T_numtype operator[](int) const
    { return value_; }

    T_numtype fastRead(int) const
    { return value_; }

  // this is needed for the stencil expression fastRead to work
  void _bz_offsetData(sizeType i) const{};

    // and these are needed for stencil expression shift to work
  void _bz_offsetData(sizeType offset, int dim) const {};
  
  void _bz_offsetData(sizeType offset1, int dim1, sizeType offset2, int dim2) const {};

    diffType suggestStride(int) const
    { return 1; }

    bool isStride(int,diffType) const
    { return true; }

    void moveTo(int) const { }

    T_numtype shift(int offset, int dim) const {return value_;}

    T_numtype shift(int offset1, int dim1,int offset2, int dim2) const 
    { return value_;}

    template<int N_rank>
    void moveTo(const TinyVector<int,N_rank>&) const { }

    void prettyPrint(BZ_STD_SCOPE(string) &str, 
        prettyPrintFormat& format) const
    {
        if (format.tersePrintingSelected())
            str += format.nextScalarOperandSymbol();
        else
            str += BZ_DEBUG_TEMPLATE_AS_STRING_LITERAL(T_numtype);
    }

    template<typename T_shape>
    bool shapeCheck(const T_shape&) const
    { return true; }


  // sliceinfo for expressions
  template<typename T1, typename T2 = nilArraySection, 
	   class T3 = nilArraySection, typename T4 = nilArraySection, 
	   class T5 = nilArraySection, typename T6 = nilArraySection, 
	   class T7 = nilArraySection, typename T8 = nilArraySection, 
	   class T9 = nilArraySection, typename T10 = nilArraySection, 
	   class T11 = nilArraySection>
  class SliceInfo {
  public:
    typedef _bz_ArrayExprConstant<T_numtype> T_slice;
};

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
        typename T7, typename T8, typename T9, typename T10, typename T11>
    typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8, T9 r9, T10 r10, T11 r11) const
    {
      return *this;
    }

protected:
    _bz_ArrayExprConstant() { }

    T_numtype value_;
};

BZ_NAMESPACE_END

#include <blitz/array/asexpr.h>

#endif // BZ_ARRAYEXPR_H

