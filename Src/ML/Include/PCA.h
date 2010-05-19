/*
 * PCA.h
 *
 *  Created on: 30-Dec-2008
 *      Author: chris
 */

#ifndef PCA_H_
#define PCA_H_
#include <boost/config.hpp>
#include "../Libs/opencv/include/cv.h"
#include "../Libs/opencv/include/cxcore.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

//#define SAFETY

class PCA {
public:
	/**
	 * Constructor takes the number of vectors that will be used and their size
	 * as input. It allocates a block of memory for storing the vectors in.
	 * If more vectors than the maxVectors parameter supplied here are used
	 * then the class will cause a segmentation fault.
	 */
	PCA(int maxVectors, int vectorSize);

	/**
	 *  Constructor to load from a file
	 */
	PCA(const char *filename) ;

	~PCA() {

	}

	/**
	 * 	Add a vector to the system
	 */
	void addVector(float *vector);

	/**
	 * Calculates the eignevectors and eigenvalues
	 * Allocates memory for storing the vectors/values and releases the memory
	 * taken by the intermediate data vectors B and BTranspose. The original data
	 * is kept, but can be freed with by user.
	 */
	void calculateEigens(bool constructUsingTranspose = false);

	/**
	 * Project the supplied vector into the reduced dimensional space
	 * The projected vector storage space should be pre-allocated outside of this
	 * function, with a size equal to the numberOfBasis
	 */
	void projectVector(float *vector, int numberOfBasis,float *projected);
	void projectAverageSubtracted(float *vector, int numberOfBasis, float *projected);
	inline void projectOriginal(int vectorNumber,int numberOfBasis,float *projected) {
#ifdef SAFETY
		if (dataFreed) return;
#endif
		projectAverageSubtracted(m_dataBTrans+vectorNumber*vectorSize,numberOfBasis,projected);
	}


	/**
	 * Reconstruct the original vector based on the projection into the eigenspace.
	 * The vector will be reconstructed into *vector, which must be allocated to the
	 * correct size outside of the function.
	 */
	void reconstructVector(float *projected, int numberOfBasis, float *vector);

	/**
	 * 	Return the array of eigenvalues
	 */
	inline float* getEigenValues() {
		return m_dataEigenValues;
	}

	/**
	 * 	Return the array of eigenvectors
	 */
	inline float* getEigenVectors() {
		return m_dataEigenVectors;
	}

	/**
	 * 	Return the average vector
	 */
	inline float* getAverage() {
		return m_dataAverageVector;
	}

	/**
	 * Deletes the original data vectors, and frees the memory storage
	 */
	void freeData() {
#ifdef SAFETY
		if (!dataFreed) {
#endif
			delete [] m_dataBTrans;
			dataFreed=true;
#ifdef SAFETY
		}
#endif
	}

	/**
	 *  Return the size of the vectors
	 */
	inline int get_vector_size() {
		return vectorSize;
	}

	/************************************************************************
	 * 	Serialisation
	 ************************************************************************/
	void saveToFile(char *filename );
private:
	void loadFromFile(const char *filename );


private:
    friend class boost::serialization::access;

	int maximumVectors;
	int numberVectors;
	int vectorSize;
	float *m_dataBTrans;		// used also to store original input vectors
	float *m_dataEigenVectors;
	float *m_dataEigenValues;
	float *m_dataAverageVector;
	CvMat a;
	bool eigensComputed;
	bool dataFreed;
};

#endif /* PCA_H_ */
