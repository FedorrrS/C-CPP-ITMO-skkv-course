#include "../include/algebra.h"

double *vmadd(const double a[], const double b[], double v, double c[], size_t n)
{
	for (size_t i = 0; i < n; ++i)
	{
		c[i] = a[i] + v * b[i];
	}
	return c;
}

matrix vmul(const double v[], size_t n)
{
	matrix x = create_matrix(n);
	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = 0; j < n; ++j)
		{
			x->v[i][j] = -2 * v[i] * v[j];
		}
	}
	for (size_t i = 0; i < n; ++i)
	{
		x->v[i][i] += 1;
	}
	return x;
}
double vnorm(const double x[], size_t n)
{
	double sum = 0;
	for (size_t i = 0; i < n; ++i)
	{
		sum += x[i] * x[i];
	}
	return sqrt(sum);
}

double *vdiv(const double x[], double d, double y[], size_t n)
{
	for (size_t i = 0; i < n; ++i)
	{
		y[i] = x[i] / d;
	}
	return y;
}

double *mcol(matrix m, double *v, size_t c)
{
	for (size_t i = 0; i < m->n; ++i)
	{
		v[i] = m->v[i][c];
	}
	return v;
}