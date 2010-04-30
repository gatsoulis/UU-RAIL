/*
 * DynamicMatrixFile.cpp
 *
 *  Created on: 25-Jan-2010
 *      Author: chris
 */

#include "DynamicMatrixFile.h"



DynamicMatrixFile::DynamicMatrixFile(int rows, int columns) {
	data = new std::vector<float*>(rows);
	for (int i=0;i<rows;i++) {
		data->at(i)=new float[columns];
		memset(data->at(i),0,columns*sizeof(float));
	}
	this->rows=rows;
	this->columns=columns;
	alive = true;
}

bool DynamicMatrixFile::saveToDisk(const char *filename, bool overwrite) const {
	if (!alive)
		return false;
	if (!overwrite) {
		std::ifstream file;
		file.open(filename, std::ifstream::in);
		file.close();
		if(!file.fail()) {
			return false;
		}
	}
	std::ofstream file;
	file.open(filename,std::ofstream::out);
	if (file.fail())
		return false;
	file << "MAT" << std::endl << rows << " " << columns << std::endl;
	time_t timenow;
	time(&timenow);
	file << asctime(localtime(&timenow));// << std::endl;
	for (int y=0;y<rows;y++){
		for (int x=0;x<columns;x++) {
			file << data->at(y)[x] << "\t";
		}
		file << std::endl;
	}
	return true;
}

DynamicMatrixFile::~DynamicMatrixFile() {
	if (data)
		delete[] data;
}

float *DynamicMatrixFile::operator [](int row) {
	if (row<rows) {
		return data->at(row);
	} else {
		if (row==rows) {
			data->push_back(new float[columns]);
			rows++;
			return data->at(row);
		}else {
			std::cout << "WARNING: DynamicMatrixFile request for non existent row\n";
		}
	}
}


