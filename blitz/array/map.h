// -*- C++ -*-
/***************************************************************************
 * blitz/array/map.h      Declaration of the ArrayIndexMapping class
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

/*
 * ArrayIndexMapping is used to implement tensor array notation.  For
 * example:
 *
 * Array<float, 2> A, B;
 * firstIndex i;
 * secondIndex j;
 * thirdIndex k;
 * Array<float, 3> C = A(i,j) * B(j,k);
 *
 * For expression templates purposes, something like B(j,k) is represented
 * by an instance of class ArrayIndexMapping.  This class maps an array onto
 * the destination array coordinate system, e.g. B(j,k) -> C(i,j,k)
 */

#ifndef BZ_ARRAYMAP_H
#define BZ_ARRAYMAP_H

#ifndef BZ_ARRAY_H
 #error <blitz/array/map.h> must be included via <blitz/array.h>
#endif

#include <blitz/array/fastiter.h>

BZ_NAMESPACE(blitz)

/*
 * _bz_doArrayIndexMapping is a helper class that does the index
 * remapping.  It is specialized for ranks 1, 2, 3, ..., 11.
 */

template<int N_rank>
struct _bz_doArrayIndexMapping {
  static const int rank=N_rank;
    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, N_rank>&, 
        const TinyVector<int,N_inputRank>&, int, int, int, int, int, int,
        int, int, int, int, int)
    {
        // If you try to use an array index mapping on an array with
        // rank greater than 11, then you'll get a precondition failure
        // here.
        BZPRECONDITION(0);
        return T_numtype();
    }
};

template<>
struct _bz_doArrayIndexMapping<1> {
  static const int rank=1;
    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, rank>& array,
        const TinyVector<int,N_inputRank>& index, int i0, int, int, int, int, 
        int, int, int, int, int, int)
    {
        return array(index[i0]);
    }

  template<int N_inputRank>
  static TinyVector<int,rank> map_dims(const TinyVector<int,N_inputRank>& index, 
			 int i0, int, int, int, int, 
			 int, int, int, int, int, int)
    {
      // this might be slower but unlike Array, FAI doesn't have 11
      // overloaded moveTo methods.
      const TinyVector<int,rank> newindex(index[i0]);
      return newindex;
    }
};


template<>
struct _bz_doArrayIndexMapping<2> {
  static const int rank=2;

    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, rank>& array,
        const TinyVector<int,N_inputRank>& index, int i0, int i1, int, 
        int, int, int, int, int, int, int, int)
    {
        return array(index[i0], index[i1]);
    }

  template<int N_inputRank>
  static TinyVector<int,rank> map_dims(const TinyVector<int,N_inputRank>& index, 
			 int i0, int i1, int, int, int, 
			 int, int, int, int, int, int)
    {
      return TinyVector<int,rank>(index[i0], index[i1]);
    }
};

template<>
struct _bz_doArrayIndexMapping<3> {
  static const int rank=3;

    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, rank>& array,
        const TinyVector<int,N_inputRank>& index, int i0, int i1, int i2,
        int, int, int, int, int, int, int, int)
    {
        return array(index[i0], index[i1], index[i2]);
    }

  template<int N_inputRank>
  static TinyVector<int,rank> map_dims(const TinyVector<int,N_inputRank>& index, 
			 int i0, int i1, int i2, int, int, 
			 int, int, int, int, int, int)
    {
      return TinyVector<int,rank>(index[i0], index[i1], index[i2]);
    }
};

template<>
struct _bz_doArrayIndexMapping<4> {
  static const int rank=4;

    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, rank>& array,
        const TinyVector<int,N_inputRank>& index, int i0, int i1, int i2,
        int i3, int, int, int, int, int, int, int)
    {
        return array(index[i0], index[i1], index[i2], index[i3]);
    }

  template<int N_inputRank>
  static TinyVector<int,rank> map_dims(const TinyVector<int,N_inputRank>& index, 
			 int i0, int i1, int i2, int i3, int, 
			 int, int, int, int, int, int)
    {
      return TinyVector<int,rank>(index[i0], index[i1], index[i2],
				  index[i3]);
    }
};

