/*
 * OCamCal.cpp
 *
 *  Created on: 29 Jan 2010
 *      Author: chris
 */

#include "OCamCal.h"
OCamCal::OCamCal(const char *params_file, float sf) {
	get_ocam_model(&o, params_file);
	mapx = cvCreateMat(o.height, o.width, CV_32FC1);
	mapy = cvCreateMat(o.height, o.width, CV_32FC1);

	create_perspecive_undistortion_LUT( mapx, mapy, &o, sf );

}

void OCamCal::undistort(IplImage *image, IplImage *destination, int mappingFlags) {
	/* --------------------------------------------------------------------*/
	  /* Undistort using specified interpolation method                      */
	  /* Other possible values are (see OpenCV doc):                         */
	  /* CV_INTER_NN - nearest-neighbor interpolation,                       */
	  /* CV_INTER_LINEAR - bilinear interpolation (used by default)          */
	  /* CV_INTER_AREA - resampling using pixel area relation. It is the
			preferred method for image decimation that gives moire-free results.
			In case of zooming it is similar to CV_INTER_NN method. 			*/
	  /* CV_INTER_CUBIC - bicubic interpolation.                             */
	  /* --------------------------------------------------------------------*/
	// TODO: A check here that the image sizes match the calibration would be
	// nice
	cvRemap( image, destination, mapx, mapy, mappingFlags, cvScalarAll(0) );
}

OCamCal::~OCamCal() {
	cvReleaseMat(&mapx);
	cvReleaseMat(&mapy);
}
