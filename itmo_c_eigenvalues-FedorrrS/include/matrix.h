#pragma once

#ifndef C_2023_M3135_MATRIX_H
#define C_2023_M3135_MATRIX_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Define type matrix_t, *matrix is a type alias for a pointer to matrix_t struct.
 * It allows to declare variables of type 'matrix' instead of writing `matrix_t*` */
typedef struct
{
	size_t n;
	double **v;
} matrix_t, *matrix;

/* allocates memory for a square matrix of size n and returns a pointer to it */
matrix create_matrix(size_t n);
/* frees the memory allocated to a matrix */
void delete_matrix(matrix m);
/* transposes a matrix in place */
void transpose(matrix m);
/* creates a new matrix and initializes it with the values from a given 2D array */
void copy_matrix(matrix m, size_t n, double a[][n]);
/* multiplies two matrices */
matrix mul_matrix(matrix x, matrix y);
/* extracts the minor of a matrix with the given index and returns it as a new matrix */
matrix find_matrix_minor(matrix x, size_t d);
/* checks if matrix is diagonal */
bool is_diagonal(matrix m, size_t n);
/* prints a matrix to the console */
void print_matrix(matrix m);

#endif
