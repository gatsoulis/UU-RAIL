/*
 * MatrixFile.h
 *
 *  Created on: 24-Jun-2009
 *      Author: chris
 *
 *  Simple class for loading and saving a matrix of tab seperated values
 *  File format:
 *  MAT
 *  height width
 *  comment line (40 chars)
 *  data row...
 *  data row...
 */

#ifndef MATRIXFILE_H_
#define MATRIXFILE_H_

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <time.h>

class MatrixFile {
public:
	/**
	 *  Constructor that loads a matrix from disk
	 */
	MatrixFile(const char *filename);

	/**
	 *  Constructor to create a new empty matrix of given size
	 */
	MatrixFile(int row, int columns);

	/**
	 *  Save the matrix to a file
	 *  Return: true if success, false otherwise.
	 */
	bool saveToDisk(const char *filename, bool overwrite=false) const;

	/**
	 *  Destructor frees space allocated for data
	 */
	virtual ~MatrixFile();

	/**
	 *  Data access operator. Contents of the loaded matrix are accessed
	 *  using  [row][column] syntax.
	 */
	float* operator[] (int row);

	inline bool isAlive() const  {return alive;}

	inline int getRows() const { return rows; }

	inline int getColumns() const { return columns; }

	inline float* getData() { return data; }

private:
	int rows, columns;
	float *data;
	char comment[100];
	bool alive; 	// is the object in a useable state?
};

#endif /* MATRIXFILE_H_ */
