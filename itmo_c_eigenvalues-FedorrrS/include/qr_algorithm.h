#pragma once

#ifndef C_2023_M3135_QR_ALGORITHM_H
#define C_2023_M3135_QR_ALGORITHM_H

#include "../return_codes.h"
#include "householder.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define EPS 1e-8
#define THRESHOLD_VALUE 1
#define MAX_ITER 1900

typedef unsigned int uint;

/* writes complex roots to output file in the format "{re} + {im}i"
 * for a given quadratic polynomial represented by a, b, c, and d */
void write_complex(FILE *output_file, double a, double b, double c, double d);
/* writes eigenvalues of diagonal matrix */
void write_diagonal(FILE *output_file, matrix m);
/* sets elements below the first subdiagonal of a matrix to zero */
void deflation(matrix A, size_t N);
/*  checks if a matrix is quasitriangular,
 * i.e., it is upper triangular except for some small entries on the diagonal */
bool is_quasitriangular(matrix A, size_t N);
/* checks if two matrices have elements that differ by more than a specified tolerance */
bool check_diff(matrix A, matrix B, size_t N);
/* extracts eigenvalues of a matrix and writes them to an output file.
 * If the matrix is of size 1 or 2, it writes the eigenvalue(s) directly.
 * Otherwise, it uses the QR algorithm to obtain the eigenvalues */
void extract_eigenvalues(matrix A, FILE *output_file, size_t N);
/* applies the QR algorithm to a given matrix to obtain its eigenvalues.
 * It iteratively calculates the QR decomposition of the matrix until it becomes quasitriangular.
 * If the number of iterations exceeds a maximum value or the difference between two consecutive matrices becomes
 * smaller than a tolerance value, it returns the current matrix. */
void qr_algorithm(matrix *R, matrix *Q, matrix *A_cur, matrix *A_next, size_t N);

#endif
