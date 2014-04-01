/**
 * @file posit.cpp
 * @authors Mariano Araneda
 * @authors Fabián C. Tommasini
 */

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <signal.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "tracker/wiimote/posit.hpp"

/**
 * Read object feature points from a file
 * @param name
 * @param nbPts
 * @return
 */
double **read_object(const char *name, int *nbPts)
{
	FILE *fp;
	int i, j, res;
	float coord;
	double **objectPts;

	fp = fopen(name, "r");
	if (fp != NULL)
	{
		if (debug_posit)
			printf("Object data read from file:\n");
		res = fscanf(fp, "%d", nbPts);
		if (debug_posit)
			printf("%6d\n", *nbPts);

		/* Initialize objectPts array */
		objectPts = init_double_array(*nbPts, nbObjectCoords);

		for (i = 0; i < *nbPts; i++)
		{
			for (j = 0; j < nbObjectCoords; j++)
			{
				res = fscanf(fp, "%f", &coord);
				objectPts[i][j] = coord;
				if (debug_posit)
					printf("%.2f ", coord);
			}
			if (debug_posit)
				printf("\n");
		}
		if (debug_posit)
			printf("\n");
	}
	else
		show_error((char *) "could not open file of object points");
	return objectPts;
}

/**
 * Initialize data structures for object data
 * @param object
 */
void init_object(object_t *object)
{
	int i, j;
	object->objectVects = init_double_array(object->nbPts, nbObjectCoords);
	object->objectCopy = init_double_array(object->nbPts, nbObjectCoords);

	for (i = 0; i < object->nbPts; i++)
	{
		for (j = 0; j < nbObjectCoords; j++)
		{
			object->objectVects[i][j] = object->objectCopy[i][j]
					= object->objectPts[i][j] - object->objectPts[0][j];
		}
	}

	object->objectMatrix = init_double_array(nbObjectCoords, object->nbPts);
}

/**
 * Initialize data structures for image data
 * @param object
 * @param image
 * @param INpto
 * @param FIpto
 * @param LIpto
 * @param EXpto
 */
void init_image(object_t *object, image_t *image, int INpto[], int FIpto[],
		int LIpto[], int EXpto[])
{
	int i;
	image->nbPts = object->nbPts;
	image->rawImagePts = init_int_array(image->nbPts, nbImageCoords);

	for (i = 0; i < 2; i++)
	{
		image->rawImagePts[0][i] = INpto[i];
		image->rawImagePts[1][i] = LIpto[i];
		image->rawImagePts[2][i] = FIpto[i];
		image->rawImagePts[3][i] = EXpto[i];
	}

	image->imageVects = init_double_array(image->nbPts, nbImageCoords);
	image->oldImageVects = init_double_array(image->nbPts, nbImageCoords);
	image->epsilon = (double *) malloc(image->nbPts * sizeof(double));
}

/**
 * POS function
 * (Pose from Orthography and Scaling, a scaled orthographic proj. approximation).
 * Returns one translation and one rotation.
 * @param object
 * @param image
 * @param camera
 */
void pos(object_t object, image_t image, camera_t *camera)
{
	double    I0[3], J0[3], row1[3], row2[3], row3[3];
	double    I0I0, J0J0;
	int  i, j;
	double scale, scale1, scale2;


	/*Computing I0 and J0, the vectors I and J in TRs listed above */
	for (i=0;i<nbObjectCoords;i++){
    	I0[i]=0;
    	J0[i]=0;
    	for (j=0;j<object.nbPts;j++){
			I0[i]+=object.objectMatrix[i][j]*image.imageVects[j][0];
			J0[i]+=object.objectMatrix[i][j]*image.imageVects[j][1];
    	}
	}

	I0I0=I0[0]*I0[0] + I0[1]*I0[1] + I0[2]*I0[2];
	J0J0=J0[0]*J0[0] + J0[1]*J0[1] + J0[2]*J0[2];

	scale1 = sqrt(I0I0);
	scale2 = sqrt(J0J0);
	scale = (scale1 + scale2) / 2.0;

	/* Computing TRANSLATION */
	camera->translation[0] = image.imagePts[0][0] / scale;
	camera->translation[1] = image.imagePts[0][1] / scale;
	camera->translation[2] = camera->focalLength / scale;
	
	//printf("%f \n", camera->translation[2]);
	//printf("%f \n", scale);

	/* Computing ROTATION */
	for (i = 0; i < 3; i++)
	{
		row1[i] = I0[i] / scale1;
		row2[i] = J0[i] / scale2;
	}

	row3[0] = row1[1] * row2[2] - row1[2] * row2[1];/* Cross-product to obtain third row */
	row3[1] = row1[2] * row2[0] - row1[0] * row2[2];
	row3[2] = row1[0] * row2[1] - row1[1] * row2[0];

	for (i = 0; i < 3; i++)
	{
		camera->rotation[0][i] = row1[i];
		camera->rotation[1][i] = row2[i];
		camera->rotation[2][i] = row3[i];
	}
}

