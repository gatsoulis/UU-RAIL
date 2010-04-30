/*
 * PPMVid.h
 *
 *  Created on: 23-Oct-2008
 *      Author: chrisb
 */

#ifndef PPMVID_H_
#define PPMVID_H_

#include <stdio.h>
#include "../Libs/opencv/include/OcvImage.h"
#define PPMV_8BPP 	1
#define PPMV_24BPP	2
#define VERSION 2

class PPMVid {
public:
	/**
	 *  Constructor just passes on to initialise
	 */
	PPMVid(const char *filename, int width=0,int height=0,int bpp=0,float framerate=0);

	/**
	 *  Destructor simple closes the file if existant.
	 */
	virtual ~PPMVid();

	/**
	 * Initialises the object. If filename exists, uses it and creates a reader, otherwise creates it and
	 * sets up a writer with the supplied sizes.
	 */
	void initialise(const char *filename, int width=0, int height=0, int bpp=0,float framerate=0);

	/**
	 * Write a frame to the file
	 */
	void writeFrame(void *data);

	/**
	 * Read a frame from the file
	 * reads it into the supplied storage space
	 */
	void readFrame(void *dataspace);
        void readFrame(OcvImage< RGBPixel<unsigned char> > *image);

	/**
	 * Closes the file storage
	 */
	void close();

	/**
	 * Flush the file to disk, don't close it
	 */
	void Flush();

	inline int getMode() const    {
        return mode;
    }

	/**
	 *  Count the number of frames in the file
	 */
	int frameCount();

	/**
	 *  Check if there is more data left in the file..
	 */
	bool moreExist();

    int width,height,bpp;
    float framerate;

private:
	FILE *file;
	int mode;	// 0=> inactive; 1=>writer; 2=>reader
	int frames;

};

#endif /* PPMVID_H_ */
