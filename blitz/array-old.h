/***************************************************************************
 * blitz/array-old.h  Maximal include version of Array<P_numtype, N_rank>
 *                    Note: see <blitz/array-impl.h> for the class def.
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

#ifndef BZ_ARRAY_OLD_H
#define BZ_ARRAY_OLD_H

/*
 * <blitz/array.h> used to include most of the Blitz++ library
 * functionality, totally ~ 120000 lines of source code.  This
 * made for extremely slow compile times; processing #include <blitz/array.h>
 * took gcc about 25 seconds on a 500 MHz pentium box.
 *
 * Much of this compile time was due to the old vector expression templates
 * implementation.  Since this is not really needed for the Array<T,N>
 * class, the headers were redesigned so that:
 *
 * #include <blitz/array-old.h>   is the old-style include, pulls in most
 *                                of Blitz++ including vector e.t. 
 * #include <blitz/array.h>       pulls in much less of the library, and
 *                                in particular excludes the vector e.t. code
 *
 * With <blitz/array-old.h>, one gets TinyVector expressions automatically.
 * With <blitz/array.h>, one must now also include <blitz/tinyvec-et.h> 
 * to get TinyVector expressions.
 *
 * The implementation of Array<T,N> has been moved to <blitz/array-impl.h>.
 */

#include <blitz/tinyvec-et.h>
#include <blitz/array-impl.h>

#endif // BZ_ARRAY_OLD_H