/**
 * Get sum of differences between coordinates in lists
 * of old image points and new image points
 * @param image
 */
long get_image_difference(image_t image)
{
	int i, j;
	long sumOfDiffs = 0;

	for (i = 0; i < image.nbPts; i++)
	{
		for (j = 0; j < 2; j++)
		{
			sumOfDiffs += abs(
					floor(0.5 + image.imageVects[i][j]) - floor(
							0.5 + image.oldImageVects[i][j]));
		}
	}
	return sumOfDiffs;
}

/**
 * Iterate over results obtained by the POS function
 * @see "Model-Based Object Pose in 25 Lines of Code", IJCV 15, pp. 123-141, 1995.
 * @param object
 * @param image
 * @param camera
 */
void posit(object_t object, image_t image, camera_t *camera)
{
	
	
	int i, j, iCount;
	int converged;
	long imageDiff;

	/* Starting point for iteration loop */
	for (iCount = 0; iCount < maxCount; iCount++)
	{
		if (iCount == 0)
		{
			for (i = 0; i < image.nbPts; i++)
			{
				for (j = 0; j < nbImageCoords; j++)
				{
					image.imageVects[i][j] = (double) (image.imagePts[i][j]
							- image.imagePts[0][j]);
				}
			}
		}
		else
		{/* iCount>0 */
			/* Compute new imageVects */
			for (i = 0; i < image.nbPts; i++)
			{
				image.epsilon[i] = 0.0;
				for (j = 0; j < 3; j++)
				{
					image.epsilon[i] += object.objectVects[i][j]
							* camera->rotation[2][j]; /*dot product M0Mi.k*/
				}
				image.epsilon[i] /= camera->translation[2]; /* divide by Z0 */
			}
			/* Corrected image vectors */
			for (i = 0; i < image.nbPts; i++)
			{
				for (j = 0; j < nbImageCoords; j++)
				{
					image.imageVects[i][j] = (double) image.imagePts[i][j] * (1
							+ image.epsilon[i]) - image.imagePts[0][j];
				}
			}
			imageDiff = get_image_difference(image);/*using pts gives same result */
			//~ printf("imageDiff %ld\n",imageDiff);
		}

		/* Remember old imageVects */
		for (i = 0; i < image.nbPts; i++)
		{
			image.oldImageVects[i][0] = image.imageVects[i][0];
			image.oldImageVects[i][1] = image.imageVects[i][1];
		}

		/* Compute rotation and translation */
		pos(object, image, camera);

		converged = (iCount > 0 && imageDiff == 0);

		if (converged)
			break;

		if (iCount == maxCount && !converged)
		{
			printf("POSIT did not converge \n");
		}
	}/*end for*/;
}

/**
 * ????
 * @see http://replay.web.archive.org/20090129120732/http://geocities.com/siliconvalley/2151/math2d.html
 * @param INpto starting point of line
 * @param FIpto ending point of line
 * @param LIpto in-line point
 * @param EXpto out-line point
 * @param setAd
 */
