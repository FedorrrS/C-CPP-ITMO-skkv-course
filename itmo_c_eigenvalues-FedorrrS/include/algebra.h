#pragma once

#include "matrix.h"

#include <math.h>

/* c = a + b * v
 * computes the linear combination of two vectors a and b, scaled by a scalar v,
 * and stores the result in vector c
 * */
double *vmadd(const double a[], const double b[], double v, double c[], size_t n);
/* computes the matrix product of a given vector v with itself,
 * scaled and transposed, and returns the resulting matrix */
matrix vmul(const double v[], size_t n);
/* ||x||
 * computes the Euclidean norm of a given vector x */
double vnorm(const double x[], size_t n);
/* y = x / d
 * computes the element-wise division of a given vector x by a scalar d, and stores the result in vector y */
double *vdiv(const double x[], double d, double y[], size_t n);
/* extracts the values of a given column c from a given matrix m and stores them in a vector v.*/
double *mcol(matrix m, double *v, size_t c);
