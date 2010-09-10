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

    Copyright (C) 2008, 2009 William Hart

******************************************************************************/

#include <stdlib.h>
#include <mpir.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpz_vec.h"
#include "fmpz_poly.h"

void
_fmpz_poly_divrem_divconquer(fmpz * Q, fmpz * R,
                             const fmpz * A, long lenA, const fmpz * B,
                             long lenB)
{
    if (lenA < 2 * lenB - 1)
    {
        /*
           Convert unbalanced division into a 2 n1 - 1 by n1 division
         */

        const long n1 = lenA - lenB + 1;
        const long n2 = lenB - n1;

        const fmpz * p1 = A + n2;
        const fmpz * d1 = B + n2;
        const fmpz * d2 = B;

        fmpz * d1q1 = R + n2;
        fmpz * d2q1 = _fmpz_vec_init(lenB - 1);

        _fmpz_poly_divrem_divconquer_recursive(Q, d1q1, p1, d1, n1);

        /*
           Compute d2q1 = Q d2, of length lenB - 1
         */

        if (n1 >= n2)
            _fmpz_poly_mul(d2q1, Q, n1, d2, n2);
        else
            _fmpz_poly_mul(d2q1, d2, n2, Q, n1);

        /*
           Compute BQ = d1q1 * x^n1 + d2q1, of length lenB - 1; 
           then compute R = A - BQ
         */

        _fmpz_vec_copy(R, d2q1, n2);
        _fmpz_vec_add(R + n2, R + n2, d2q1 + n2, n1 - 1);
        _fmpz_vec_sub(R, A, R, lenA);

        _fmpz_vec_clear(d2q1, lenB - 1);
    }
    else if (lenA > 2 * lenB - 1)
    {
        /*
           We shift A right until it is of length 2 lenB - 1, call this p1
         */

        const long shift = lenA - 2 * lenB + 1;
        const fmpz * p1 = A + shift;

        fmpz * q1   = Q + shift;
        fmpz * q2   = Q;
        fmpz * dq1  = _fmpz_vec_init(lenA);
        fmpz * d1q1 = dq1 + shift;

        /* 
           Set q1 to p1 div B, a 2 lenB - 1 by lenB division, so q1 ends up 
           being of length lenB; set d1q1 = d1 * q1 of length 2 lenB - 1
         */

        _fmpz_poly_divrem_divconquer_recursive(q1, d1q1, p1, B, lenB);

        /* 
           We have dq1 = d1 * q1 * x^shift, of length lenA

           Compute R = A - dq1; the first lenB coeffs represent remainder 
           terms (zero if division is exact), leaving lenA - lenB significant 
           terms which we use in the division
         */

        _fmpz_vec_copy(dq1, A, shift);
        _fmpz_vec_sub(dq1 + shift, A + shift, dq1 + shift, lenB - 1);
        _fmpz_vec_sub(R + lenA - lenB, A + lenA - lenB,
                      dq1 + lenA - lenB, lenB);

        /*
           Compute q2 = trunc(R) div B; it is a smaller division than the 
           original since len(trunc(R)) = lenA - lenB
         */

        _fmpz_poly_divrem_divconquer(q2, R, dq1, lenA - lenB, B, lenB);

        /*
           We have Q = q1 * x^shift + q2; Q has length lenB + shift; 
           note q2 has length shift since the above division is 
           lenA - lenB by lenB

           We've also written the remainder in place
         */

        _fmpz_vec_clear(dq1, lenA);
    }
    else  /* lenA = 2 * lenB - 1 */
    {
        _fmpz_poly_divrem_divconquer_recursive(Q, R, A, B, lenB);
        _fmpz_vec_sub(R, A, R, lenA);
    }
}

void
fmpz_poly_divrem_divconquer(fmpz_poly_t Q, fmpz_poly_t R,
                            const fmpz_poly_t A, const fmpz_poly_t B)
{
    fmpz_poly_t t1, t2;
    fmpz *Q_coeffs, *R_coeffs;

    if (B->length == 0)
    {
        printf("Exception: division by zero in fmpz_poly_divrem_divconquer\n");
        abort();
    }

    if (A->length < B->length)
    {
        fmpz_poly_zero(Q);
        fmpz_poly_set(R, A);
        return;
    }

    if (Q == A || Q == B)
    {
        fmpz_poly_init2(t1, A->length - B->length + 1);
        Q_coeffs = t1->coeffs;
    }
    else
    {
        fmpz_poly_fit_length(Q, A->length - B->length + 1);
        Q_coeffs = Q->coeffs;
    }

    if (R == A || R == B)
    {
        fmpz_poly_init2(t2, A->length);
        R_coeffs = t2->coeffs;
    }
    else
    {
        fmpz_poly_fit_length(R, A->length);
        R_coeffs = R->coeffs;
    }

    _fmpz_poly_divrem_divconquer(Q_coeffs, R_coeffs, A->coeffs, A->length,
                                 B->coeffs, B->length);

    if (Q == A || Q == B)
    {
        _fmpz_poly_set_length(t1, A->length - B->length + 1);
        fmpz_poly_swap(t1, Q);
        fmpz_poly_clear(t1);
    }
    else
        _fmpz_poly_set_length(Q, A->length - B->length + 1);

    if (R == A || R == B)
    {
        _fmpz_poly_set_length(t2, A->length);
        fmpz_poly_swap(t2, R);
        fmpz_poly_clear(t2);
    }
    else
        _fmpz_poly_set_length(R, A->length);

    _fmpz_poly_normalise(Q);
    _fmpz_poly_normalise(R);
}
