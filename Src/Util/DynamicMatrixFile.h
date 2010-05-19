/*
 * MatrixFile.h
 *
 *  Created on: 25-Jan2010
 *      Author: chris
 *
 *  Simple class for storing and saving a matrix of tab seperated values of dynamic row number
 *  No loading capability - use MatrixFile for that.
 *
 *  File format:
 *  MAT
 *  height width
 *  comment line (40 chars)
 *  data row...
 *  data row...
 */

#ifndef DYNAMICMATRIXFILE_H_
#define DYNAMICMATRIXFILE_H_

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>

class DynamicMatrixFile {
public:

	/**
	 *  Constructor to create a new empty matrix of given size
	 *  Pre-allocates 'row' rows
	 */
	DynamicMatrixFile(int row, int columns);

	/**
	 *  Save the matrix to a file
	 *  Return: true if success, false otherwise.
	 */
	bool saveToDisk(const char *filename, bool overwrite=false) const;

	/**
	 *  Destructor frees space allocated for data
	 */
	virtual ~DynamicMatrixFile();

	/**
	 *  Data access operator. Contents of the loaded matrix are accessed
	 *  using  [row][column] syntax.
	 */
	float* operator[] (int row);

	inline bool isAlive() const  {return alive;}

	inline int getRows() const { return rows; }

	inline int getColumns() const { return columns; }


private:
	int rows, columns;
//	float *data;
	std::vector<float*> *data;
	char comment[100];
	bool alive; 		// is the object in a usable state?
};

#endif /* DYNAMICMATRIXFILE_H_ */