template<>
struct _bz_doArrayIndexMapping<5> {
  static const int rank=5;

    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, rank>& array,
        const TinyVector<int,N_inputRank>& index, int i0, int i1, int i2,
        int i3, int i4, int, int, int, int, int, int)
    {
        return array(index[i0], index[i1], index[i2], index[i3], index[i4]);
    }

  template<int N_inputRank>
  static TinyVector<int,rank> map_dims(const TinyVector<int,N_inputRank>& index, 
			 int i0, int i1, int i2, int i3, int i4, 
			 int, int, int, int, int, int)
    {
      return TinyVector<int,rank>(index[i0], index[i1], index[i2],
				  index[i3], index[i4]);
    }
};

template<>
struct _bz_doArrayIndexMapping<6> {
  static const int rank=6;

    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, rank>& array,
        const TinyVector<int,N_inputRank>& index, int i0, int i1, int i2,
        int i3, int i4, int i5, int, int, int, int, int)
    {
        return array(index[i0], index[i1], index[i2], index[i3], index[i4],
            index[i5]);
    }

  template<int N_inputRank>
  static TinyVector<int,rank> map_dims(const TinyVector<int,N_inputRank>& index, 
			 int i0, int i1, int i2, int i3, int i4, 
			 int i5, int, int, int, int, int)
    {
      return TinyVector<int,rank>(index[i0], index[i1], index[i2],
				  index[i3], index[i4], index[i5]);
    }
};

template<>
struct _bz_doArrayIndexMapping<7> {
  static const int rank=7;

    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, rank>& array,
        const TinyVector<int,N_inputRank>& index, int i0, int i1, int i2,
        int i3, int i4, int i5, int i6, int, int, int, int)
    {
        return array(index[i0], index[i1], index[i2], index[i3], index[i4],
            index[i5], index[i6]);
    }

  template<int N_inputRank>
  static TinyVector<int,rank> map_dims(const TinyVector<int,N_inputRank>& index, 
			 int i0, int i1, int i2, int i3, int i4, 
			 int i5, int i6, int, int, int, int)
    {
      return TinyVector<int,rank>(index[i0], index[i1], index[i2],
				  index[i3], index[i4], index[i5],
				  index[i6]);
    }
};

template<>
struct _bz_doArrayIndexMapping<8> {
  static const int rank=8;

    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, rank>& array,
        const TinyVector<int,N_inputRank>& index, int i0, int i1, int i2,
        int i3, int i4, int i5, int i6, int i7, int, int, int)
    {
        return array(index[i0], index[i1], index[i2], index[i3], index[i4],
            index[i5], index[i6], index[i7]);
    }

  template<int N_inputRank>
  static TinyVector<int,rank> map_dims(const TinyVector<int,N_inputRank>& index, 
			 int i0, int i1, int i2, int i3, int i4, 
			 int i5, int i6, int i7, int, int, int)
    {
      return TinyVector<int,rank>(index[i0], index[i1], index[i2],
				  index[i3], index[i4], index[i5],
				  index[i6], index[i7]);
    }
};

template<>
struct _bz_doArrayIndexMapping<9> {
  static const int rank=9;

    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, rank>& array,
        const TinyVector<int,N_inputRank>& index, int i0, int i1, int i2,
        int i3, int i4, int i5, int i6, int i7, int i8, int, int)
    {
        return array(index[i0], index[i1], index[i2], index[i3], index[i4],
            index[i5], index[i6], index[i7], index[i8]);
    }

  template<int N_inputRank>
  static TinyVector<int,rank> map_dims(const TinyVector<int,N_inputRank>& index, 
			 int i0, int i1, int i2, int i3, int i4, 
			 int i5, int i6, int i7, int i8, int, int)
    {
      return TinyVector<int,rank>(index[i0], index[i1], index[i2],
				  index[i3], index[i4], index[i5],
				  index[i6], index[i7], index[i8]);
    }
};

