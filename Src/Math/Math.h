/*
 * Math.h
 *
 *  Created on: 08-May-2009
 *      Author: chris
 */

#ifndef MATH_H_
#define MATH_H_
#include <math.h>
#include <iostream>
#include <string.h>
#include <stdio.h>

#define M_2PI 6.28318531

template <class T> T calculateEuclideanDistance(T* vector1, T* vector2, int size ) {
	T distance=0;
	for (int i=0;i<size;i++)
		distance+=pow((vector1[i]-vector2[i]),2);
	distance=sqrt(distance);
	return distance;
}

template <class T> T calculateManhattanDistance(T* vector1, T* vector2, int size ) {
	T distance=0;
	for (int i=0;i<size;i++)
		distance+=fabs(vector1[i]-vector2[i]);
	return distance;
}



template <class T> T vectorMagnetude(T* vector1, int size ) {
	T mag=0;
	for (int i=0;i<size;i++)
		mag+=pow(vector1[i],2);
	return sqrt(mag);
}

template <class T> T vectorMax(T* vector, int size ) {
	T max=-1e15;
	for (int i=0;i<size;i++)
		if (max<vector[i])
			max=vector[i];
	return max;
}

template < class T > std::pair< int, T > vectorMaxComplete(T* vector, int size ) {
	std::pair< int, T > max;
	max.second=-1e15;
	max.first=0;
	for (int i=0;i<size;i++)
		if (max.second<vector[i]){
			max.second=vector[i];
			max.first=i;
		}
	return max;
}

template <class T> T vectorSum(T* vector, int size ) {
	T sum=0;
	for (int i=0;i<size;i++)
		sum+=vector[i];
	return sum;
}

template <class T> T normaliseVector(T* vector1, int size ) {
	T magnetude=vectorMagnetude<T>(vector1,size);
	for (int i=0;i<size;i++)
		vector1[i]/=magnetude;
	return magnetude;
}

template <class T> T normaliseVectorPDF(T* vector1, int size ) {
	T sum=vectorSum<T>(vector1,size);
	for (int i=0;i<size;i++)
		vector1[i]/=sum;
	return sum;
}
/**
 *  ""invert"" the vector ::=>  value = max - value
 */
template <class T> T invertVector(T* vector, int size ) {
	T max=vectorMax(vector,size);
	for (int i=0;i<size;i++)
		vector[i]=max - vector[i];
	return max;
}

/**
 *   Rotate a vector by dth
 *   Must be two dimensional vector (x, y)
 */
template <class T> void vectorRotate(T *vector, float dth){
	T temp;
	temp=cos(dth)*vector[0] - sin(dth)*vector[1];
	vector[1]=sin(dth)*vector[0] + cos(dth)*vector[1];
	vector[0]=temp;
}

/**
 *  Returns the two largest factors of the supplied number
 */
void largest_factors(int x, int *a, int *b);

/**
 *  Fixes the angle supplied (in radians) to the correct range -PI to PI
 */
float angle_fix_rad(float &angle);

/**
 *  Return the fixed angle
 * @param angle
 */
template <  class T > T angle_fix_rad_return(T angle) {
	T ret;
	if (angle>M_PI)
		ret=angle-(2.0*M_PI-angle);
	if (angle<-M_PI)
		ret=2.0*M_PI+angle;
	return ret;
}

/**
 *  Conversions between degrees and radians
 */
inline float deg2rad(float deg) {
	return deg*M_PI/180.0;
}
inline float rad2deg(float rad) {
	return rad*180.0/M_PI;
}

/*!
 *  Calculate the absolute difference between two angles in radians, taking
 *  into account the wrap aroud at pi / -pi.
 * @param a First angle (radians)
 * @param b Second angle (radians)
 * @return absolute difference (radians)
 */
float absolute_angle_difference(float a, float b);

/*!
 *  Calculate the difference between two angles in radians, taking
 *  into account the wrap aroud at pi / -pi.
 * @param a First angle (radians)
 * @param b Second angle (radians)
 * @return absolute difference (radians)
 */
float angle_difference(float a, float b);

/*!
 *
 * Create an array of type T such that each element of the array contains the
 * linear spacing of the values between start and finish
 *
 * @param start		The start point
 * @param finish	The end point
 * @param points	How many sample points
 * @return
 */
template < class T > T* linspace(T start, T finish, int points, T* samples=NULL) {
	if (samples == NULL) {
		printf("Allocating\n");
		samples = new T[points];
	}
	for(int i=0;i<points;i++) {
		samples[i]=(T)i/(T)(points-1) * (finish-start) + start;
	}
	return samples;
}

/**
 * Calculate pascal triangle number
 *
 * @param n the row of the triangle. rows start at 0
 * @param k the entry in the row, starting at 0
 * @return the pascal number
 */
int pascal_number(int n, int k);


template <class T> inline T sign(T number) {
	if (number>0)
		return (T)1;
	else
		return (T)-1;
}

#endif /* MATH_H_ */
