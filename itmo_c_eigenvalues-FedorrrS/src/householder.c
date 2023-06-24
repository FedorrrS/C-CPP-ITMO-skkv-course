#include "../include/householder.h"

/* The main point of the Householder's method is to successively reduce the matrix A to the upper triangular form by
 * means of its domaining by the reflection matrices. The resulting triangular matrix will be the matrix R, and the
 * matrix Q will be equal to the product of the conjugate reflection matrices.
 *
 * At the i-th step the task of the i-th reflection matrix is to zeroize all the subdiagonal elements of the i-th column
 * of matrix A (the columns to the left of the i-th are not changed). The algorithm consists of n-1 steps, at each
 * of which the next reflection matrix is calculated, after which the found reflection is applied to the matrix that is
 * the result of the previous step */
void householder_decomposition(matrix A, matrix *R, matrix *Q)
{
	matrix *q = (matrix *)malloc(A->n * sizeof(matrix));
	double(*e)[A->n] = malloc(sizeof(double[A->n][A->n]));
	double(*x)[A->n] = malloc(sizeof(double[A->n][A->n]));

	matrix z = A, z1;
	for (size_t k = 0; k < A->n - 1; ++k)
	{
		z1 = find_matrix_minor(z, k);
		if (z != A)
		{
			delete_matrix(z);
		}
		z = z1;

		mcol(z, *x, k);
		double a = vnorm(*x, A->n);
		if (A->v[k][k] > 0)
		{
			a = -a;
		}

		for (size_t i = 0; i < A->n; i++)
		{
			(*e)[i] = (i == k) ? 1 : 0;
		}

		vmadd(*x, *e, a, *e, A->n);
		vdiv(*e, vnorm(*e, A->n), *e, A->n);
		q[k] = vmul(*e, A->n);
		z1 = mul_matrix(q[k], z);
		if (z != A)
		{
			delete_matrix(z);
		}
		z = z1;
	}
	delete_matrix(z);
	*Q = q[0];
	*R = mul_matrix(q[0], A);
	for (size_t i = 1; i < A->n - 1; ++i)
	{
		z1 = mul_matrix(q[i], *Q);
		if (i > 1)
		{
			delete_matrix(*Q);
		}
		*Q = z1;
		delete_matrix(q[i]);
	}
	delete_matrix(q[0]);
	delete_matrix(*R);
	*R = mul_matrix(*Q, A);
	transpose(*Q);

	free(e);
	free(x);
	free(q);
}
