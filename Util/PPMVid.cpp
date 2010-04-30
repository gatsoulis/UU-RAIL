/*
 * PPMVid.cpp
 *
 *  Created on: 23-Oct-2008
 *      Author: chrisb
 */

#include "PPMVid.h"


PPMVid::PPMVid(const char *filename, int width, int height, int bpp, float framerate) {
	mode=0;
	initialise(filename,width,height,bpp,framerate);
}

PPMVid::~PPMVid() {
	if (mode!=0)
		fclose(file);
}

void PPMVid::initialise(const char *filename, int width, int height, int bpp, float framerate) {
	file = fopen64(filename, "r");
	if (file) {
		// The file exists so we are a reader
		mode=2;
		fscanf(file,"PPMV2\n%d  %d  %d  %f\n",&this->width,&this->height,&this->bpp,&this->framerate);
		long int datapos = ftell(file);
		frames=0;
		printf("width=%d, height=%d, bpp=%d\n",this->width,this->height,this->bpp);
//		while(fseek(file,this->height*this->width*(this->bpp==PPMV_24BPP?3:1),SEEK_CUR)==0) {
//			frames++;
//		}
//		fseek(file,datapos,SEEK_SET);
	} else {
		if ((width==0)&&(height==0)) {
			// a writer but no sizes given
			mode=0;
			frames=0;
			return;
		}
		file = fopen64(filename, "w");
		fprintf(file,"PPMV2\n%d  %d  %d  %f\n",width,height,bpp,framerate);
		mode=1;
		this->bpp=bpp;
		this->height=height;
		this->width=width;
		this->framerate=framerate;
	}
}

void PPMVid::writeFrame(void *data) {
	if (mode==1)
		fwrite(data,width*height,(bpp==PPMV_8BPP)? 1 : 3,file);
}

void PPMVid::readFrame(void *dataspace) {
	if (mode==2)
		fread(dataspace,width*height,(bpp==PPMV_8BPP)? 1 : 3,file);
}

void PPMVid::readFrame(OcvImage< RGBPixel<unsigned char> > *image) {
	if (mode==2)
		fread(image->iplimage->imageData,width*height,(bpp==PPMV_8BPP)? 1 : 3,file);
}

void PPMVid::close() {
	if (file)
		fclose(file);
	mode=0;
	width=height=bpp=framerate=0;
}

void PPMVid::Flush() {
	fflush(file);
}

bool PPMVid::moreExist() {
	return !(feof(file));
}

int PPMVid::frameCount(){
	return frames;
}






