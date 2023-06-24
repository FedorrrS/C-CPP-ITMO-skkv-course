#include "include/matrix.h"
#include "include/qr_algorithm.h"

#include <string.h>

int main(int argc, char *argv[])
{
	int result = SUCCESS;
	size_t N;
	FILE *input_file = NULL;
	FILE *output_file = NULL;
	matrix R = NULL, Q = NULL, A_cur = NULL, A_next = NULL;

	/* working with input */
	if (argc != 3)
	{
		fprintf(stderr, "IOError: Given %d arguments to command line, expected 2\n", argc);
		result = ERROR_PARAMETER_INVALID;
		goto cleanup_input;
	}

	input_file = fopen(argv[1], "r");
	if (!input_file)
	{
		fprintf(stderr, "IOError: Error when opening a file\n");
		result = ERROR_CANNOT_OPEN_FILE;
		goto cleanup_input;
	}
	/* fscanf returns the number of input items successfully matched and assigned */
	if (fscanf(input_file, "%zd", &N) != 1)
	{
		fprintf(stderr, "IOError: Error reading matrix size from input file\n");
		result = ERROR_DATA_INVALID;
		goto cleanup_input;
	}

	/* working with VLA */
	{
		/* declaring A as a pointer to an N arrays of N db's */
		double(*A)[N] = malloc(sizeof(double[N]) * N);
		if (!A)
		{
			fprintf(stderr, "Can't allocate memory\n");
			result = ERROR_OUT_OF_MEMORY;
			goto cleanup_matrices;
		}
		for (size_t i = 0; i < N; ++i)
		{
			for (size_t j = 0; j < N; ++j)
			{
				if (fscanf(input_file, "%lf", &A[i][j]) != 1)
				{
					fprintf(stderr, "IOError: Error reading matrix element from input file\n");
					result = ERROR_PARAMETER_INVALID;
					goto cleanup_matrices;
				}
			}
		}
		output_file = fopen(argv[2], "w");
		if (!output_file)
		{
			fprintf(stderr, "Error when opening a file\n");
			result = ERROR_CANNOT_OPEN_FILE;
			goto cleanup_matrices;
		}
		if (N == 1)
		{
			fprintf(output_file, "%g\n", A[0][0]);
		}
		else if (N == 2)
		{
			write_complex(output_file, A[0][0], A[1][0], A[0][1], A[1][1]);
		}
		else
		{
			A_cur = create_matrix(N);
			copy_matrix(A_cur, N, A);
			if (!A_cur)
			{
				fprintf(stderr, "Can't allocate memory for matrix\n");
				result = ERROR_OUT_OF_MEMORY;
				goto cleanup_matrices;
			}
			if (is_diagonal(A_cur, N))
			{
				write_diagonal(output_file, A_cur);
			}
			else
			{
				A_next = A_cur;
				qr_algorithm(&R, &Q, &A_cur, &A_next, N);
				extract_eigenvalues(A_next, output_file, N);
			}
		}

cleanup_matrices:
		free(A);
		if (A_next)
		{
			delete_matrix(A_next);
		}
		if (A_cur)
		{
			delete_matrix(A_cur);
		}
		if (R)
		{
			delete_matrix(R);
		}
		if (Q)
		{
			delete_matrix(Q);
		}
		if (output_file)
		{
			fclose(output_file);
		}
	}

cleanup_input:
	if (input_file)
	{
		fclose(input_file);
	}
	return result;
}
