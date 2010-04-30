/*
 * PCA.cpp
 *
 *  Created on: 30-Dec-2008
 *      Author: chris
 */

#include "PCA.h"


PCA::PCA(int maxVectors, int vectorSize) {
	maximumVectors=maxVectors;
	this->vectorSize=vectorSize;
	numberVectors=0;
	// Allocate the space for storing the vectors
	// Vectors are stored in BTranspose so that they can simply be stacked
	// on top of each other as row vectors
	m_dataBTrans = new float[maxVectors*vectorSize];
	eigensComputed=false;
	dataFreed=false;
}

PCA::PCA(const char *filename) {
	loadFromFile(filename);
};

void PCA::addVector(float *vector) {
#ifdef SAFETY
	if ((eigensComputed)  || (numberVectors==maximumVector)) {
		printf("Trying to add a vector to PCA class: too many vectors or class already evaluated.");
		return;
	}
#endif
	memcpy(m_dataBTrans+numberVectors*vectorSize,vector,vectorSize*sizeof(float));
	numberVectors++;
//		printf("Vector added: %f, %f; total vectors=%d\n",vector[0],vector[1],numberVectors);
}

void PCA::calculateEigens(bool constructUsingTranspose) {
	int position, i,j, vect;

	// Stage 1: Calculate the average vector
	m_dataAverageVector = new float[vectorSize];
	memset(m_dataAverageVector,0,vectorSize*sizeof(float));
	position=0;
	for (vect=0;vect<numberVectors;vect++) {
//		printf ("Addving vector (pos=%d)\n   ",position);
		for (i=0;i<vectorSize;i++) {
//			printf("%f\t",m_dataBTrans[position]);
			m_dataAverageVector[i]+=m_dataBTrans[position];
			position++;
		}
//		printf("\n");
	}
	for (i=0;i<vectorSize;i++) {
		m_dataAverageVector[i]/=numberVectors;
	}
	a = cvMat(vectorSize,1,CV_32FC1,m_dataAverageVector);

	// Stage 2: Subtract the average vector from each vector
	position=0;
	for (vect=0;vect<numberVectors;vect++) {
		for (i=0;i<vectorSize;i++) {
			m_dataBTrans[position] -= m_dataAverageVector[i];
			position++;
		}
	}

	// output temp
//	/for(i=0;i<vectorSize;i++) {
//		printf("%f\t",m_dataAverageVector[i]);
//	}
//	printf("\n");
//	for (i=0;i<numberVectors;i++){
//		printf("Vector minus average %d : %f, %f\n",i,m_dataBTrans[i*2],m_dataBTrans[i*2+1]);
//	}
///////
	// Stage 3: For the B and BTranspose matrixes
//	printf("Stage 3...\n");
	CvMat BTranspose = cvMat(numberVectors, vectorSize, CV_32FC1, m_dataBTrans);
	float *m_dataB = new float[numberVectors*vectorSize];
	CvMat B = cvMat(vectorSize,numberVectors, CV_32FC1, m_dataB);
	cvTranspose(&BTranspose, &B);

//	printf("BTrans=\n");
//	position=0;
//	for (i=0;i<numberVectors;i++) {
//		for (j=0;j<vectorSize;j++) {
//			printf("%f\t",m_dataBTrans[position]);
//
//			position++;
//		}
//		printf("\n");
//	}
//
//	printf("B=\n");
//		position=0;
//		for (i=0;i<vectorSize;i++) {
//			for (j=0;j<numberVectors;j++) {
//				printf("%f\t",m_dataB[position]);
//
//				position++;
//			}
//			printf("\n");
//		}



//	printf("Stage 4...\n");
	// Stage 4: Calculate the covariance matrix R, or the R squiggle for efficiency when
	// vastly less vectors than vector size
	if (constructUsingTranspose) {
		// R_Tild
		float *m_dataR = new float[numberVectors*numberVectors];
		CvMat R = cvMat(numberVectors,numberVectors,CV_32FC1,m_dataR);
		cvMatMul(&BTranspose,&B,&R);

//		printf("Stage 5...\n");
		float *m_dataEigenValuesTild = new float[numberVectors];
		float *m_dataEigenVectorsTild = new float[numberVectors*numberVectors];
		CvMat ETild  = cvMat(numberVectors,numberVectors,CV_32FC1,m_dataEigenVectorsTild);
		CvMat lTild  = cvMat(numberVectors,1,CV_32FC1,m_dataEigenValuesTild);
		cvEigenVV(&R, &ETild, &lTild);  // l = eigenvalues of R (descending order)

		// Convert eigenvectors & values to correct ones
		m_dataEigenValues = new float[vectorSize];
		m_dataEigenVectors = new float[vectorSize*vectorSize];
		CvMat E  = cvMat(vectorSize,vectorSize,CV_32FC1,m_dataEigenVectors);
		CvMat l  = cvMat(vectorSize,1,CV_32FC1,m_dataEigenValues);

		// Eigenvalues
		for (i=0; i<numberVectors;i++) {
			m_dataEigenValues[i]=m_dataEigenValuesTild[i];
		}
		// Eigenvectors
		for (i=0; i<numberVectors;i++) {
			CvMat eigenvectorTild = cvMat(numberVectors,1,CV_32FC1,m_dataEigenVectorsTild+(i*numberVectors));
			CvMat eigenvector = cvMat(vectorSize,1,CV_32FC1,m_dataEigenVectors+ (i*vectorSize));
			cvMatMul(&B,&eigenvectorTild,&eigenvector);
			float sqrtEigVal = (fabs(m_dataEigenValuesTild[i])>=1e-5) ? 1.0/sqrt(m_dataEigenValuesTild[i]) : 0.0;
//			printf("sqrtEigVal=%f  (eigV=%f)\n",sqrtEigVal,m_dataEigenValuesTild[i]);
			for (j=0;j<vectorSize;j++) {
				m_dataEigenVectors[i*vectorSize + j]*=sqrtEigVal;
			}
		}

		delete m_dataR;
		delete []m_dataEigenValuesTild;
//		delete []m_dataEigenVectorsTild;



	} else {

		float *m_dataR = new float[vectorSize*vectorSize];
		CvMat R = cvMat(vectorSize,vectorSize,CV_32FC1,m_dataR);
		cvMatMul(&B,&BTranspose,&R);


//		printf("Stage 5...\n");
		// Stage 5: Calculate the eigenvalues and vectors of R
		m_dataEigenValues = new float[vectorSize];
		m_dataEigenVectors = new float[vectorSize*vectorSize];
		CvMat E  = cvMat(vectorSize,vectorSize,CV_32FC1,m_dataEigenVectors);
		CvMat l  = cvMat(vectorSize,1,CV_32FC1,m_dataEigenValues);
//		printf("Now eigendecomposing...\n");
	//	void cvSVD( CvArr* A, CvArr* W, CvArr* U=NULL, CvArr* V=NULL, int flags=0 );

		cvEigenVV(&R, &E, &l);  // l = eigenvalues of R (descending order)
	                        // E = corresponding eigenvectors (rows)
		delete m_dataR;
//		cvReleaseMat(&&R);
	}
	// Now delete un needed data
	eigensComputed=true;
	delete m_dataB;

	printf("Eigenvalues=\n");
	for (int i=0; i<vectorSize;i++) {
		printf("%f\n",m_dataEigenValues[i]);
	}
//	printf("\n\nEigvectors=\n");
//	position=0;
//	for (i=0;i<vectorSize;i++) {
//		for (j=0;j<vectorSize;j++) {
//			printf("%f\t",m_dataEigenVectors[position]);
//
//			position++;
//		}
//		printf("\n");
//	}printf("\n");

}

