/**
 * @file svd.c
 * @brief Based on ...
 * @see "Numerical Recipies", W.H. Press, B.P. Flannery, S.A. Teukolsky, et W.T. Vetterling, Cambridge University Press 1993
 */

/// TODO build a class for this...

#include "svd.h"

/**
 * Shows an error and exit
 * @param error_text text of error
 */
void show_error(char *error_text)
{
	printf("%s\n", error_text);
	printf("...now exiting to system...\n");
	exit(1);
}

/**
 * Allocation of memory
 * @param nl
 * @param nh
 * @return
 */
double *vector(int nl, int nh)
{
	double *v;
	v = (double *) malloc((unsigned) (nh - nl + 1) * sizeof(double));
	if (!v)
		show_error("allocation failure in vector()");
	return v - nl;
}

/**
 * Deallocation of memory
 * @param v
 * @param nl
 */
void free_vector(double *v, int nl)
{
	free((char*) (v + nl));
}

double pythag(double a, double b)
{
	double absa, absb;

	absa = fabs(a);
	absb = fabs(b);
	if (absa > absb)
		return absa * sqrt(1.0 + SQR(absb/absa));
	else
		return (absb == 0.0 ? 0.0 : absb * sqrt(1.0 + SQR(absa/absb)));
}

/**
 * Allocate memory for a 2D array of double
 * @param nbRows number of rows
 * @param nbCols number of columns
 * @return 2D array of double
 */
double **init_double_array(int nbRows, int nbCols)
{

	int i;
	double **anArray;

	anArray = (double **) malloc(nbRows * sizeof(double *));

	if (!anArray)
		show_error("allocation failure in init_double_array");

	for (i = 0; i < nbRows; i++)
	{
		anArray[i] = (double *) malloc(nbCols * sizeof(double));

		if (!anArray[i])
			show_error("allocation failure in init_double_array");
	}

	return anArray;
}

/**
 * Allocate memory for a 2D array of int
 * @param nbRows number of rows
 * @param nbCols number of columns
 * @return 2D array of int
 */
int **init_int_array(int nbRows, int nbCols)
{
	int i;
	int **anArray;

	anArray = (int **) malloc(nbRows * sizeof(int *));
	if (!anArray)
		show_error("allocation failure in init_int_array");
	for (i = 0; i < nbRows; i++)
	{
		anArray[i] = (int *) malloc(nbCols * sizeof(int));
		if (!anArray[i])
			show_error("allocation failure in init_int_array");
	}
	return anArray;
}

/**
 * Deallocate memory for 2D array of double
 * @param anArray
 * @param nbRows
 */
void free_double_array(double **anArray, int nbRows)
{
	int i;

	for (i = 0; i < nbRows; i++)
	{
		free(anArray[i]);
	}
}

/**
 * Deallocate memory for 2D array of int
 * @param anArray
 * @param nbRows
 */
void free_int_array(int **anArray, int nbRows)
{
	int i;

	for (i = 0; i < nbRows; i++)
	{
		free(anArray[i]);
	}
}

/**
 * Print a matrix
 * @param mat
 * @param nbRows
 * @param nbCols
 */
