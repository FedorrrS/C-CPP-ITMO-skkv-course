#pragma once

#ifndef C_2023_M3135_HOUSEHOLDER_H
#define C_2023_M3135_HOUSEHOLDER_H

#include "algebra.h"
#include "matrix.h"
#include "algebra.h"

/* that performs the Householder decomposition of a given matrix */
void householder_decomposition(matrix A, matrix *R, matrix *Q);

#endif