void PCA::projectVector(float *vector, int numberOfBasis, float *projected) {
	float *vectorMinusA = new float[vectorSize];
	for (int i=0;i<vectorSize;i++) {
		vectorMinusA[i]=vector[i]-m_dataAverageVector[i];
	}

	projectAverageSubtracted(vector,numberOfBasis,projected);
	delete vectorMinusA;
}

void PCA::projectAverageSubtracted(float *vector, int numberOfBasis, float *projected) {
	CvMat e;
	CvMat I = cvMat(vectorSize,1,CV_32FC1,vector);
	for (int base=0;base<numberOfBasis;base++) {
		e=cvMat(vectorSize,1,CV_32FC1,m_dataEigenVectors+base*vectorSize);
		projected[base]=(float)cvDotProduct(&e,&I);
	}
}

void PCA::reconstructVector(float *projected, int numberOfBasis, float *vector){
	memset(vector,0,vectorSize*sizeof(float));
	for (int base=0;base<numberOfBasis;base++) {
		for (int position=0;position<vectorSize;position++) {
			vector[position]+=projected[base]*m_dataEigenVectors[base*vectorSize + position];
		}
	}
	for (int position=0;position<vectorSize;position++) {
		vector[position]+=m_dataAverageVector[position];
	}
}


/************************************************************************
 * 	Serialisation
 ************************************************************************/
void PCA::saveToFile(char *filename )  {
	std::ofstream file(filename);
	boost::archive::text_oarchive ar(file);

	ar & maximumVectors;
	ar & numberVectors;
	ar & vectorSize;
	ar & dataFreed;
	ar & eigensComputed;

	if (!dataFreed) {
		// write the data
		for (int i=0; i<maximumVectors*vectorSize;i++) {
			ar & m_dataBTrans[i];
		}
	}

	if (eigensComputed) {
		// write the eigenval, eigenvectors and average vector
		for (int i=0;i<vectorSize;i++) {
			ar & m_dataAverageVector[i];
		}

		for (int i=0;i<vectorSize;i++) {
			ar & m_dataEigenValues[i];
		}

		for (int i=0;i<vectorSize*vectorSize;i++) {
			ar & m_dataEigenVectors[i];
		}
	}
	file.close();
}

void PCA::loadFromFile(const char *filename ) {
	std::ifstream file(filename);
	boost::archive::text_iarchive ar(file);

	ar & maximumVectors;
	ar & numberVectors;
	ar & vectorSize;
	ar & dataFreed;
	ar & eigensComputed;

	if (!dataFreed) {
		// load the data
		m_dataBTrans = new float[maximumVectors*vectorSize];
		for (int i=0; i<maximumVectors*vectorSize;i++) {
			ar & m_dataBTrans[i];
		}
	}

	if (eigensComputed) {
		// Load the eigenval, eigenvectors and average vector
		m_dataAverageVector = new float[vectorSize];
		for (int i=0;i<vectorSize;i++) {
			ar & m_dataAverageVector[i];
		}
		a = cvMat(vectorSize,1,CV_32FC1,m_dataAverageVector);

		m_dataEigenValues = new float[vectorSize];
		for (int i=0;i<vectorSize;i++) {
			ar & m_dataEigenValues[i];
		}

		m_dataEigenVectors = new float[vectorSize*vectorSize];
		for (int i=0;i<vectorSize*vectorSize;i++) {
			ar & m_dataEigenVectors[i];
		}
	}
	file.close();
}