template<>
struct _bz_doArrayIndexMapping<10> {
  static const int rank=10;

    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, rank>& array,
        const TinyVector<int,N_inputRank>& index, int i0, int i1, int i2,
        int i3, int i4, int i5, int i6, int i7, int i8, int i9, int)
    {
        return array(index[i0], index[i1], index[i2], index[i3], index[i4],
            index[i5], index[i6], index[i7], index[i8], index[i9]);
    }

  template<int N_inputRank>
  static TinyVector<int,rank> map_dims(const TinyVector<int,N_inputRank>& index, 
			 int i0, int i1, int i2, int i3, int i4, 
			 int i5, int i6, int i7, int i8, int i9, int)
    {
      return TinyVector<int,rank>(index[i0], index[i1], index[i2],
				  index[i3], index[i4], index[i5],
				  index[i6], index[i7], index[i8],
				  index[i9]);
    }
};

template<>
struct _bz_doArrayIndexMapping<11> {
  static const int rank=11;

    template<typename T_numtype, int N_inputRank>
    static T_numtype map(const Array<T_numtype, rank>& array,
        const TinyVector<int,N_inputRank>& index, int i0, int i1, int i2,
        int i3, int i4, int i5, int i6, int i7, int i8, int i9, int i10)
    {
        return array(index[i0], index[i1], index[i2], index[i3], index[i4],
            index[i5], index[i6], index[i7], index[i8], index[i9],
            index[i10]);
    }

  template<int N_inputRank>
  static TinyVector<int,rank> map_dims(const TinyVector<int,N_inputRank>& index, 
			 int i0, int i1, int i2, int i3, int i4, 
			 int i5, int i6, int i7, int i8, int i9, int i10)
    {
      return TinyVector<int,rank>(index[i0], index[i1], index[i2],
				  index[i3], index[i4], index[i5],
				  index[i6], index[i7], index[i8],
				  index[i9], index[i10]);
    }
};


template<typename P_expr, int N_map0, int N_map1=0, int N_map2=0,
    int N_map3=0, int N_map4=0, int N_map5=0, int N_map6=0, int N_map7=0, 
    int N_map8=0, int N_map9=0, int N_map10=0>
class ArrayIndexMapping {
public:
  typedef P_expr T_expr;
  typedef typename T_expr::T_numtype T_numtype;
  typedef T_expr T_ctorArg1;
    typedef int                            T_ctorArg2;    // dummy
  typedef ArrayIndexMapping<typename T_expr::T_range_result,N_map0,N_map1,N_map2,N_map3,N_map4,N_map5,N_map6,N_map7,N_map8,N_map9,N_map10> T_range_result;

    /*
     * This enum block finds the maximum of the N_map0, N_map1, ..., N_map10
     * parameters and stores it in maxRank10.  The rank of the expression is
     * then maxRank10 + 1, since the IndexPlaceholders start at 0 rather than
     * 1.  
     */
    static const int
        maxRank1 = (N_map0 > N_map1) ? N_map0 : N_map1,
        maxRank2 = (N_map2 > maxRank1) ? N_map2 : maxRank1,
        maxRank3 = (N_map3 > maxRank2) ? N_map3 : maxRank2,
        maxRank4 = (N_map4 > maxRank3) ? N_map4 : maxRank3,
        maxRank5 = (N_map5 > maxRank4) ? N_map5 : maxRank4,
        maxRank6 = (N_map6 > maxRank5) ? N_map6 : maxRank5,
        maxRank7 = (N_map7 > maxRank6) ? N_map7 : maxRank6,
        maxRank8 = (N_map8 > maxRank7) ? N_map8 : maxRank7,
        maxRank9 = (N_map9 > maxRank8) ? N_map9 : maxRank8,
        maxRank10 = (N_map10 > maxRank9) ? N_map10 : maxRank9;

    static const int 
        numArrayOperands = 1, 
        numIndexPlaceholders = 1,
        rank = maxRank10 + 1,
        exprRank = T_expr::rank;

  /*
    ArrayIndexMapping(const Array<T_numtype, rank>& array)
        : iter_(array)
    { 
    }
  */

