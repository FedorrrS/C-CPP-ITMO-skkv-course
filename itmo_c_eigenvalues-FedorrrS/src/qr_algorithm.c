#include "../include/qr_algorithm.h"

void write_complex(FILE *output_file, double a, double b, double c, double d)
{
	double disc = pow(a - d, 2) + 4 * c * b;
	double re = (a + d) / 2, im;
	if (disc < 0)
	{
		im = fabs(sqrt(-disc)) / 2;
		fprintf(output_file, "%g +%gi\n%g -%gi\n", re, im, re, im);
	}
	else
	{
		im = sqrt(disc) / 2;
		fprintf(output_file, "%g\n%g\n", re + im, re - im);
	}
}

void write_diagonal(FILE *output_file, matrix m)
{
	for (size_t i = 0; i < m->n; ++i)
	{
		fprintf(output_file, "%g\n", m->v[0][0]);
	}
}

void deflation(matrix A, size_t N)
{
	for (size_t i = 0; i < N - 1; ++i)
	{
		for (size_t j = 0; j < N && i == j; ++j)
		{
			if (A->v[i + 1][i] < EPS)
			{
				A->v[i + 1][i] = 0;
			}
		}
	}
}

bool is_quasitriangular(matrix A, size_t N)
{
	for (size_t i = 1; i < N; ++i)
	{
		size_t j = i - 1;
		if (fabs(A->v[i][j]) > EPS)
		{
			if (i == 1 && fabs(A->v[i + 1][j + 1]) > EPS)
			{
				return false;
			}
			if (i + 1 < N && fabs(A->v[i - 1][j - 1]) > EPS && fabs(A->v[i + 1][j + 1]) > EPS)
			{
				continue;
			}
			if (i == N - 1 && fabs(A->v[i - 1][j - 1]) > EPS)
			{
				continue;
			}
			return false;
		}
	}
	return true;
}

bool check_diff(matrix A, matrix B, size_t N)
{
	for (size_t i = 0; i < N; ++i)
	{
		for (size_t j = 0; j <= i - 1; ++j)
		{
			if (fabs(A->v[i][j] - B->v[i][j]) > EPS)
			{
				return false;
			}
		}
	}
	return true;
}

void extract_eigenvalues(matrix A, FILE *output_file, size_t N)
{
	uint counter = 0;
	for (size_t i = 0; i < N; ++i)
	{
		for (size_t j = 0; j < N; ++j)
		{
			if (counter < N)
			{
				if (i == j && i < N - 1 && fabs(A->v[i + 1][j]) < THRESHOLD_VALUE && fabs(A->v[i][j - 1]) < THRESHOLD_VALUE)
				{
					fprintf(output_file, "%g\n", A->v[i][j]);
					counter++;
				}
				else if (i == j && i < N - 1 && fabs(A->v[i + 1][j]) > THRESHOLD_VALUE)
				{
					double a = A->v[i][j], b = A->v[i + 1][j], c = A->v[i][j + 1], d = A->v[i + 1][j + 1];
					write_complex(output_file, a, b, c, d);
					counter += 2;
				}
				else if (i == N - 1)
				{
					fprintf(output_file, "%g\n", A->v[N - 1][N - 1]);
					counter++;
				}
			}
		}
	}
}

/* A_cur - current matrix, on the basis of which the QR-decomposition will be calculated at each step
 * A_next - new matrix obtained after multiplication of matrices R and Q */
void qr_algorithm(matrix *R, matrix *Q, matrix *A_cur, matrix *A_next, size_t N)
{
	deflation(*A_next, N);
	uint iter = 0;
	while (!(is_quasitriangular(*A_next, N)))
	{
		if (iter == MAX_ITER)
		{
			break;
		}
		*A_cur = *A_next;
		householder_decomposition(*A_cur, R, Q);
		*A_next = mul_matrix(*R, *Q);
		if (check_diff(*A_cur, *A_next, N))
		{
			break;
		}
		iter++;
	}
}