void print_matrix(double **mat, int nbRows, int nbCols)
{
	int i, j;
	for (i = 0; i < nbRows; i++)
	{
		for (j = 0; j < nbCols; j++)
		{
			printf("%.5f ", mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

/**
 * Print a vector
 * @param vect
 * @param nbElems
 */
void print_vector(double vect[], int nbElems)
{
	int i;
	for (i = 0; i < nbElems; i++)
	{
		printf("%.2f ", vect[i]);
	}
	printf("\n");
}

/**
 *  Given a matrix a with dimension m x n, get singular value decomposition,
 *  a=U.W.(V)t. Matrix U replaces a in output. Diagonal matrix W is provided as vector w.
 *  m must be larger than n or equal to it. If not, user must pad with zeroes.
 * @param a
 * @param m
 * @param n
 * @param w
 * @param v
 */
void svdcmp(double **a, int m, int n, double w[], double **v)
{
	int flag, i, its, j, jj, k, l, nm;
	double c, f, h, s, x, y, z;
	double anorm = 0.0, g = 0.0, scale = 0.0;
	double *rv1, *vector();
	//double myNb = 1;

	if (m < n)
		show_error("SVDCMP: You must augment A with extra zero rows");
	rv1 = vector((int) (1), n);
	for (i = 1; i <= n; i++)
	{
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i <= m)
		{
			for (k = i; k <= m; k++)
				scale += fabs(a[k - 1][i - 1]);
			if (scale)
			{
				for (k = i; k <= m; k++)
				{
					a[k - 1][i - 1] /= scale;
					s += a[k - 1][i - 1] * a[k - 1][i - 1];
				}
				f = a[i - 1][i - 1];
				g = -SIGN(sqrt(s),f);
				h = f * g - s;
				a[i - 1][i - 1] = f - g;
				if (i != n)
				{
					for (j = l; j <= n; j++)
					{
						for (s = 0.0, k = i; k <= m; k++)
							s += a[k - 1][i - 1] * a[k - 1][j - 1];
						f = s / h;
						for (k = i; k <= m; k++)
							a[k - 1][j - 1] += f * a[k - 1][i - 1];
					}
				}
				for (k = i; k <= m; k++)
					a[k - 1][i - 1] *= scale;
			}
		}
		w[i - 1] = scale * g;
		g = s = scale = 0.0;
		if (i <= m && i != n)
		{
			for (k = l; k <= n; k++)
				scale += fabs(a[i - 1][k - 1]);
			if (scale)
			{
				for (k = l; k <= n; k++)
				{
					a[i - 1][k - 1] /= scale;
					s += a[i - 1][k - 1] * a[i - 1][k - 1];
				}
				f = a[i - 1][l - 1];
				g = -SIGN(sqrt(s),f);
				h = f * g - s;
				a[i - 1][l - 1] = f - g;
				for (k = l; k <= n; k++)
					rv1[k] = a[i - 1][k - 1] / h;
				/*	if (i != m) { */
				for (j = l; j <= m; j++)
				{
					for (s = 0.0, k = l; k <= n; k++)
						s += a[j - 1][k - 1] * a[i - 1][k - 1];
					for (k = l; k <= n; k++)
						a[j - 1][k - 1] += s * rv1[k];
				}
				/*	}*/
				for (k = l; k <= n; k++)
					a[i - 1][k - 1] *= scale;
			}
		}
		anorm = MAX(anorm,(fabs(w[i-1])+fabs(rv1[i])));
	}
	for (i = n; i >= 1; i--)
	{
		if (i < n)
		{
			if (g)
			{
				for (j = l; j <= n; j++)
					v[j - 1][i - 1] = (a[i - 1][j - 1] / a[i - 1][l - 1]) / g;
				for (j = l; j <= n; j++)
				{
					for (s = 0.0, k = l; k <= n; k++)
						s += a[i - 1][k - 1] * v[k - 1][j - 1];
					for (k = l; k <= n; k++)
						v[k - 1][j - 1] += s * v[k - 1][i - 1];
				}
			}
			for (j = l; j <= n; j++)
				v[i - 1][j - 1] = v[j - 1][i - 1] = 0.0;
		}
		v[i - 1][i - 1] = 1.0;
		g = rv1[i];
		l = i;
	}
	for (i = n; i >= 1; i--)
	{
		l = i + 1;
		g = w[i - 1];
		if (i < n)
			for (j = l; j <= n; j++)
				a[i - 1][j - 1] = 0.0;
		if (g)
		{
			g = 1.0 / g;
			if (i != n)
			{
				for (j = l; j <= n; j++)
				{
					for (s = 0.0, k = l; k <= m; k++)
						s += a[k - 1][i - 1] * a[k - 1][j - 1];
					f = (s / a[i - 1][i - 1]) * g;
					for (k = i; k <= m; k++)
						a[k - 1][j - 1] += f * a[k - 1][i - 1];
				}
			}
			for (j = i; j <= m; j++)
				a[j - 1][i - 1] *= g;
		}
		else
		{
			for (j = i; j <= m; j++)
				a[j - 1][i - 1] = 0.0;
		}
		++a[i - 1][i - 1];
	}
	for (k = n; k >= 1; k--)
	{
		for (its = 1; its <= 30; its++)
		{
			flag = 1;
			for (l = k; l >= 1; l--)
			{
				nm = l - 1;
				if (fabs(rv1[l]) + anorm == anorm)
				{
					flag = 0;
					break;
				}
				if (fabs(w[nm]) + anorm == anorm)
					break;
			}
			if (flag)
			{
				c = 0.0;
				s = 1.0;
				for (i = l; i <= k; i++)
				{
					f = s * rv1[i];
					if (fabs(f) + anorm != anorm)
					{
						g = w[i - 1];
						h = pythag(f, g);
						w[i - 1] = h;
						h = 1.0 / h;
						c = g * h;
						s = (-f * h);
						for (j = 1; j <= m; j++)
						{
							y = a[j - 1][nm - 1];
							z = a[j - 1][i - 1];
							a[j - 1][nm - 1] = y * c + z * s;
							a[j - 1][i - 1] = z * c - y * s;
						}
					}
				}
			}
			z = w[k - 1];
			if (l == k)
			{
				if (z < 0.0)
				{
					w[k - 1] = -z;
					for (j = 1; j <= n; j++)
						v[j - 1][k - 1] = (-v[j - 1][k - 1]);
				}
				break;
			}
			if (its == 50)
				show_error("No convergence in 50 SVDCMP iterations");
			x = w[l - 1];
			nm = k - 1;
			y = w[nm - 1];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = pythag(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g,f))) - h)) / x;
			c = s = 1.0;
			for (j = l; j <= nm; j++)
			{
				i = j + 1;
				g = rv1[i];
				y = w[i - 1];
				h = s * g;
				g = c * g;
				z = pythag(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y = y * c;
				for (jj = 1; jj <= n; jj++)
				{
					x = v[jj - 1][j - 1];
					z = v[jj - 1][i - 1];
					v[jj - 1][j - 1] = x * c + z * s;
					v[jj - 1][i - 1] = z * c - x * s;
				}
				z = pythag(f, h);
				w[j - 1] = z;
				if (z)
				{
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = (c * g) + (s * y);
				x = (c * y) - (s * g);
				for (jj = 1; jj <= m; jj++)
				{
					y = a[jj - 1][j - 1];
					z = a[jj - 1][i - 1];
					a[jj - 1][j - 1] = y * c + z * s;
					a[jj - 1][i - 1] = z * c - y * s;
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k - 1] = x;
		}
	}
	free_vector(rv1, (int) (1));
}


/**
 *  B returns the pseudoinverse of A (A with dimension N rows x 3 columns).
 *  It is the matrix v.[diag(1/wi)].(u)t (cf. svdcmp())
 *  Function returns True if B has maximum rank, False otherwise
 * @param A
 * @param N
 * @param B
 * @return
 */
int pseudo_inverse(double **A, int N, double **B)
{
	void svdcmp();
	double **V, temp[3][3];
	double W[3];
	double WMAX;
	double TOL = 0.01;
	int i, j, k;
	int isMaxRank = 1;/* stays true if no singular value under tolerance level */

	 /*allocations*/
	V = init_double_array(nbCoords, nbCoords);

	/*Singular value decomposition*/
	if (debug_svd)
		print_matrix(A, N, nbCoords);

	svdcmp(A, N, nbCoords, W, V);

	if (debug_svd)
	{
		print_matrix(A, N, nbCoords);
		print_matrix(V, nbCoords, nbCoords);
		print_vector(W, nbCoords);
	}
	/*Getting largest singular value*/
	WMAX = 0.0;
	for (i = 0; i < nbCoords; i++)
	{
		if (W[i] > WMAX)
			WMAX = W[i];
	}

	/*Checking for signular values smaller than TOL times the largest one*/
	for (i = 0; i < nbCoords; i++)
	{
		if (W[i] < TOL * WMAX)
		{
			W[i] = 0;
			isMaxRank = 0;
			return isMaxRank;
		}
	}

	if (isMaxRank)
	{
		/*Computing B*/
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				temp[i][j] = V[i][j] / W[j];
			}
		}
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < N; j++)
			{
				B[i][j] = 0.0;
				for (k = 0; k < 3; k++)
				{
					B[i][j] += temp[i][k] * A[j][k];
				}
			}
		}
		if (debug_svd)
			print_matrix(B, nbCoords, N);

		/*deallocations*/
		for (i = 0; i < nbCoords; i++)
			free(V[i]);

		return isMaxRank;
	}

	return isMaxRank;
}

