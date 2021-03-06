/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2010,2011 Fredrik Johansson
    Copyright (C) 2013 Mike Hansen

******************************************************************************/


#ifdef T

#include "templates.h"

void
TEMPLATE(T, mat_solve_triu) (TEMPLATE(T, mat_t) X, const TEMPLATE(T, mat_t) U,
                             const TEMPLATE(T, mat_t) B, int unit,
                             const TEMPLATE(T, ctx_t) ctx)
{
    if (B->r < TEMPLATE(CAP_T, MAT_SOLVE_TRI_ROWS_CUTOFF) ||
        B->c < TEMPLATE(CAP_T, MAT_SOLVE_TRI_COLS_CUTOFF))
    {
        TEMPLATE(T, mat_solve_triu_classical) (X, U, B, unit, ctx);
    }
    else
    {
        TEMPLATE(T, mat_solve_triu_recursive) (X, U, B, unit, ctx);
    }
}


#endif