    ArrayIndexMapping(const ArrayIndexMapping<T_expr,N_map0,
        N_map1,N_map2,N_map3,N_map4,N_map5,N_map6,N_map7,N_map8,N_map9,
        N_map10>& z)
        : iter_(z.iter_)
    { 
    }

  ArrayIndexMapping(BZ_ETPARM(T_expr) a)
        : iter_(a)
    { }

  ArrayIndexMapping(_bz_typename T_expr::T_ctorArg1 a)
        : iter_(a)
    { }

  // these bypass the FAI and go directly to the array. That should
  // prevent any performance impact of using the FAI instead of an
  // array directly.

#ifdef BZ_ARRAY_EXPR_PASS_INDEX_BY_VALUE
    template<int N_inputRank>
    T_numtype operator()(TinyVector<int, N_inputRank> i) const
    {
      return _bz_doArrayIndexMapping<exprRank>::map(iter_.array(), i,
            N_map0, N_map1, N_map2, N_map3, N_map4, N_map5, N_map6,
            N_map7, N_map8, N_map9, N_map10);
    }
#else
    template<int N_inputRank>
    T_numtype operator()(const TinyVector<int, N_inputRank>& i) const
    {
      return _bz_doArrayIndexMapping<exprRank>::map(iter_.array(), i,
            N_map0, N_map1, N_map2, N_map3, N_map4, N_map5, N_map6,
            N_map7, N_map8, N_map9, N_map10);
    }
#endif

  // find which dimension in mapped expression that corresponds to
  // dimension dim. This works such that dimension dim in this
  // expression corresponds to dimension map_dim(dim) in iter_.
    int map_dim(const int dim) const
    { 
        if (N_map0 == dim)    
	  return 0;
        else if ((N_map1 == dim) && (exprRank > 1))
	  return 1;
        else if ((N_map2 == dim) && (exprRank > 2))
	  return 2;
        else if ((N_map3 == dim) && (exprRank > 3))
	  return 3;
        else if ((N_map4 == dim) && (exprRank > 4))
	  return 4;
        else if ((N_map5 == dim) && (exprRank > 5))
	  return 5;
        else if ((N_map6 == dim) && (exprRank > 6))
	  return 6;
        else if ((N_map7 == dim) && (exprRank > 7))
	  return 7;
        else if ((N_map8 == dim) && (exprRank > 8))
	  return 8;
        else if ((N_map9 == dim) && (exprRank > 9))
	  return 9;
        else if ((N_map10 == dim) && (exprRank > 10))
	  return 10;
        else
	  // means dimension is not in this operand
	  return -1;
    }

  // remaps the dimensions of an index vector so it can be applied to
  // iter_, using the _bz_doArrayIndexMapping helper class.
  template<int N> 
  TinyVector<int, exprRank> map_dims(const TinyVector<int, N>& i) const {
    return _bz_doArrayIndexMapping<exprRank>::map_dims
      (i, N_map0, N_map1, N_map2, N_map3, N_map4, N_map5,
       N_map6, N_map7, N_map8, N_map9, N_map10);
  }

  int ascending(const int dim) const
  {
    const int d=map_dim(dim);
    const int o = d>=0 ? iter_.ascending(d) : INT_MIN ;
    return o;
  }

  int ordering(const int dim) const
  {
    // JCC: ignore ordering result from 1d Array
    if (exprRank == 1)
      return INT_MIN;  // tiny(int());

    const int d=map_dim(dim);
    const int o = d>=0 ? iter_.ordering(d) : INT_MIN ;
    return o;
  }

  int lbound(const int dim) const
  {
    const int d=map_dim(dim);
    const int o = d>=0 ? iter_.lbound(d) : INT_MIN ;
    return o;
  }

