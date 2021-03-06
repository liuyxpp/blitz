// -*- C++ -*-
/***************************************************************************
 * blitz/matref.h      Declaration of the _bz_MatrixRef<P_numtype, P_structure>
 *                     class.
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
 ***************************************************************************/

#ifndef BZ_MATREF_H
#define BZ_MATREF_H

#ifndef BZ_MATEXPR_H
 #error <blitz/matref.h> must be included via <blitz/matexpr.h>
#endif // BZ_MATEXPR_H

BZ_NAMESPACE(blitz)

template<typename P_numtype, typename P_structure>
class _bz_MatrixRef {

public:
    typedef P_numtype T_numtype;

    _bz_MatrixRef(const Matrix<P_numtype, P_structure>& m)
        : matrix_(&m)
    { }

    T_numtype operator()(unsigned i, unsigned j) const
    { return (*matrix_)(i,j); }

    unsigned rows(unsigned) const
    { return matrix_->rows(); }

    unsigned cols(unsigned) const
    { return matrix_->cols(); }

private:
    _bz_MatrixRef() { } 

    const Matrix<P_numtype, P_structure>* matrix_;
};

BZ_NAMESPACE_END

#endif // BZ_MATREF_H
