#include "../include/matrix.h"

matrix create_matrix(size_t n)
{
	matrix x = malloc(sizeof(matrix_t));
	if (!x)
	{
		fprintf(stderr, "Can't allocate memory for matrix\n");
		return NULL;
	}
	/* allocating an array of pointers, where each pointer points to a 1D-array of db's */
	x->v = (double **)malloc(sizeof(double *) * n);
	if (!x->v)
	{
		fprintf(stderr, "Can't allocate memory for matrix data\n");
		free(x);
		return NULL;
	}
	/* allocating an array of doubles */
	x->v[0] = (double *)calloc(sizeof(double), n * n);
	if (!x->v[0])
	{
		fprintf(stderr, "Can't allocate memory for matrix data\n");
		free(x->v);
		free(x);
		return NULL;
	}
	for (size_t i = 0; i < n; ++i)
	{
		x->v[i] = x->v[0] + n * i;
	}
	x->n = n;
	return x;
}

void delete_matrix(matrix m)
{
	if (m)
	{
		if (m->v)
		{
			if (m->v[0])
			{
				free(m->v[0]);
			}
			free(m->v);
		}
		free(m);
	}
}

void transpose(matrix m)
{
	for (size_t i = 0; i < m->n; ++i)
	{
		for (size_t j = 0; j < i; ++j)
		{
			double t = m->v[i][j];
			m->v[i][j] = m->v[j][i];
			m->v[j][i] = t;
		}
	}
}
void copy_matrix(matrix m, size_t n, double a[][n])
{
	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = 0; j < n; ++j)
		{
			m->v[i][j] = a[i][j];
		}
	}
}

matrix mul_matrix(matrix x, matrix y)
{
	if (x->n != y->n)
	{
		return 0;
	}
	matrix r = create_matrix(x->n);
	for (size_t i = 0; i < x->n; ++i)
	{
		for (size_t j = 0; j < y->n; ++j)
		{
			for (size_t k = 0; k < x->n; ++k)
			{
				r->v[i][j] += x->v[i][k] * y->v[k][j];
			}
		}
	}
	return r;
}

matrix find_matrix_minor(matrix x, size_t d)
{
	matrix m = create_matrix(x->n);
	for (size_t i = 0; i < d; ++i)
	{
		m->v[i][i] = 1;
	}
	for (size_t i = d; i < x->n; ++i)
	{
		for (size_t j = d; j < x->n; ++j)
		{
			m->v[i][j] = x->v[i][j];
		}
	}
	return m;
}

bool is_diagonal(matrix m, size_t n)
{
	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = 0; j < n; ++j)
		{
			if (i != j && m->v[i][j] != 0)
			{
				return false;
			}
		}
	}
	return true;
}

#define DEBUG
#ifdef DEBUG
void print_matrix(matrix m)
{
	for (size_t i = 0; i < m->n; ++i)
	{
		for (size_t j = 0; j < m->n; ++j)
		{
			printf(" %10.5f", m->v[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
#endif