  int ubound(const int dim) const
  {
    const int d=map_dim(dim);
    const int o = d>=0 ? iter_.ubound(d) : INT_MAX ;
    return o;
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

    // If you have a precondition failure on this routine, it means
    // you are trying to use stack iteration mode on an expression
    // which contains an index placeholder.  You must use index
    // iteration mode instead.
  // (no -- added to support stencils /PJ)
    T_numtype operator*() const
    {
      return *iter_;
    }

    // See operator*() note
    void push(int)
    {
        BZPRECONDITION(0);
    }

    // See operator*() note
    void pop(int)
    {
        BZPRECONDITION(0);
    }

    // See operator*() note
    void advance()
    {
        BZPRECONDITION(0);
    }

    // See operator*() note
    void advance(int)
    {
        BZPRECONDITION(0);
    }

    // See operator*() note
    void loadStride(int)
    {
        BZPRECONDITION(0);
    }

    bool isUnitStride(int) const
    {
        BZPRECONDITION(0);
        return false;
    }

    void advanceUnitStride()
    {
        BZPRECONDITION(0);
    }

    bool canCollapse(int,int) const
    {   BZPRECONDITION(0);  return false; }

    T_numtype operator[](int)
    {   
        BZPRECONDITION(0);
        return T_numtype();
    }

    T_numtype fastRead(int) const
    {
        BZPRECONDITION(0);
        return T_numtype();
    }

    int suggestStride(int) const
    {
        BZPRECONDITION(0);
        return 0;
    }

    bool isStride(int,int) const
    {
        BZPRECONDITION(0);
        return true;
    }

#ifdef BZ_ARRAY_EXPR_PASS_INDEX_BY_VALUE
    template<int N_destrank>
    void moveTo(const TinyVector<int,N_destrank> i)
    {
      iter_.moveTo(map_dims(i));
    }
#else
    template<int N_destrank>
    void moveTo(const TinyVector<int,N_destrank>& i)
    {
      iter_.moveTo(map_dims(i));
    }
#endif

  T_numtype shift(int offset, int dim) const { 
    // need to check if dim is mapped into this expression
    const int d=map_dim(dim);
    if (d<0)
      return *iter_;
    else
      return iter_.shift(offset, d);
  }

  T_numtype shift(int offset1, int dim1,int offset2, int dim2) const {
    // need to check if dims are mapped into this expression
    int d1=map_dim(dim1);
    int d2=map_dim(dim2);
    if (d1<0) //disable offset
      {d1=0;offset1=0;}
    if (d2<0) //disable offset
      {d2=0;offset2=0;}
    return iter_.shift(offset1, d1, offset2, d2);
  }

  void _bz_offsetData(sizeType i) { BZPRECONDITION(0); }

  template<int N>
  T_range_result operator()(RectDomain<N> d) const
  { // need to reorder dimensions here
    TinyVector<int, exprRank> lb(map_dims(d.lbound())), ub(map_dims(d.ubound()));
    RectDomain<exprRank> newd(lb,ub);
    return T_range_result(iter_(newd));
  }

    void prettyPrint(BZ_STD_SCOPE(string) &str, prettyPrintFormat&) const
    {
        // NEEDS_WORK-- do real formatting for reductions
        str += "map[NEEDS_WORK]";
    }

    template<typename T_shape>
    bool shapeCheck(const T_shape&) const
    { 
        // NEEDS_WORK-- do a real shape check (tricky)
        return true; 
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
    typedef typename T_expr::template SliceInfo<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::T_slice T_slice1;
    typedef ArrayIndexMapping<T_slice1, N_map0, N_map1, N_map2,
			      N_map3, N_map4, N_map5, N_map6, N_map7, 
			      N_map8, N_map9, N_map10> T_slice;
};

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6,
        typename T7, typename T8, typename T9, typename T10, typename T11>
    typename SliceInfo<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::T_slice
    operator()(T1 r1, T2 r2, T3 r3, T4 r4, T5 r5, T6 r6, T7 r7, T8 r8, T9 r9, T10 r10, T11 r11) const
    {
      /* Slicing for remapped expressions doesn't work. Because of the
	 potential different types (Range vs int) in the expression,
	 it would be very awkward to implement. As far as I can see,
	 it would require manual coding of the 3^11 calling
	 possibilities. /PJ */
      BZPRECONDITION(0);
    }

private:
    ArrayIndexMapping() : iter_( Array<T_numtype, exprRank>() ) { }

    T_expr iter_;
};

BZ_NAMESPACE_END

#endif // BZ_ARRAYMAP_H