void assing_points(ir2object_points_t *ir2obj, int INpto[], int FIpto[], int LIpto[], int EXpto[])
{
	double rv[2], c[2], pa_p[2];
	double rvn[2], rv2, lamb, d;
	int i;

	rv[0] = (FIpto[0] - INpto[0]);
	rv[1] = (FIpto[1] - INpto[1]);
	pa_p[0] = (LIpto[0] - INpto[0]);
	pa_p[1] = (LIpto[1] - INpto[1]);
	rv2 = rv[0] * rv[0] + rv[1] * rv[1];
	rvn[0] = (double) rv[0] / rv2;
	rvn[1] = (double) rv[1] / rv2;
	lamb = (double) (pa_p[0] * rvn[0]) + (pa_p[1] * rvn[1]);

	for (i = 0; i < 2; i++)
	{
		c[i] = LIpto[i] - (INpto[i] + rv[i] * lamb);

	}

	d = (double) sqrt(c[0] * c[0] + c[1] * c[1]);

	if ((lamb > 0) && (lamb < 1))
	{
		if (d < ir2obj->A_d)
		{
			ir2obj->A_d = d;

			for (i = 0; i < 2; i++)
			{
				ir2obj->A_INpto[i] = INpto[i];
				ir2obj->A_FIpto[i] = FIpto[i];
				ir2obj->A_LIpto[i] = LIpto[i];
				ir2obj->A_EXpto[i] = EXpto[i];
			}
		}
	}
}

/**
 * Determines the Euler angles according to the convention ZYX from the rotation matrix
 * @see	"Planning Algorithms", Steven M. LaValle
 * @param camera
 */
void euler_angles(camera_t *camera)
{
	double r, q, yaw, pitch, roll;
	r = sqrt(
			(camera->rotation[2][1] * camera->rotation[2][1])
					+ (camera->rotation[2][2] * camera->rotation[2][2]));
	q = (-1) * camera->rotation[2][0];
	yaw = -atan2(camera->rotation[1][0], camera->rotation[0][0]);  // negative for negative azimuth at left
	roll = atan2(q, r);
	pitch = atan2(camera->rotation[2][1], camera->rotation[2][2]);

	// to degree
	camera->yaw = ((yaw * 180) / M_PI);
	camera->pitch = ((pitch * 180) / M_PI);
	camera->roll = ((roll * 180) / M_PI);
}

/**
 * Performs a modified moving average filter
 * @param MMAold
 * @param x
 * @return
 */
double moving_average(double MMAold, int x)
{
	double MMAnew;
	int N=1;
		
	MMAnew=(double) ((N-1)*MMAold + x)/N;//modified moving average
	
	return MMAnew;
}

/**
 * Smooth
 * @param image
 */
void smooth(image_t *image, bool first_run)
{
	int i;
	image->imagePts = init_double_array(image->nbPts, nbImageCoords);

	if (first_run)
	{
		image->oldImagePts = init_double_array(image->nbPts, nbImageCoords);
		for (i = 0; i < 2; i++)
		{
			image->oldImagePts[0][i] = image->imagePts[0][i];
			image->oldImagePts[1][i] = image->imagePts[1][i];
			image->oldImagePts[2][i] = image->imagePts[2][i];
			image->oldImagePts[3][i] = image->imagePts[3][i];
		}

	}

	for(i=0;i<2;i++)
	{
		image->imagePts[0][i]=moving_average(image->oldImagePts[0][i],image->rawImagePts[0][i]);
		image->imagePts[1][i]=moving_average(image->oldImagePts[1][i],image->rawImagePts[1][i]);
		image->imagePts[2][i]=moving_average(image->oldImagePts[2][i],image->rawImagePts[2][i]);
		image->imagePts[3][i]=moving_average(image->oldImagePts[3][i],image->rawImagePts[3][i]);
	}

	for(i=0;i<2;i++)
	{
		image->oldImagePts[0][i]=image->imagePts[0][i];
		image->oldImagePts[1][i]=image->imagePts[1][i];
		image->oldImagePts[2][i]=image->imagePts[2][i];
		image->oldImagePts[3][i]=image->imagePts[3][i];
	}

}
