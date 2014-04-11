/*
 * Copyright (C) 2013-2014 Fabián C. Tommasini <fabian@tommasini.com.ar>
 *
 * Copyright (C) 2009-2012 Mariano Araneda
 *                         Fabián C. Tommasini <fabian@tommasini.com.ar>
 *  *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 *
 */

/**
 * @file svd.h
 */

#ifndef SVD_H_
#define SVD_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Prototypes
void show_error(char error_text[]);
double *vector(int nl, int nh);
void free_vector(double *v, int nl);
double **init_double_array(int nbRows, int nbCols);
int **init_int_array(int nbRows, int nbCols);
void free_double_array(double **anArray, int nbRows);
void free_int_array(int **anArray, int nbRows);
double pythag(double a, double b);
void print_matrix(double **mat, int nbRows, int nbCols);
void print_vector(double vect[], int nbElems);
void svdcmp(double **a, int m, int n, double *w, double **v);
int pseudo_inverse(double **A, int N, double **B);

// Global variables
static double sqrarg;
//static double at, bt, ct;
static double maxarg1, maxarg2;
static int debug_svd = 0; /* 0 to avoid printing */
static const int nbCoords = 3;

// Defines
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg * sqrarg)

#define MAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
	(maxarg1) : (maxarg2))

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

#endif  // SVD_H_
