/**
 * @file posit.h
 */

#ifndef POSIT_HPP_
#define POSIT_HPP_

extern "C"
{
#include "svd.h"
}

// Constants
const int maxCount = 30; /* exit iteration with a message after this many loops */
const int nbObjectCoords = 3; /* x, y, z */
const int nbImageCoords = 2; /* x, y */

// Structures
typedef struct
{
	int nbPts;
	double **objectPts;		///< Array of coordinates of points
	double **objectVects;	///< Array of coordinates of vectors from reference pt to all pts
	double **objectCopy;  	///< Copy of objectVects, used because SVD code destroys input data
	double **objectMatrix;	///< Pseudo-inverse of objectVects
} object_t;

typedef struct
{
	int nbPts;
	int imageCenter[2];		///< Image center coordinates
	int **rawImagePts;		///< Camera points
	double **imagePts;		///< Averaged points
	double **oldImagePts;	///< Previous capture averaged points
	double **imageVects;	///< Scaled orthographic projections
	double **oldImageVects;	///< Projections at previous iteration step
	double *epsilon;		///< Corrections to scaled orthographic projections at each iteration
} image_t;

typedef struct
{
	int focalLength;
	double rotation[3][3];	///< Rotation of SCENE in camera reference, NOT other way around
	double translation[3];	///< Translation of SCENE in camera reference
	double yaw;				///< Counterclockwise rotation about the z axis
	double pitch;			///< Counterclockwise rotation about the y axis
	double roll;			///< Counterclockwise rotation about the x axis
//	double refyaw;			///< Counterclockwise rotation about the z axis of reference position
//	double refpitch;		///< Counterclockwise rotation about the y axis of reference position
//	double refroll;			///< Counterclockwise rotation about the x axis of reference position
} camera_t;

// IR dots ID
typedef struct
{
	int A_INpto[2];
	int A_FIpto[2];
	int A_LIpto[2];
	int A_EXpto[2];
	double A_d;  // any big big value
} ir2object_points_t;

// Prototypes
double **read_object(const char *name, int *nbPts);
void init_object(object_t *object);
void init_image(object_t *object, image_t *image, int INpto[], int FIpto[],
		int LIpto[], int EXpto[]);
long get_image_difference(image_t image);
void pos(object_t object, image_t image, camera_t *camera);
void posit(object_t object, image_t image, camera_t *camera);
void assing_points(ir2object_points_t *ir2obj, int INpto[], int FIpto[], int LIpto[], int EXpto[]);
void euler_angles(camera_t *camera);
double moving_average(double MMAold, int x);
void smooth(image_t *image, bool first_run);

// Global variables
static int debug_posit = 0;/* 0 for no printout */
//static bool first_run = true;

#endif  // POSIT_HPP_
