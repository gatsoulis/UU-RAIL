/*
 * OCamCal.h
 *
 *  Created on: 29 Jan 2010
 *      Author: chris
 *
 * Class that uses the fisheye calibration results from OCamCalib matlab toolbox
 * and provides perspective unwarping functions.
 *
 * Simply wrapps around the code provided with the toolbox, nothing new.
 *
 */

#ifndef OCAMCAL_H_
#define OCAMCAL_H_

#include "ocam_functions.h"

class OCamCal {
	private:
		struct ocam_model o; // our ocam_models
		CvMat* mapx;
		CvMat* mapy;

	public:
		/**
		 *  Constructor reads the parameters file, creates an unwarping table
		 * @param params_file
		 * @param imageWidth. imageHeight
		 * @param sf Scale factor
		 */
		OCamCal(const char *params_file, float sf=8.0);

		void undistort(IplImage *image, IplImage *destination, int mappingFlags=CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS);

		inline int width() {
			return o.width;
		}
		inline int height() {
			return o.height;
		}
		virtual ~OCamCal();
};

#endif /* OCAMCAL_H_ */
