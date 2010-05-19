/*
 * MatrixFile.cpp
 *
 *  Created on: 24-Jun-2009
 *      Author: chris
 */

#include "MatrixFile.h"


MatrixFile::MatrixFile(const char *filename) {
	data=0;
	// Check the input samples file exists and open it
	std::ifstream file;
	file.open(filename, std::ifstream::in);
	if(file.fail()) {
		alive=false;
	} else {
		alive=true;
		char buffer[40];
		file.getline(buffer,20);
		if (buffer[0]!='M' || buffer[1]!='A' || buffer[2]!='T') {
			std::cout << "Warning: MAT file not a MAT file\n";
			std::cout << "buffer=" << buffer[0]<<buffer[1]<<buffer[2]<<std::endl;
			alive=false;
		} else {
			file.getline(buffer,20,' ');
			rows = atoi(buffer);
			file.getline(buffer,20);
			columns = atoi(buffer);
			file.getline(comment,100);
			data = new float[rows*columns];
			for (int y=0;y<rows;y++){
				for (int x=0;x<columns;x++) {
					file >> data[y * columns + x];
				}
			}
		}
	}
	file.close();

}

MatrixFile::MatrixFile(int rows, int columns) {
	data = new float[rows*columns];
	this->rows=rows;
	this->columns=columns;
	memset(data,0,rows*columns*sizeof(float));
	alive = true;
}

bool MatrixFile::saveToDisk(const char *filename, bool overwrite) const {
	if (!alive)
		return false;
	if (!overwrite) {
		// Check the input samples file exists and open it
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
			file << data[y * columns + x] << "\t";
		}
		file << std::endl;
	}
	return true;
}

MatrixFile::~MatrixFile() {
	if (data)
		delete[] data;
}

float *MatrixFile::operator [](int row) {
	return data+row*columns;
}


