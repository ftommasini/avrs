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
